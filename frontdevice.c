#include "frontdevice.h"
#include "list.h"
#include <stdlib.h>

struct device *alloc_device( const struct device_ops *ops)
{
    struct device *dev = (struct device *)malloc(ops->size);
    if (dev)
    {
        dev->refcount = 1;
        dev->ops      = ops;
		dev->type	  = ops->type;
        list_init( &dev->channels);
		list_init( &dev->inputs);
		list_init( &dev->outputs);
        return dev;
    }
    return (struct device *)NULL;
}



















