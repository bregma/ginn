/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Chase Douglas <chase.douglas@canonical.com>
 * Author: Mohamed-ikbel Boulabiar  <boulabiar@gmail.com>
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <X11/X.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>

#include <grail-types.h>

#include "gesture.h"

#define MIN(l,o) ((l) < (o) ? (l) : (o))

int sendK(KeySym ks)
{
	Display* disp = XOpenDisplay(NULL);
   XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks), True, CurrentTime);
   XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks), False, CurrentTime);  
	XCloseDisplay(disp);
   return 0;
}

int sendCK(KeySym ks)
{
	Display* disp = XOpenDisplay(NULL);
   XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, XK_Control_L), True, CurrentTime);
   XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks), True, CurrentTime);
   XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks), False, CurrentTime);   
   XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, XK_Control_L), False, CurrentTime); 
	XCloseDisplay(disp);
   return 0;
}

static void usage(char *path) {
	fprintf(stderr, "Usage: %s <window> <device ID> <mask>\n", path);
}

static int parse_opts(int argc, char *argv[], xcb_window_t *window,
		      uint16_t *device_id, uint16_t *mask_len,
		      uint32_t **mask) {
	unsigned long long int tmp;
	char *end;
        int i;

	if (argc != 4) {
		usage(argv[0]);
		return -1;
	}

	tmp = strtoul(argv[1], &end, 0);
	if (*end != '\0' || tmp > UINT_MAX) {
		usage(argv[0]);
		return -1;
	}
	*window = (xcb_window_t)tmp;

	tmp = strtoul(argv[2], &end, 0);
	if (*end != '\0' || tmp > USHRT_MAX) {
		usage(argv[0]);
		return -1;
	}
	*device_id = (uint16_t)tmp;

	tmp = strtoull(argv[3], &end, 0);
	if (*end != '\0') {
		usage(argv[0]);
		return -1;
	}
	*mask_len = (tmp > UINT_MAX) ? 2 : 1;
	
	*mask = malloc(sizeof(uint32_t) * *mask_len);
	if (!*mask) {
		perror("Error: Failed to allocate event mask");
		return -1;
	}

	for (i = 0; i < *mask_len; i++)
		(*mask)[i] = tmp >> (32 * i);

	return 0;
}

static int set_mask_on_window(xcb_connection_t *connection, xcb_window_t window,
			      uint16_t device_id, uint16_t mask_len,
			      uint32_t *mask) {
	xcb_generic_error_t *error;
	xcb_void_cookie_t select_cookie;
	xcb_gesture_get_selected_events_cookie_t events_cookie;
	xcb_gesture_get_selected_events_reply_t *events_reply;
	xcb_gesture_event_mask_iterator_t events_iterator;
	int masks_len_reply;
	int mask_len_reply;
	uint32_t *mask_reply;

	select_cookie = xcb_gesture_select_events_checked(connection, window,
							  device_id, mask_len,
							  mask);
	error = xcb_request_check(connection, select_cookie);
	if (error) {
		fprintf(stderr,
			"Error: Failed to select events on window 0x%x\n",
			window);
		return -1;
	}

	events_cookie = xcb_gesture_get_selected_events(connection, window);
	events_reply = xcb_gesture_get_selected_events_reply(connection,
							     events_cookie,
							     &error);
	if (!events_reply) {
		fprintf(stderr,
			"Error: Failed to receive selected events reply on "
			"window 0x%x\n", window);
		return -1;
	}

	masks_len_reply =
		xcb_gesture_get_selected_events_masks_length(events_reply);
	if (masks_len_reply != 1) {
		fprintf(stderr,
			"Error: Wrong selected masks length returned: %d\n",
			masks_len_reply);
		return -1;
	}

	events_iterator =
		xcb_gesture_get_selected_events_masks_iterator(events_reply);
	if (events_iterator.data->device_id != device_id) {
		fprintf(stderr,
			"Error: Incorrect device id returned by server: %d\n",
			events_iterator.data->device_id);
		return -1;
	}

	mask_len_reply =
		xcb_gesture_event_mask_mask_data_length(events_iterator.data);
	if (mask_len_reply != mask_len) {
		fprintf(stderr,
			"Error: Incorrect mask length returned by server: %d\n",
			mask_len_reply);
		return -1;
	}

	mask_reply = xcb_gesture_event_mask_mask_data(events_iterator.data);
	if (memcmp(mask, mask_reply, mask_len * 4) != 0) {
		fprintf(stderr, "Error: Incorrect mask returned by server\n");
		return -1;
	}

	free(events_reply);

	return 0;
}

