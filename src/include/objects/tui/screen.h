#ifndef _SRC_INCLUDE_SCREEN_H_
#define _SRC_INCLUDE_SCREEN_H_

#include "global.h"
#include <ncurses.h>

#include "signal.h"
#include "objects/object.h"

typedef struct Screen Screen;
typedef struct ScreenClass ScreenClass;

struct Screen {
    Object _parent;
    WINDOW *win;

    unsigned int isResizeable :1;

    Signal keypress;
    Signal focusEnter;
    Signal focusLeave;
};

struct ScreenClass {
    ObjectClass _parent;

    void (*render) (Screen *this);
    void (*resize) (Screen *this, int width, int height);
};

int screenGetType();

void screenRender (Screen *this);
void screenResize (Screen *this, int width, int height);

#define SCREEN_CAST(object) (CAST(object, Screen, screenGetType()))
#define IS_SCREEN(object) (isInstanceOf(object, screenGetType()))
#define SCREEN_CLASS_CAST(class) (CLASS_CAST(class, ScreenClass, screenGetType()))
#define IS_SCREEN_CLASS(class) (isInstanceOfClass(class, screenGetType()))

#define SCREEN_CALL(object) (SCREEN_CLASS_CAST(GET_CLASS(object)))

#endif
