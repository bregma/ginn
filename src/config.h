#include <libxml/tree.h>
#include <string.h>

typedef struct ginn_config {
        xmlDocPtr doc;
	xmlNodePtr root;
} cfg;

typedef struct att {
	char * attrName ;
	int val ;
	int valMax;
} att;

