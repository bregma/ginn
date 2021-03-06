/**
 * Copyright 2010 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * Authors:
 * 	Mohamed-Ikbel Boulabiar <boulabiar@gmail.com>
 * 	Henrik Rydberg <rydberg@bitmath.org>
 *	Stephen M. Webb <stephen.webb@canonical.com>
 *
 */
#include "config.h"
#include <geis/geis.h>
#include <getopt.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

att config_attr[25] = {[0 ... 24] = {.attrName = "",.val = 0,.valMax = 0}
};

wish w1 = {.config_attr = {[0 ... 24] =
                           {.attrName = "",.val = 0,.valMax = 0}
                           }
,
.key = "",
.next = NULL
};

wish *wp, *wpEnd;
apps *ap;

static int inside(float x, float a, float b)
{
    return ((x <= b) && (x >= a));
}

void initw(struct wish *wp)
{
    int i;
    wp->button = 0;
    wp->key = "";
    wp->next = NULL;
    for (i = 0; i < 4; i++)
        wp->modifiers[i] = "";
    for (i = 0; i < 25; i++) {
        wp->config_attr[i].attrName = "";
        wp->config_attr[i].val = 0;
        wp->config_attr[i].valMax = 0;
    }
}

void inita(struct apps *ap)
{
    ap->next = NULL;
    ap->wp = NULL;
    ap->appName = "";
}

static void clear_accum_attrs(att * attrs)
{
    int i = 0;
    while (strcmp(attrs[i].attrName, "") != 0) {
        if (attrs[i].accumulate)
            attrs[i].accumVal = 0;
        i++;
    }
}

static void update_wishes()
{
    char *activeApp;
    apps *tmpAp = ap;
    int diff = 0;

    activeApp = (char *) getCurrentApp();
    printf(" --ActiveApp %s\n", activeApp);
    if (activeApp)
        diff = strcmp(activeApp, ap->appName);

    while (diff && ap->next) {
        ap = ap->next;
        diff = strcmp(activeApp, ap->appName);
    }

    if (!diff)
        wpEnd->next = ap->wp;
    else
        wpEnd->next = NULL;

    ap = tmpAp;
}

static void
gesture_match(GeisGestureType gesture_type,
              GeisGestureId gesture_id,
              GeisSize attr_count, GeisGestureAttr * attrs, int state)
{
    struct wish *topw;
    topw = wp;
    update_wishes();
    while (wp && (0 != strcmp(wp->key, "") || wp->button)) {
        int valid = 1;
        if (gesture_type == wp->config_attr[0].val
            && attrs[8].integer_val == wp->config_attr[1].val) {
            int attrsI = 9, cAttrI = 2;
            do {
                if (0 ==
                    strcmp(attrs[attrsI].name,
                           wp->config_attr[cAttrI].attrName)) {
                    printf("DEBUG -- comparing %s %s : ",
                           attrs[attrsI].name,
                           wp->config_attr[cAttrI].attrName);
                    printf("%.2f %.2f %.2f \n",
                           attrs[attrsI].float_val,
                           wp->config_attr[cAttrI].val,
                           wp->config_attr[cAttrI].valMax);
                    printf("%i \n",
                           inside(attrs[attrsI].float_val,
                                  wp->config_attr[cAttrI].val,
                                  wp->config_attr[cAttrI].valMax));
                    if (wp->config_attr[cAttrI].accumulate) {
                        wp->config_attr[cAttrI].accumVal +=
                            attrs[attrsI].float_val;
                        valid = valid
                            && inside(wp->config_attr
                                      [cAttrI].accumVal,
                                      wp->config_attr
                                      [cAttrI].val,
                                      wp->config_attr[cAttrI].valMax);
                    } else
                        valid = valid
                            && inside(attrs[attrsI].float_val,
                                      wp->config_attr
                                      [cAttrI].val,
                                      wp->config_attr[cAttrI].valMax);
                    attrsI++;
                    cAttrI++;
                } else
                    attrsI++;
            } while ((0 != strcmp(wp->config_attr[cAttrI].attrName, ""))
                     && attrsI < 18);
            if (valid && wp->when == state) {
                if ((0 != wp->button)
                    && (0 != strcmp(wp->key, ""))) {
                    injMixBtnKey(XStringToKeysym(wp->key),
                                 wp->button, wp->modifiers);
                    printf("MIX -- MIX");
                } else {
                    if (0 != wp->button)
                        injButton(wp->button, wp->modifiers);
                    if (0 != strcmp(wp->key, ""))
                        injKey(XStringToKeysym(wp->key), wp->modifiers);
                }
                clear_accum_attrs(wp->config_attr);
            }
        }
        if (state == GINN_FINISH)
            clear_accum_attrs(wp->config_attr);
        wp = wp->next;
    }
    wp = topw;
}

