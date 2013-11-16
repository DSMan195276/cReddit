
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "objects/tui/container.h"

static void registerScreen (Container *this, Screen *screen, const ScreenInfo *info)
{
    const int blockSize = 10;
    int cnum = this->conCount++;
    if (this->conCount >= this->conAlloc)
        this->cons = realloc(this->cons, (this->conAlloc += blockSize) * sizeof(Con));

    this->cons[cnum].scr = screen;
    memcpy(&(this->cons[cnum].info), info, sizeof(ScreenInfo));
}

static void removeScreen (Container *this, Screen *screen)
{
    int c;
    for (c = 0; c < this->conCount; c++) {
        if (this->cons[c].scr == screen) {
            if (this->cons[c].info.deleteWithContainer)
                delete(OBJECT_CAST(this->cons[c].scr));

            memmove(this->cons + c, this->cons + c + 1, (this->conCount - c) * sizeof(Con));
            break;
        }
    }
}

static int getScreenIndex (Container *this, Screen *screen)
{
    int c;
    for (c = 0; c < this->conCount; c++)
        if (this->cons[c].scr == screen)
            return c;

    return -1;
}

static void setScreenIndex (Container *this, Screen *screen, int index)
{
    int c;
    Con tmp;
    for (c = 0; c < this->conCount; c++) {
        if (this->cons[c].scr == screen) {
            tmp = this->cons[c];
            if (c > index)
                memmove(this->cons + c, this->cons + c + 1, (index - c) * sizeof(Con));
            else if (c < index)
                memmove(this->cons + index, this->cons + index + 1, (c - index) * sizeof(Con));

            if (c != index)
                this->cons[index] = tmp;
            return ;
        }
    }
}

static ScreenInfo *getScreenInfo (Container *this, Screen *screen)
{
    int c;
    for (c = 0; c < this->conCount; c++)
        if (this->cons[c].scr == screen)
            return &(this->cons[c].info);

    return NULL;
}

static void setScreenInfo (Container *this, Screen *screen, const ScreenInfo *info)
{
    int c;
    for (c = 0; c < this->conCount; c++) {
        if (this->cons[c].scr == screen) {
            memcpy(&(this->cons[c].info), info, sizeof(ScreenInfo));
            return ;
        }
    }
}

static void containerCalcSizes(Container *this)
{
    int rows, cols;
    int sizes[this->conCount];
}

static void containerRender(Screen *this)
{
    Container *container = CONTAINER_CAST(this);
    int i, curRow = 0;
    int rows, cols;

    /* We can't render to an empty window */
    if (this->win == NULL)
        return ;

    containerCalcSizes(container);

    getmaxyx(this->win, rows, cols);

    for (i = 0; i < container->conCount; i++) {
        copywin(container->cons[i].scr->win, this->win,
                0, 0,
                0, 0, container->cons[i].info.dispSize, cols,
                1);
        curRow += container->cons[i].info.dispSize;
    }

}

static void containerCtor(Object *obj)
{

}

static void containerDtor(Object *obj)
{
    Container *container = CONTAINER_CAST(obj);
    int c;

    for (c = 0; c < container->conCount; c++)
       if (container->cons[c].info.deleteWithContainer)
           delete(OBJECT_CAST(container->cons[c].scr));

    free(container->cons);
}

static void containerCtorClass(ObjectClass *objClass)
{
    ContainerClass *conClass = UNCHECKED_CLASS_CAST(objClass, ContainerClass);
    ScreenClass *scrClass    = SCREEN_CLASS_CAST(objClass);

    conClass->registerScreen = registerScreen;
    conClass->removeScreen   = removeScreen;

    conClass->getScreenIndex = getScreenIndex;
    conClass->getScreenInfo  = getScreenInfo;
    conClass->setScreenIndex = setScreenIndex;
    conClass->setScreenInfo  = setScreenInfo;

    scrClass->render         = containerRender;
}

int containerGetType()
{
    static int typeNum = -1;
    if (typeNum == -1) {
        struct ObjectSpec o = {
            .size = sizeof(Container),
            .classSize = sizeof(ContainerClass),
            .parent = screenGetType(),
            .ctor = containerCtor,
            .dtor = containerDtor,
            .ctorClass = containerCtorClass
        };
        typeNum = registerSpec(o);
    }
    return typeNum;
}

