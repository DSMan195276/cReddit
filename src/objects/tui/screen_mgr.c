
#include "global.h"
#include "objects/tui/screen_mgr.h"

static void screenMgrCtor(Object *obj)
{
    Screen *screen = SCREEN_CAST(obj);

    screen->isResizeable = 0;
    screen->win = stdscr;
}

static void screenMgrDtor(Object *obj)
{

}

static void screenMgrCtorClass(ObjectClass *objClass)
{

}

int screenMgrGetType()
{
    static int typeNum = -1;
    if (typeNum == -1) {
        struct ObjectSpec o = {
            .size = sizeof(ScreenMgr),
            .classSize = sizeof(ScreenMgrClass),
            .parent = containerGetType(),
            .ctor = screenMgrCtor,
            .dtor = screenMgrDtor,
            .ctorClass = screenMgrCtorClass 
        };
        typeNum = registerSpec(o);
    }
    return typeNum;
}

void screenMgrInit(ScreenMgr *screen)
{
    initalize(OBJECT_CAST(screen), screenMgrGetType());
}

ScreenMgr *screenMgrNew()
{
    return SCREEN_MGR_CAST(new(screenMgrGetType()));
}

