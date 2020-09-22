#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <core/list.h>
#include <core/types.h>

typedef struct object
{
    struct list_head entry;
    const char *name;
} object_t;

error_t     object_attach(struct object *o, const char *name);
void 		object_detach(struct object *o);
object_t 	*object_find(const char *name);
list_t      *object_lists(void);

#endif
