/**
 * Copyright 2011 Canonical Ltd.
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
#ifndef GINN_CONFIG_H
#define GINN_CONFIG_H

#include <libxml/tree.h>
#include <string.h>

#define GINN_START	0
#define GINN_UPDATE	1
#define GINN_FINISH	2

#define GINN_DEFAULT_ACCUMULATE		1

typedef struct ginn_config {
    xmlDocPtr doc;
    xmlNodePtr root;
} cfg;

typedef struct att {
    char *attrName;
    float val;
    float valMax;
    int accumulate;
    float accumVal;
} att;

typedef struct wish {
    att config_attr[25];
    char *key;
    int button;
    char *modifiers[4];
    struct wish *next;
    int when;
} wish;

typedef struct apps {
    char *appName;
    struct wish *wp;
    struct apps *next;
} apps;

#endif                          /* GINN_CONFIG_H */