static int set_mask_on_subwindows(xcb_connection_t *connection,
				  xcb_window_t window, uint16_t device_id,
				  uint16_t mask_len, uint32_t *mask) {
	xcb_generic_error_t *error;
	xcb_query_tree_cookie_t tree_cookie;
	xcb_query_tree_reply_t *tree_reply;
	xcb_window_t *children;
	int num_children;
	int i;

	if (set_mask_on_window(connection, window, device_id, mask_len, mask))
		return -1;

	tree_cookie = xcb_query_tree(connection, window);
	tree_reply = xcb_query_tree_reply(connection, tree_cookie, &error);
	if (!tree_reply) {
		fprintf(stderr, "Error: Failed to query tree for window 0x%x\n",
		window);
		return -1;
	}

	num_children = xcb_query_tree_children_length(tree_reply);
	if (num_children <= 0)
		return 0;

	children = xcb_query_tree_children(tree_reply);
	if (!children) {
		fprintf(stderr,
			"Error: Failed to retrieve children of window 0x%x\n",
			window);
		return -1;
	}

	for (i = 0; i < num_children; i++)
		if (set_mask_on_subwindows(connection, children[i], device_id,
					   mask_len, mask))
			return -1;
	
	free(tree_reply);

	return 0;
}

static int set_mask(xcb_connection_t *connection, xcb_window_t window,
		    uint16_t device_id, uint16_t mask_len, uint32_t *mask) {
	xcb_generic_error_t *error;
	xcb_gesture_query_version_cookie_t version_cookie;
	xcb_gesture_query_version_reply_t *version_reply;

	version_cookie = xcb_gesture_query_version(connection, 0, 5);
	version_reply = xcb_gesture_query_version_reply(connection,
							version_cookie, &error);
	if (!version_reply) {
		fprintf(stderr,
			"Error: Failed to receive gesture version reply\n");
		return -1;
	}

	if (version_reply->major_version != 0 &&
	    version_reply->minor_version != 5) {
		fprintf(stderr,
			"Error: Server supports unrecognized version: %d.%d\n",
			version_reply->major_version,
			version_reply->minor_version);
		return -1;
	}

	free(version_reply);

	if (window != 0) {
		if (set_mask_on_window(connection, window, device_id, mask_len,
				       mask))
			return -1;
	}
	else {
		const xcb_setup_t *setup;
		xcb_screen_iterator_t screen;

		setup = xcb_get_setup(connection);
		if (!setup) {
			fprintf(stderr, "Error: Failed to get xcb setup\n");
			return -1;
		}

		for (screen = xcb_setup_roots_iterator(setup); screen.rem;
		     xcb_screen_next(&screen))
			if (set_mask_on_subwindows(connection,
			    screen.data->root, device_id, mask_len, mask))
				return -1;
	}

	return 0;
}

