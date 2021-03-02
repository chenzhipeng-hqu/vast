#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <core/list.h>
#include <core/types.h>

typedef struct object
{
    struct list_head entry;
    const char *name;
} object_t;

int16_t     object_attach(list_t *list, struct object *o, const char *name);
void 	    object_detach(list_t *list, struct object *o);
object_t 	*object_find(list_t *list, const char *name);

#endif
