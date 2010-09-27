/*
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

#include "config.h"
#include <libxml/parser.h>


int ginn_config_open(struct ginn_config *cfg, const char *path)
{
	memset(cfg, 0, sizeof(*cfg));
	cfg->doc = xmlReadFile(path, NULL, 0);
	if (cfg->doc == NULL) {
		fprintf(stderr, "Failed to parse %s\n", path);
		return -1;
	}
	return 0;
}

void ginn_config_close(struct ginn_config *cfg)
{
	xmlFreeDoc(cfg->doc);
}


static void print_node(const xmlNode *root, int depth)
{
	xmlNode *node;
	for (node = root; node; node = node->next) {
		int i;
		if (node->type != XML_ELEMENT_NODE)
			continue;
		for (i = 0; i < depth; i++)
			printf(" ");
		printf("%s\n", node->name);
		print_node(node->children, depth + 1);
	}
}

void ginn_config_print(const struct ginn_config *cfg)
{
	const xmlNode *root = xmlDocGetRootElement(cfg->doc);
	print_node(root, 0);
}