static void window_to_name(xcb_connection_t *connection, xcb_window_t window,
			   char *name, int name_size) {
	xcb_generic_error_t *error;
	xcb_get_property_cookie_t property_cookie;
	xcb_get_text_property_reply_t property_reply;
	int got_reply;
	static xcb_atom_t utf8_string_atom = XCB_ATOM_NONE;
	
	property_cookie = xcb_get_wm_name(connection, window);
	got_reply = xcb_get_wm_name_reply(connection, property_cookie,
					  &property_reply, &error);
	if (!got_reply || property_reply.name_len == 0) {
		snprintf(name, name_size, "(has no name)");
		if (got_reply)
			goto out;
		return;
	}

	if (property_reply.encoding == XCB_ATOM_STRING) {
		snprintf(name, name_size, "\"%.*s\"", property_reply.name_len,
			 property_reply.name);
		goto out;
	}

	if (utf8_string_atom == XCB_ATOM_NONE) {
		xcb_intern_atom_cookie_t atom_cookie;
		xcb_intern_atom_reply_t *atom_reply;
		atom_cookie = xcb_intern_atom(connection, 1,
					      strlen("UTF8_STRING"),
					      "UTF8_STRING");
		atom_reply = xcb_intern_atom_reply(connection, atom_cookie,
						   &error);
		if (!atom_reply) {
			fprintf(stderr, "Warning: Failed to receive UTF8_STRING "
				"atom\n");
			snprintf(name, name_size, "(undecipherable name)");
			goto out;
		}

		utf8_string_atom = atom_reply->atom;
		if (property_reply.encoding == utf8_string_atom) {
			snprintf(name, name_size, "\"%.*s\"", property_reply.name_len,
				 property_reply.name);
			free(atom_reply);
			goto out;
		}

		snprintf(name, name_size, "(undecipherable name)");
		free(atom_reply);
	}

out:
	xcb_get_text_property_reply_wipe(&property_reply);
}

