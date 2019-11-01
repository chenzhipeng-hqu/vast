#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <core/list.h>
#include <core/types.h>

typedef struct object
{
    const char *name;
    struct list_head entry;
} object_t;

error_t 		object_attach(struct object *o, const char *name);
void 		object_detach(struct object *o);
object_t 	*object_find(const char *name);

#endif
