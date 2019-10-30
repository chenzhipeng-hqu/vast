

#include <core/list.h>
#include <core/object.h>
#include <core/types.h>
#include <core/utils.h>
#include <string.h>
//#include <third/croutine/port.h>

static LIST_HEAD(object_list);

err_t object_attach(object_t *o, const char *name)
{
    OS_CPU_SR cpu_sr;
    err_t ret = 1;

	if(NULL == object_find(name))
	{
	    o->name = name;
		enter_critical();
	    list_add_tail(&o->entry, &object_list);
	    exit_critical();
	    ret = 0;
	}

	return ret;
}

void object_detach(struct object *o)
{
    OS_CPU_SR cpu_sr;

    enter_critical();
    list_del(&o->entry);
    exit_critical();
}

object_t *object_find(const char *name)
{
    struct list_head *iter;

    list_for_each(iter, &object_list)
    {
        struct object *o = container_of(iter, struct object, entry);
        if (!strcmp(o->name, name))
            return o;
    }
    return NULL;
}