static void print_events(xcb_connection_t *connection) {
	const xcb_query_extension_reply_t *extension_info;
	char window_name[80];


	   //Display* disp = XOpenDisplay(NULL);

	extension_info = xcb_get_extension_data(connection, &xcb_gesture_id);

	for (;;) {
		xcb_generic_event_t *event;
		xcb_gesture_notify_event_t *gesture_event;
		float *properties;
		int i;

		event = xcb_wait_for_event(connection);
		if (!event) {
			fprintf(stderr,
				"Warning: I/O error while waiting for event\n");
			return;
		}

		if (event->response_type != GenericEvent) {
			fprintf(stderr,
				"Warning: Received non-generic event type: "
				"%d\n", event->response_type);
			continue;
		}

		gesture_event = (xcb_gesture_notify_event_t *)event;

		if (gesture_event->extension != extension_info->major_opcode) {
			fprintf(stderr,
				"Warning: Received non-gesture extension "
				"event: %d\n", gesture_event->extension);
			continue;
		}

                if (gesture_event->event_type != XCB_GESTURE_NOTIFY) {
			fprintf(stderr,
				"Warning: Received unrecognized gesture event "
				"type: %d\n", gesture_event->event_type);
			continue;
		}

		printf("Gesture ID:\t\t%hu\n", gesture_event->gesture_id);

		printf("\tGesture Type:\t%d: ", gesture_event->gesture_type);
		switch (gesture_event->gesture_type) {
			case GRAIL_TYPE_DRAG1:
				printf("Drag - One Finger\n");
				break;
			case GRAIL_TYPE_PINCH1:
				printf("Pinch - One Finger\n");
				break;
			case GRAIL_TYPE_ROTATE1:
				printf("Rotate - One Finger\n");
				break;
			case GRAIL_TYPE_DRAG2:
				printf("Drag - Two Fingers\n");
				break;
			case GRAIL_TYPE_PINCH2:
				printf("Pinch - Two Fingers\n");
				break;
			case GRAIL_TYPE_ROTATE2:
				printf("Rotate - Two Fingers\n");
				break;
			case GRAIL_TYPE_DRAG3:
				printf("Drag - Three Fingers\n");
				break;
			case GRAIL_TYPE_PINCH3:
				printf("Pinch - Three Fingers\n");
				break;
			case GRAIL_TYPE_ROTATE3:
				printf("Rotate - Three Fingers\n");
				break;
			case GRAIL_TYPE_DRAG4:
				printf("Drag - Four Fingers\n");
				break;
			case GRAIL_TYPE_PINCH4:
				printf("Pinch - Four Fingers\n");
				break;
			case GRAIL_TYPE_ROTATE4:
				printf("Rotate - Four Fingers\n");
				break;
			case GRAIL_TYPE_DRAG5:
				printf("Drag - Five Fingers\n");
				break;
			case GRAIL_TYPE_PINCH5:
				printf("Pinch - Five Fingers\n");
				break;
			case GRAIL_TYPE_ROTATE5:
				printf("Rotate - Five Fingers\n");
				break;
			case GRAIL_TYPE_TAP1:
				printf("Tap - One Finger\n");
				break;
			case GRAIL_TYPE_TAP2:
				printf("Tap - Two Fingers\n");
				break;
			case GRAIL_TYPE_TAP3:
				printf("Tap - Three Fingers\n");
				break;
			case GRAIL_TYPE_TAP4:
				printf("Tap - Four Fingers\n");
				break;
			case GRAIL_TYPE_TAP5:
				printf("Tap - Five Fingers\n");
				break;
			case GRAIL_TYPE_EDRAG:
				printf("Environment Drag\n");
				break;
			case GRAIL_TYPE_EPINCH:
				printf("Environment Pinch\n");
				break;
			case GRAIL_TYPE_EROTATE:
				printf("Environment Rotate\n");
				break;
			case GRAIL_TYPE_MDRAG:
				printf("Meta Drag\n");
				break;
			case GRAIL_TYPE_MPINCH:
				printf("Meta Pinch\n");
				break;
			case GRAIL_TYPE_MROTATE:
				printf("Meta Rotate\n");
				break;
			default:
				printf("Unknown\n");
				break;
		}

		printf("\tDevice ID:\t%hu\n", gesture_event->device_id);
		printf("\tTimestamp:\t%u\n", gesture_event->time);

		printf("\tRoot Window:\t0x%x: (root window)\n",
		       gesture_event->root);

		printf("\tEvent Window:\t0x%x: ", gesture_event->event);
		if (gesture_event->event == gesture_event->root)
			printf("(root window)\n");
		else {
			window_to_name(connection, gesture_event->event,
				       window_name, sizeof(window_name));
			printf("%s\n", window_name);
		}

		printf("\tChild Window:\t0x%x: ", gesture_event->child);
		if (gesture_event->child == gesture_event->root)
			printf("(root window)\n");
		else {
			window_to_name(connection, gesture_event->child,
				       window_name, sizeof(window_name));
			printf("%s\n", window_name);
		}

		printf("\tFocus X:\t%f\n", gesture_event->focus_x);
		printf("\tFocus Y:\t%f\n", gesture_event->focus_y);
		printf("\tStatus:\t\t%hu\n", gesture_event->status);
		printf("\tNum Props:\t%hu\n", gesture_event->num_props);

		properties = (float *)(gesture_event + 1);

		for (i = 0; i < gesture_event->num_props; i++) {
			printf("\t\tProperty %u:\t%f\n", i, properties[i]);
		}
	
		if ((gesture_event->gesture_type)==4 &&	(abs(properties[0])>20) )
                	sendK((properties[0]>0)?(XK_KP_Add):(XK_KP_Subtract));

		if ((gesture_event->gesture_type)==3 &&	(abs(properties[0])>5) )
                	sendCK((properties[0]>0)?(XK_Left):(XK_Right));
		
		if ((gesture_event->gesture_type)==3 &&	(abs(properties[1])>5) )
                	sendCK((properties[1]>0)?(XK_Up):(XK_Down));
		printf("\n");
	}
}

int main(int argc, char *argv[]) {
	xcb_window_t window;
	uint16_t device_id;
	uint16_t mask_len;
	uint32_t *mask;
	xcb_connection_t *connection;

	if (parse_opts(argc, argv, &window, &device_id, &mask_len, &mask)) {
		return -1;
	}

	connection = xcb_connect(NULL, NULL);
	if (!connection) {
		free(mask);
		fprintf(stderr, "Error: Failed to connect to server\n");
		return -1;
	}

	if (set_mask(connection, window, device_id, mask_len, mask)) {
		free(mask);
		return -1;
	}

	free(mask);
	print_events(connection);
	xcb_disconnect(connection);
	return 0;
}
