#ifndef _SRC_INCLUDE_LINK_SCREEN_H_
#define _SRC_INCLUDE_LINK_SCREEN_H_

#include "global.h"

#include <wchar.h>
#include "reddit.h"

typedef struct {
    RedditLinkList *list;
    int displayed;
    int offset;
    int selected;
    int screenLineCount;
    int allocLineCount;
    wchar_t **screenLines;
    int linkOpenSize;
    unsigned int linkOpen : 1;
    unsigned int helpOpen : 1;
    int helpLineCount;
    wchar_t **helpText;
} LinkScreen;

extern LinkScreen *linkScreenNew        ();
extern void        linkScreenFree       (LinkScreen *screen);
extern void        drawScreen           (LinkScreen *screen);
extern void        linkScreenDown       (LinkScreen *screen);
extern void        linkScreenUp         (LinkScreen *screen);
extern void        linkScreenOpenLink   (LinkScreen *screen);
extern void        linkScreenCloseLink  (LinkScreen *screen);
extern void        linkScreenToggleLink (LinkScreen *screen);
extern void        linkScreenOpenHelp   (LinkScreen *screen);
extern void        linkScreenCloseHelp  (LinkScreen *screen);
extern void        linkScreenToggleHelp (LinkScreen *screen);

#endif
