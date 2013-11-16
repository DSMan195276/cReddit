
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "objects/object.h"

/* Note: 'objects' is never freed, but in general it shouldn't ever matter
 * because it's required while the object system is in use, and uses so little
 * memory there isn't any advantage in trying to free it early */
int objCount = 0;
int objAllocCount = 0;
ObjectSpec *objects = NULL;

int registerSpec(ObjectSpec d)
{
    ObjectSpec *curObj;

    if (objCount >= objAllocCount)
        objects = realloc(objects, (objAllocCount += 50) * sizeof(ObjectSpec));

    curObj = objects + objCount;
    *curObj = d;

    /* Initalize this Object type's class structure */
    curObj->class = malloc(d.classSize);
    memset(curObj->class, 0, d.classSize);
    if (d.parent >= 0)
        memcpy(curObj->class, objects[d.parent].class, objects[d.parent].classSize);

    curObj->type = objCount;
    curObj->class->type = objCount;

    (*d.ctorClass) (curObj->class);

    /* Note: PostIncrement */
    return objCount++;
}

ObjectClass *getParentClass(int type)
{
    if (objects[type].parent >= 0)
        return objects[objects[type].parent].class;
    else
        return NULL;
}

ObjectSpec getTypeInfo(int type)
{
    return objects[type];
}

int isInstanceOfClass(ObjectClass *obj, int type)
{
    int t;
    if (!obj)
        return 0;
    for (t = obj->type; t >= 0; t = objects[t].parent)
        if (t == type)
            return 1;

    return 0;
}

int isInstanceOf(Object *obj, int type)
{
    int t;
    if (!obj)
        return 0;
    for (t = obj->type; t >= 0; t = objects[t].parent)
        if (t == type)
            return 1;

    return 0;
}

void initalize(Object *obj, int typeNum)
{
    int *parents;
    int parentCount = 0, i = 0;
    ObjectSpec *spec = objects + typeNum;

    obj->type = typeNum;
    obj->class = spec->class;

    for (; spec->parent >= 0; spec = objects + spec->parent)
        parentCount++;

    parents = alloca(parentCount * sizeof(int));

    parentCount = 0;

    for (; spec->parent >= 0; spec = objects + spec->parent)
        parents[parentCount++] = spec->type;

    for (i = parentCount - 1; i >= 0 ; i--)
        (*objects[parents[i]].ctor) (obj);
}

Object *new(int typeNum)
{
    ObjectSpec *spec = objects + typeNum;
    Object *obj = NULL;

    obj = malloc(spec->size);
    memset(obj, 0, spec->size);

    initalize(obj, typeNum);

    return obj;
}

void finalize(Object *obj)
{
    ObjectSpec *spec = objects + obj->type;

    /* Call every dtor function, starting with the top-level object and then
     * work our way down the inheritance chain */
    for (spec = objects + obj->type; spec->parent >= 0; spec = objects + spec->parent)
        (*spec->dtor) (obj);

    (*spec->dtor) (obj);
}

void delete(Object *obj)
{
    finalize(obj);
    free(obj);
}


void objectCtor (Object *obj)
{
    return ;
}

void objectDtor (Object *obj)
{
    return ;
}

void objectCtorClass (ObjectClass *class)
{
    return ;
}

int objectGetType ()
{
    static int typeNum = -1;

    if (typeNum == -1) {
        struct ObjectSpec o = {
            .size = sizeof(Object),
            .classSize = sizeof(ObjectClass),
            .parent = -1,
            .ctor = objectCtor,
            .dtor = objectDtor,
            .ctorClass = objectCtorClass
        };
        typeNum = registerSpec(o);
    }

    return typeNum;
}

#ifdef REDDIT_DEBUG
 void *objectCheckCast(Object *obj, int type, const wchar_t *errorLine)
#else
 void *objectCheckCast(Object *obj, int type)
#endif
{
    if (!obj)
        return NULL;

    if (isInstanceOf(obj, type)) {
        return obj;
    } else {
        DEBUG_PRINT(L"%ls\n", errorLine);
        return NULL;
    }
}

#ifdef REDDIT_DEBUG
 void *objectClassCheckCast(ObjectClass *class, int type, const wchar_t *errorLine)
#else
 void *objectClassCheckCast(ObjectClass *class, int type)
#endif
{
    if (!class)
        return NULL;

    if (isInstanceOfClass(class, type)) {
        return class;
    } else {
        DEBUG_PRINT(L"%ls\n", errorLine);
        return NULL;
    }
}

