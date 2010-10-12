#include <libxml/tree.h>

typedef struct ginn_config {
        xmlDocPtr doc;
	xmlNodePtr root;
} cfg;

typedef struct att {
	char * attrName ;
	double val ;
} att;

