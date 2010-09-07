/**
 * ginn.c 
 *
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
 */
#include <errno.h>
#include <geis/geis.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include "cJSON.h"


static void
print_attr(GeisGestureAttr *attr)
{
  fprintf(stdout, "\tattr %s=", attr->name);
  switch (attr->type)
  {
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
gesture_added(void            *cookie,
	      GeisGestureType  gesture_type,
	      GeisGestureId    gesture_id,
	      GeisSize         attr_count,
	      GeisGestureAttr *attrs)
{
  int i = 0;
  fprintf(stdout, "Gesture type %d added\n", gesture_type);
  for (i = 0; i < attr_count; ++i)
    print_attr(&attrs[i]);
}

static void
gesture_removed(void              *cookie,
		GeisGestureType    gesture_type,
		GeisGestureId      gesture_id,
	        GeisSize           attr_count,
	        GeisGestureAttr   *attrs)
{
  int i = 0;
  fprintf(stdout, "Gesture type %d removed\n", gesture_type);
  for (i = 0; i < attr_count; ++i)
    print_attr(&attrs[i]);
}

static void
gesture_start(void              *cookie,
              GeisGestureType    gesture_type,
              GeisGestureId      gesture_id,
	      GeisSize           attr_count,
	      GeisGestureAttr   *attrs)
{
  int i = 0;
  fprintf(stdout, "Gesture type %d started\n", gesture_type);
  for (i = 0; i < attr_count; ++i)
    print_attr(&attrs[i]);
}

static void
gesture_update(void              *cookie,
               GeisGestureType    gesture_type,
               GeisGestureId      gesture_id,
	       GeisSize           attr_count,
	       GeisGestureAttr   *attrs)
{
  int i = 0;
  fprintf(stdout, "Gesture type %d updated\n", gesture_type);
  for (i = 0; i < attr_count; ++i)
    print_attr(&attrs[i]);
}

static void
gesture_finish(void              *cookie,
               GeisGestureType    gesture_type,
               GeisGestureId      gesture_id,
	       GeisSize           attr_count,
	       GeisGestureAttr   *attrs)
{
  int i = 0;
  fprintf(stdout, "Gesture type %d finished\n", gesture_type);
  for (i = 0; i < attr_count; ++i)
    print_attr(&attrs[i]);
}


GeisGestureFuncs gesture_funcs = {
  gesture_added,
  gesture_removed,
  gesture_start,
  gesture_update,
  gesture_finish
};


int
parse_opts(int argc, char* argv[], uint32_t *window_id)
{
  if (argc < 2) {
    *window_id = 0xfe;
    return 1;
  } else if (argc > 2)
    return 0;

  *window_id = strtol(argv[1], NULL, 0);
  return 1;
}

int
parse_wishes(char *text)
{
  char *out;
  cJSON *json;
  json=cJSON_Parse(text);
  out=cJSON_Print(json);
  //cJSON_Delete(json);
  printf("%s\n",out);
  free(out);
  return 1;
}

int 
load_wishes(char *filename)
{
  FILE *f=fopen(filename,"rb");
  fseek(f,0,SEEK_END);
  long len=ftell(f);
  fseek(f,0,SEEK_SET);
  char *data=malloc(len+1);
  fread(data,1,len,f);
  fclose(f);
  parse_wishes(data);
  free(data);
  return 1;
}


int
main(int argc, char* argv[])
{
  GeisStatus status = GEIS_UNKNOWN_ERROR;
  GeisXcbWinInfo xcb_win_info = {
    .display_name  = NULL,
    .screenp       = NULL,
    .window_id     = 0x296
  };
  GeisWinInfo win_info = {
    GEIS_XCB_FULL_WINDOW,
    &xcb_win_info
  };
  GeisInstance instance;

  if (!parse_opts(argc, argv, &xcb_win_info.window_id))
  {
    fprintf(stderr, "usage: %s windowid\n", argv[0]);
    fprintf(stdout, "If no parameters are received, root window '0xfe' is taken \n");
    return -1;
  }

  if(!load_wishes("wishes.conf")) {
    fprintf(stderr, "Could not load Ginn whishes\n");
    return -1;
  }

  status = geis_init(&win_info, &instance);
  if (status != GEIS_STATUS_SUCCESS)
  {
    fprintf(stderr, "error in geis_init\n");
    return 1;
  }

  status = geis_configuration_supported(instance, GEIS_CONFIG_UNIX_FD);
  if (status != GEIS_STATUS_SUCCESS)
  {
    fprintf(stderr, "GEIS does not support Unix fd\n");
    return 1;
  }

  int fd = -1;
  status = geis_configuration_get_value(instance, GEIS_CONFIG_UNIX_FD, &fd);
  if (status != GEIS_STATUS_SUCCESS)
  {
    fprintf(stderr, "error retrieving GEIS fd\n");
    return 1;
  }

  status = geis_subscribe(instance,
                          GEIS_ALL_INPUT_DEVICES,
                          GEIS_ALL_GESTURES,
                          &gesture_funcs,
                          NULL);
  if (status != GEIS_STATUS_SUCCESS)
  {
    fprintf(stderr, "error subscribing to gestures\n");
    return 1;
  }

  while(1)
  {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    int sstat = select(fd+1, &read_fds, NULL, NULL, NULL);
    if (sstat < 0)
    {
      fprintf(stderr, "error %d in select(): %s\n", errno, strerror(errno));
      break;
    }

    if (FD_ISSET(fd, &read_fds))
    {
      geis_event_dispatch(instance);
    }
  }

  geis_finish(instance);
  return 0;
}

