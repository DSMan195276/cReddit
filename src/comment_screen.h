#ifndef _SRC_COMMENT_SCREEN_H_
#define _SRC_COMMENT_SCREEN_H_

#include <wchar.h>
#include "reddit.h"

typedef struct {
    wchar_t *text;
    RedditComment *comment;
    unsigned int folded : 1;
    int foldCount;
    int indentCount;
} CommentLine;

typedef struct {
    RedditCommentList *list;
    int lineCount;
    int allocLineCount;
    CommentLine **lines;
    int displayed;
    int offset;
    int selected;
    int commentOpenSize;
    unsigned int commentOpen : 1;
} CommentScreen;

extern CommentLine *commentLineNew();
extern void CommentLineFree(CommentLine *line);
extern CommentScreen *commentScreenNew();
extern void commentScreenFree(CommentScreen *screen);

extern void commentScreenAddLine(CommentScreen *screen, CommentLine *line);

extern CommentScreen *getCommentScreenFromCommentList (RedditCommentList *list, int width);

extern void commentScreenDown (CommentScreen *screen);
extern void commentScreenUp (CommentScreen *screen);

extern void commentScreenDisplay (CommentScreen *screen);

extern void commentScreenOpenComment (CommentScreen *screen);
extern void commentScreenCloseComment (CommentScreen *screen);
extern void commentScreenToggleComment (CommentScreen *screen);

#endif
