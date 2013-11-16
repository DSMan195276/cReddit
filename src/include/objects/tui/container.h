#ifndef _SRC_INCLUDE_CONTAINER_H_
#define _SRC_INCLUDE_CONTAINER_H_

#include "global.h"
#include "objects/tui/screen.h"

typedef struct Container      Container;
typedef struct ContainerClass ContainerClass;

typedef enum   ContainerType  ContainerType;
typedef struct ScreenInfo     ScreenInfo;
typedef enum   ScreenSizeType ScreenSizeType;
typedef struct Con            Con;

enum ContainerType {
    CONTAINER_H
};

enum ScreenSizeType {
    CONTAINER_SIZE_FIXED,
    CONTAINER_SIZE_PERCENT,
    CONTAINER_SIZE_EXPAND
};

struct ScreenInfo {
    ScreenSizeType type;
    unsigned int allowExpand :1; /* FIXED will never be allowed to expand to
                                  * fill empty space unless this is set */
    unsigned int deleteWithContainer :1;

    union {
        int size;
        int percent;
    };

    int dispSize; /* Note: Used internally, will be recalculated */
};

struct Con {
    Screen *scr;
    ScreenInfo info;
};

struct Container {
    Screen _parent;

    ContainerType type;

    int conCount, conAlloc;
    Con *cons; 
};

struct ContainerClass {
    ScreenClass _parent;

    /* The ScreenInfo pointed to by info is copied into the 'Con' entry for
     * this screen */
    void (*registerScreen) (Container *this, Screen *screen, const ScreenInfo *info);
    void (*removeScreen)   (Container *this, Screen *screen);

    int  (*getScreenIndex) (Container *this, Screen *screen);
    void (*setScreenIndex) (Container *this, Screen *screen, int index);

    /* The returned pointer points directly to the ScreenInfo inside this
     * screens 'Con' entry. modifying the data pointed to by that pointer will
     * modify the settings of this screen directly. */
    ScreenInfo *(*getScreenInfo) (Container *this, Screen *screen);
    /* Like 'registerScreen', the ScreenInfo at 'info' is copied. */
    void        (*setScreenInfo) (Container *this, Screen *screen, const ScreenInfo *info);
};

int containerGetType();

#define CONTAINER_CAST(object) (CAST(object, Container, containerGetType()))
#define IS_CONTAINER(object) (isInstanceOf(object, containerGetType()))
#define CONTAINER_CLASS_CAST(class) (CLASS_CAST(class, ContainerClass, containerGetType()))
#define IS_CONTAINER_CLASS(class) (isInstanceOfClass(class, containerGetType()))

#define CONTAINER_CALL(object) (CONTAINER_CLASS_CAST(GET_CLASS(object)))

#endif
