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

static int ginn_str2state(const char *str){
	if (str == NULL) return -1;
	if (strcmp(str, "start") == 0) return GINN_START;
	if (strcmp(str, "update") == 0) return GINN_UPDATE;
	if (strcmp(str, "finish") == 0) return GINN_FINISH;
	fprintf(stderr, "ERROR: Undefined state: %s\n", str);
	return -2;
}

static int ginn_str2bool(const char *str){
	if (str == NULL) return -1;
	if (strcmp(str, "true") == 0) return 1;
	if (strcmp(str, "false") == 0) return 0;
	fprintf(stderr, "ERROR: Ivalid value for boolean attribute: %s\n", str);
	return -2;
}

void parse_node(const xmlNode *root, int depth, struct wish *wp)
{
	xmlNode *node, *tnode;
	struct wish *wpNext, *topwp;
	int position=2;
	topwp=wp;	
	for (node = root; node; node = node->next) {
		do {
			if (node->type != XML_ELEMENT_NODE)
				continue;
		     if (0==strcmp(node->name, "wish")) {
		  	printf(" gesture %s fingers %s ",
				(xmlGetProp(node, "gesture")),
				(xmlGetProp(node, "fingers")));
		  	wp->config_attr[0].attrName = "gesture name";
			switch (xmlGetProp(node, "gesture")[0]) {
				case 'D': wp->config_attr[0].val=wp->config_attr[0].valMax= 0; break; 
				case 'P': wp->config_attr[0].val=wp->config_attr[0].valMax= 1; break;  
				case 'R': wp->config_attr[0].val=wp->config_attr[0].valMax= 2; break;  
				case 'T': wp->config_attr[0].val=wp->config_attr[0].valMax=15; break;  
			}
			wp->config_attr[1].attrName = "touches";
		  	wp->config_attr[1].val      = atoi(xmlGetProp(node, "fingers"));
		  	wp->config_attr[1].valMax   = atoi(xmlGetProp(node, "fingers"));
		     }
		     if (0==strcmp(node->name, "action")) {
			     wp->when = ginn_str2state(xmlGetProp(node, "when"));
			     if (wp->when < 0)
				     fprintf(stderr, "ERROR: you must provide property 'when' to the action: %s\n", xmlGetProp(node, "name"));
		     }
		     while (0==strcmp(node->name, "trigger")) {
			wp->config_attr[position].attrName = xmlGetProp(node, "prop");
		  	wp->config_attr[position].val = atoi(xmlGetProp(node, "min"));
		  	wp->config_attr[position].valMax = atoi(xmlGetProp(node, "max"));
		  	int acc = ginn_str2bool(xmlGetProp(node, "accumulate"));
			wp->config_attr[position].accumulate = acc == -1 ? GINN_DEFAULT_ACCUMULATE : acc;
			wp->config_attr[position].accumVal = 0;
			position++;
			if (node->next) 
				node = (node->next);
		     }
		     if (0==strcmp(node->name, "key")) {
			wp->key = xmlNodeGetContent(node);
			if (xmlGetProp(node, "modifier1"))
				wp->modifiers[0] = xmlGetProp(node, "modifier1"); 
			if (xmlGetProp(node, "modifier2"))
				wp->modifiers[1] = xmlGetProp(node, "modifier2"); 
			if (xmlGetProp(node, "modifier3"))
				wp->modifiers[2] = xmlGetProp(node, "modifier3"); 
			if (xmlGetProp(node, "modifier4"))
				wp->modifiers[3] = xmlGetProp(node, "modifier4"); 
			if (node->parent->parent->next){
				node = (node->parent->parent->next);
				position=2;
				//FIXME: There is always an extra allocation (i.e: 4 allocation for 3 wishes)
				wpNext = (struct wish *) malloc(sizeof(struct wish));
				init(wpNext);
				wp = wp->next = wpNext;
			}
		     }
		} while (node->children && (node = node->children));
	}
	wp=topwp;
}


void ginn_config_store(const struct ginn_config *cfg, struct wish *w)
{
	const xmlNode *root = xmlDocGetRootElement(cfg->doc);
	parse_node(root, 0, w);
}

void ginn_config_print(const struct ginn_config *cfg)
{
	const xmlNode *root = xmlDocGetRootElement(cfg->doc);
	print_node(root, 0);
}

