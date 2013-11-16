#ifndef _SRC_INCLUDE_OBJECTS_OBJECT_H_
#define _SRC_INCLUDE_OBJECTS_OBJECT_H_

#include "global.h"

typedef struct Object      Object;
typedef struct ObjectClass ObjectClass;

#ifdef REDDIT_DEBUG
/* Note -- Leading comma important, debugging adds an extra parameter to
 * 'objectCheckCast' and 'objectClassCheckCast' */
# define ERROR_LINE(object, type) , (L"Error casting " Q(object) " to " Q(type))
#else
# define ERROR_LINE(object, type)
#endif

/* Note, in checked versions, ERROR_LINE inserts a comma for the third
 * parameter if debugging is on */
#define CHECKED_CAST(object, type, typeNum) \
    ((type*)objectCheckCast((Object*)object, typeNum ERROR_LINE(object, type)))

#define CHECKED_CLASS_CAST(class, type, typeNum) \
    ((type*)objectClassCheckCast ((ObjectClass*)class, typeNum ERROR_LINE(class, type)))

#define UNCHECKED_CAST(object, type) \
    ((type*)object)

#define UNCHECKED_CLASS_CAST(class, type) \
    ((type*)class)

#ifdef REDDIT_DEBUG
/* This implements type checking on all normal casting if debugging in on */
# define CAST(object, type, typeNum)      CHECKED_CAST(object, type, typeNum)
# define CLASS_CAST(class, type, typeNum) CHECKED_CLASS_CAST(class, type, typeNum)
#else
# define CAST(object, type, typeNum)      UNCHECKED_CAST(object, type)
# define CLASS_CAST(class, type, typeNum) UNCHECKED_CLASS_CAST(class, type)
#endif

struct Object {
    int type;
    const ObjectClass *class;
};

typedef struct ObjectSpec  ObjectSpec;

struct ObjectClass {
    int type;
};

struct ObjectSpec {
    int type;
    size_t size;
    size_t classSize;
    int parent;

    void (*ctor) (Object *obj);
    void (*dtor) (Object *obj); 
    void (*ctorClass) (ObjectClass *class);

    ObjectClass *class;
};

extern int          registerSpec             (ObjectSpec d);
extern ObjectSpec   getTypeInfo              (int type);
extern ObjectClass *getParentClass           (int type);
extern int          isInstanceOfClass        (ObjectClass *class, int type);
extern Object      *new                      (int type);
extern void         initalize                (Object *obj, int type);

extern int  objectGetType ();
extern int  isInstanceOf  (Object *obj, int type);
extern void delete        (Object *obj);
extern void finalize      (Object *obj);

/* Debugging requires a debug message -- If debugging is off then a checked
 * cast doesn't create an error message */
#ifdef REDDIT_DEBUG
 extern void *objectCheckCast      (Object *obj,        int type, const wchar_t *errorLine);
 extern void *objectClassCheckCast (ObjectClass *class, int type, const wchar_t *errorLine);
#else
 extern void *objectCheckCast      (Object *obj,        int type);
 extern void *objectClassCheckCast (ObjectClass *class, int type);
#endif

#define OBJECT_CAST(object) (CAST(object, Object, objectGetType()))
#define IS_OBJECT(object) (isInstanceOf(object, objectGetType()))
#define OBJECT_CLASS_CAST(class) (CLASS_CAST(class, ObjectClass, objectGetType()))
#define GET_CLASS(object) (OBJECT_CAST(object)->class)

#endif
