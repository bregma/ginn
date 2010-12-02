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
	char *attrName ;
	float val ;
	float valMax;
	int accumulate;
	float accumVal;
} att;

typedef struct wish {
	att config_attr[25];
	char *key;
	int   button;
	char *modifiers[4];
	struct wish* next;
	int when;
} wish;

typedef struct apps {
	char *appName;
	struct wish *wp;
	struct apps *next;
} apps;
