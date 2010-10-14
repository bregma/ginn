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


//static 
void parse_node(const xmlNode *root, int depth, struct att config_attr[])
{
	xmlNode *node;
	int position=2;
	for (node = root; node; node = node->next) {
		do {
			if (node->type != XML_ELEMENT_NODE)
				continue;
		     if (0==strcmp(node->name, "wish")) {
		  	printf(" gesture %s fingers %s ",
				(xmlGetProp(node, "gesture")),
				(xmlGetProp(node, "fingers")));
		  	config_attr[0].attrName = "gesture name";
			switch (xmlGetProp(node, "gesture")[0]) {
				case 'D': config_attr[0].val= 0; break; 
				case 'P': config_attr[0].val= 1; break;  
				case 'R': config_attr[0].val= 2; break;  
				case 'T': config_attr[0].val=15; break;  
			}
			config_attr[1].attrName = "touches";
		  	config_attr[1].val = atoi(xmlGetProp(node, "fingers"));
		     }
		     while (0==strcmp(node->name, "trigger")) {
			config_attr[position].attrName = xmlGetProp(node, "prop");
		  	config_attr[position].val = atoi(xmlGetProp(node, "min"));
			position++;
			if (node->next) node = (node->next);	
		     }	
		} while (node->children && (node = node->children));
	}
}

void ginn_config_store(const struct ginn_config *cfg, struct att config_attr[])
{
	const xmlNode *root = xmlDocGetRootElement(cfg->doc);
	parse_node(root, 0, config_attr);
}

void ginn_config_print(const struct ginn_config *cfg)
{
	const xmlNode *root = xmlDocGetRootElement(cfg->doc);
	print_node(root, 0);
}

