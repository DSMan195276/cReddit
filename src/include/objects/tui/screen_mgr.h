#ifndef _SRC_INCLUDE_SCREEN_MGR_H_
#define _SRC_INCLUDE_SCREEN_MGR_H_

#include "global.h"
#include "objects/tui/container.h"

typedef struct ScreenMgr ScreenMgr;
typedef struct ScreenMgrClass ScreenMgrClass;

struct ScreenMgr {
    Container _parent;
};

struct ScreenMgrClass {
    ContainerClass _parent;
};

int screenMgrGetType();

ScreenMgr *screenMgrNew ();
void       screenMgrInitalize(ScreenMgr *screen);

#define SCREEN_MGR_CAST(object) (CAST(object, ScreenMgr, screenMgrGetType()))
#define IS_SCREEN_MGR(object) (isInstanceOf(object, screenMgrGetType()))
#define SCREEN_MGR_CLASS_CAST(class) (CAST(class, screenMgrGetType()))
#define IS_SCREEN_MGR_CLASS(class) (isInstanceOfClass(cast, screenMgrGetType()))

#endif
