
#include "global.h"
#include "signal.h"
#include "objects/tui/screen.h"

static void screenCtor(Object *obj)
{

}

static void screenDtor(Object *obj)
{
    Screen *scr = SCREEN_CAST(obj);
    signalClear(&(scr->keypress));
    signalClear(&(scr->focusEnter));
    signalClear(&(scr->focusLeave));
}

static void screenCtorClass (ObjectClass *class)
{

}

int screenGetType()
{
    static int typeNum = -1;
    if (typeNum == -1) {
        struct ObjectSpec o = {
            .size = sizeof(Screen),
            .classSize = sizeof(ScreenClass),
            .parent = objectGetType(),
            .ctor = screenCtor,
            .dtor = screenDtor,
            .ctorClass = screenCtorClass
        };
        typeNum = registerSpec(o);
    }
    return typeNum;
}