static Window getRootWindow()
{
    Display *display = NULL;
    Window window = 0;

    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "error opening X11 display.\n");
        exit(1);
    }

    window = DefaultRootWindow(display);

    XCloseDisplay(display);

    return window;
}

static void print_attr(GeisGestureAttr * attr)
{
    fprintf(stdout, "\tattr %s=", attr->name);
    switch (attr->type) {
    case GEIS_ATTR_TYPE_BOOLEAN:
        fprintf(stdout, "%s\n", attr->boolean_val ? "true" : "false");
        break;
    case GEIS_ATTR_TYPE_FLOAT:
        fprintf(stdout, "%f\n", attr->float_val);
        break;
    case GEIS_ATTR_TYPE_INTEGER:
        fprintf(stdout, "%d\n", attr->integer_val);
        break;
    case GEIS_ATTR_TYPE_STRING:
        fprintf(stdout, "\"%s\"\n", attr->string_val);
        break;
    default:
        fprintf(stdout, "<unknown>\n");
        break;
    }
}

static void
gesture_added(void *cookie,
              GeisGestureType gesture_type,
              GeisGestureId gesture_id,
              GeisSize attr_count, GeisGestureAttr * attrs)
{
    int i = 0;
    fprintf(stdout, "Gesture type %d added\n", gesture_type);
    for (i = 0; i < attr_count; ++i)
        print_attr(&attrs[i]);
}

static void
gesture_removed(void *cookie,
                GeisGestureType gesture_type,
                GeisGestureId gesture_id,
                GeisSize attr_count, GeisGestureAttr * attrs)
{
    int i = 0;
    fprintf(stdout, "Gesture type %d removed\n", gesture_type);
    for (i = 0; i < attr_count; ++i)
        print_attr(&attrs[i]);
}

static void
gesture_start(void *cookie,
              GeisGestureType gesture_type,
              GeisGestureId gesture_id,
              GeisSize attr_count, GeisGestureAttr * attrs)
{
    int i = 0;
    fprintf(stdout, "Gesture type %d started\n", gesture_type);
    for (i = 0; i < attr_count; ++i)
        print_attr(&attrs[i]);

    // In GEIS v1, we know that the focus coords are in attrs 5 and 6
    movePointer((int)attrs[5].float_val, (int)attrs[6].float_val);
}

static void
gesture_update(void *cookie,
               GeisGestureType gesture_type,
               GeisGestureId gesture_id,
               GeisSize attr_count, GeisGestureAttr * attrs)
{
    int i = 0;
    fprintf(stdout, "Gesture type %d updated\n", gesture_type);
    for (i = 0; i < attr_count; ++i)
        print_attr(&attrs[i]);
    gesture_match(gesture_type, gesture_id, attr_count, attrs,
                  GINN_UPDATE);
}

static void
gesture_finish(void *cookie,
               GeisGestureType gesture_type,
               GeisGestureId gesture_id,
               GeisSize attr_count, GeisGestureAttr * attrs)
{
    int i = 0;
    fprintf(stdout, "Gesture type %d finished\n", gesture_type);
    for (i = 0; i < attr_count; ++i);   //print_attr(&attrs[i]);
    gesture_match(gesture_type, gesture_id, attr_count, attrs,
                  GINN_FINISH);
}

GeisGestureFuncs gesture_funcs = {
    gesture_added,
    gesture_removed,
    gesture_start,
    gesture_update,
    gesture_finish
};

/*
 * Searches for a default config file.
 *
 * Returns a pointer to a config file name (which must be freed) or NULL
 * if no default config file was found.
 */
