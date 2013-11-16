#if 0
#include "global.h"
#include "objects/tui/screen_buf.h"

static void screenBufCtor(Object *obj)
{

}

static void screenBufDtor(Object *obj)
{

}

static void screenBufCtorClass (ObjectClass *obj)
{

}

int screenBufGetType()
{
    static int typeNum = -1;
    if (typeNum == -1) {
        struct objectSpec o = {
            .size = sizeof(ScreenBuf),
            .classSize = sizeof(ScreenBufClass),
            .parent = screenGetType(),
            .ctor = screenBufCtor,
            .dtor = screenBufDtor,
            .ctorClass = screenBufCtorClass
        };
        typeNum = registerSpec(o);
    }
    return typeNum;
}
#endif
