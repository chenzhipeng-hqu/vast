#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "types.h"
#include "list.h"

typedef struct object
{
    const char *name;
    struct list_head entry;
} object_t;

err_t 		object_attach(struct object *o, const char *name);
void 		object_detach(struct object *o);
object_t 	*object_find(const char *name);

#endif