static char *ginn_default_config()
{
    static const char default_file_name[] = "/wishes.xml";
    static const char *search_paths[] = {
        "etc",
        "../etc",
        ".",
        "$HOME/.ginn",
        GINN_CONFIG_DIR
    };
    static const int num_paths =
        sizeof(search_paths) / sizeof(const char *);
    int i;

    for (i = 0; i < num_paths; ++i) {
        struct stat sbuf;
        char *file_name = NULL;

        if (strstr(search_paths[i], "$HOME")) {
            char *home_dir = getenv("HOME");
            if (!home_dir) {
                continue;
            } else {
                char *cdr = index(search_paths[i], '/');
                size_t file_name_length = strlen(home_dir)
                    + strlen(cdr)
                    + strlen(default_file_name)
                    + 1;
                file_name = calloc(file_name_length, sizeof(char));
                strcpy(file_name, home_dir);
                strcat(file_name, cdr);
            }
        } else {
            size_t file_name_length = strlen(search_paths[i])
                + strlen(default_file_name)
                + 1;
            file_name = calloc(file_name_length, sizeof(char));
            strcpy(file_name, search_paths[i]);
        }
        strcat(file_name, default_file_name);
        int sres = stat(file_name, &sbuf);
        if (sres == 0) {
            fprintf(stdout, "Using wishes file %s\n", file_name);
            return file_name;
        }
        free(file_name);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    char* wishes_schema_file_name = NULL;
    while (1) {
      int option_index = 0;
      static struct option long_options[] = {
        { "validate", required_argument, 0, 'r' },
        { 0,                          0, 0,  0  }
      };

      int c = getopt_long(argc, argv, "r:", long_options, &option_index);
      if (c == -1)
        break;

      switch (c) {
        case 'r':
          wishes_schema_file_name = strdup(optarg);
          break;
        default:
          break;
      }
    }

    GeisStatus status = GEIS_UNKNOWN_ERROR;
    GeisXcbWinInfo xcb_win_info = {
        .display_name = NULL,
        .screenp = NULL,
        .window_id = getRootWindow()
    };
    GeisWinInfo win_info = {
        GEIS_XCB_FULL_WINDOW,
        &xcb_win_info
    };
    GeisInstance instance;
    struct ginn_config cfg;

    {
        char *config_file_name = NULL;
        if (argc - optind < 1) {
            fprintf(stderr, "usage: %s <configxml>\n", argv[0]);
            config_file_name = ginn_default_config();
            if (config_file_name) {
                fprintf(stderr,
                        "using default configuration file %s ... \n",
                        config_file_name);
            }
        } else {
            config_file_name = strdup(argv[optind]);
        }
        if (NULL == config_file_name) {
            fprintf(stderr, "Could not find Ginn wishes\n");
            return -1;
        }

        if (ginn_config_open(&cfg, config_file_name, wishes_schema_file_name)) {
            fprintf(stderr, "Could not load Ginn wishes\n");
            return -1;
        }
        free(config_file_name);
    }

    ginn_config_print(&cfg);
    ap = (struct apps *) malloc(sizeof(struct apps));
    inita(ap);
    wp = (struct wish *) malloc(sizeof(struct wish));
    initw(wp);
    ginn_config_store(&cfg, wp, ap);
    wpEnd = wp;
    while (wpEnd->next)
        wpEnd = wpEnd->next;

    int pos = 0;
    printf("\n");
    while (strcmp(config_attr[pos].attrName, "")) {
        printf("DEBUG %d %s %.2f %.2f \n", pos,
               config_attr[pos].attrName, config_attr[pos].val,
               config_attr[pos].valMax);
        pos++;
    }
    
    /**
     * Check the loaded wishes to see which gestures are used
     * and add these gestures to a NULL terminated list "sub_gestures_list"
     * 
     * This list is used to subscribe only the necessary gestures
     * to GEIS. This will avoid issues with 2 finger scrolls and other gestues
     * being dropped when not accounted for in the wishes file.
    **/
    
    apps * appPtr = ap;
    wish * wishPtr;
    char * sub_gestures_list[17] = {NULL};
    int finished = 0;
    
    /**
     * Loop through each wish inside ap
     * After all of the wishes in ap are accounted for
     * add global wishes
    **/ 
    
    while(finished != 1)
    {
        // Check if we are finished with the application wishes
        if(appPtr == NULL)
        {
            wishPtr = wp;
            finished = 1;
        }else{
            wishPtr = appPtr->wp;
        }
        
        // Loop through each wish
        while(wishPtr)
        {
            int i;
            for(i = 0; i < 25; i ++)
            {
                
                char *sub_gesture = malloc(sizeof(char[16]));
                int pos = 0;
                
                // Check for Drag gesture
                if(strcmp(wishPtr->config_attr[i].attrName, "delta x") == 0 ||
                    strcmp(wishPtr->config_attr[i].attrName, "delta y") == 0 ||
                    strcmp(wishPtr->config_attr[i].attrName, "velocity x") == 0 ||
                    strcmp(wishPtr->config_attr[i].attrName, "velocity y") == 0
                    )            
                {
                    sprintf(sub_gesture, "Drag,touch=%i", (int)wishPtr->config_attr[1].val);
                }
                
                // Check for Rotate gesture
                else if(strcmp(wishPtr->config_attr[i].attrName, "angle delta") == 0 ||
                    strcmp(wishPtr->config_attr[i].attrName, "angular velocity") == 0 ||
                    strcmp(wishPtr->config_attr[i].attrName, "angle") == 0
                    )            
                {
                    sprintf(sub_gesture, "Rotate,touch=%i", (int)wishPtr->config_attr[1].val);
                }
                
                // Check for Pinch gesture
                else if(strcmp(wishPtr->config_attr[i].attrName, "radius delta") == 0 ||
                    strcmp(wishPtr->config_attr[i].attrName, "radial velocity") == 0 ||
                    strcmp(wishPtr->config_attr[i].attrName, "radius") == 0
                    )            
                {
                    sprintf(sub_gesture, "Pinch,touch=%i", (int)wishPtr->config_attr[1].val);
                }
                
                // Check for Tap gesture
                else if(strcmp(wishPtr->config_attr[i].attrName, "tap time") == 0)            
                {
                    sprintf(sub_gesture, "Tap,touch=%i", (int)wishPtr->config_attr[1].val);
                }
                
                // Deallocate sub_gesture if no gesture was found
                else
                {
                    free(sub_gesture);
                    sub_gesture = NULL;
                    continue;
                }
                
                /** Check the current gesture list
                 * Only add if the gesture is not already in the list
                 * Store in index of the first NULL found
                **/
                
                while(pos < 17)
                {
                    if(sub_gestures_list[pos] == NULL)
                    {
                        sub_gestures_list[pos] = sub_gesture;
                        break;
                    }else if(strcmp(sub_gestures_list[pos], sub_gesture) == 0)
                    {
                        break;
                    }
                    pos ++;
                }
                
                
            }
            // Iterate to the next wish
            wishPtr = wishPtr->next;
        }
        
        // If there are more applications, iterate to the next
        
        if(finished != 1)
        {
            appPtr = appPtr->next;
        }
    }
    
    // Show which gestures were subscribed
    printf("Gestures subscribed:\n");
    
    pos = 0;
    while(pos < 17)
    {
        if(sub_gestures_list[pos] != NULL)
        {
            printf("%s\n", sub_gestures_list[pos]);
        }
            
        pos ++;
    }
    
    // End sub gesture list creation 

    status = geis_init(&win_info, &instance);
    if (status != GEIS_STATUS_SUCCESS) {
        fprintf(stderr, "error in geis_init\n");
        return 1;
    }

    status = geis_configuration_supported(instance, GEIS_CONFIG_UNIX_FD);
    if (status != GEIS_STATUS_SUCCESS) {
        fprintf(stderr, "GEIS does not support Unix fd\n");
        return 1;
    }

    int fd = -1;
    status =
        geis_configuration_get_value(instance, GEIS_CONFIG_UNIX_FD, &fd);
    if (status != GEIS_STATUS_SUCCESS) {
        fprintf(stderr, "error retrieving GEIS fd\n");
        return 1;
    }

    status = geis_subscribe(instance, GEIS_ALL_INPUT_DEVICES, sub_gestures_list,        //  GEIS_ALL_GESTURES,
                            &gesture_funcs, NULL);
    if (status != GEIS_STATUS_SUCCESS) {
        fprintf(stderr, "error subscribing to gestures\n");
        return 1;
    }

    openDisplay();

    while (1) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
        int sstat = select(fd + 1, &read_fds, NULL, NULL, NULL);
        if (sstat < 0) {
            fprintf(stderr, "error %d in select(): %s\n", errno,
                    strerror(errno));
            break;
        }

        if (FD_ISSET(fd, &read_fds)) {
            geis_event_dispatch(instance);
        }
    }

    geis_finish(instance);
    closeDisplay();
    free(wp);
    return 0;
}
