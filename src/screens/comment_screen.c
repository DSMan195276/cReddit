
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "screens/comment_screen.h"
#include "reddit.h"

CommentLine *commentLineNew()
{
    CommentLine *line = malloc(sizeof(CommentLine));
    memset(line, 0, sizeof(CommentLine));
    return line;
}

void commentLineFree(CommentLine *line)
{
    if (line == NULL)
        return ;
    free(line->text);
    free(line);
}

CommentScreen *commentScreenNew()
{
    CommentScreen *screen = malloc(sizeof(CommentScreen));
    memset(screen, 0, sizeof(CommentScreen));
    return screen;
}

void commentScreenFreeLines (CommentScreen *screen)
{
    int i;
    if (screen == NULL)
        return ;
    for (i = 0; i < screen->lineCount; i++)
        commentLineFree(screen->lines[i]);
    free(screen->lines);
    screen->lines = NULL;
    screen->lineCount = 0;
    screen->allocLineCount = 0;
}

void commentScreenFree(CommentScreen *screen)
{
    if (screen == NULL)
        return ;
    commentScreenFreeLines(screen);

    free(screen->lines);
    free(screen);
}

void commentScreenAddLine(CommentScreen *screen, CommentLine *line)
{
    screen->lineCount++;
    if (screen->lineCount >= screen->allocLineCount) {
        screen->allocLineCount+=100;
        screen->lines = realloc(screen->lines, sizeof(CommentLine**) * screen->allocLineCount);
    }
    screen->lines[screen->lineCount - 1] = line;
}

wchar_t *createCommentLine(RedditComment *comment, int width, int indent)
{
    wchar_t *text = malloc(sizeof(wchar_t) * (width+1));
    int i, ilen = indent * 3, bodylen, texlen;

    bodylen = wcslen(comment->wbodyEsc);
    memset(text, 32, sizeof(wchar_t) * (width));
    text[width] = (wchar_t)0;

    if (comment->directChildrenCount > 0)
        swprintf(text + ilen, width + 1 - ilen, L"%s (%d hidden) > ", comment->author, comment->totalReplyCount);
    else
        swprintf(text + ilen, width + 1 - ilen, L"%s > ", comment->author);

    texlen = wcslen(text);
    for (i = 0; i <= width - texlen - 1; i++)
        if (i <= bodylen - 1 && comment->wbodyEsc[i] != L'\n')
            text[i + texlen] = comment->wbodyEsc[i];
        else
            text[i + texlen] = (wchar_t)32;

    return text;
}

int getCommentScreenRecurse(CommentScreen *screen, RedditComment *comment, int width, int indent)
{
    int i;
    RedditComment *current;
    CommentLine *line;
    int indentCur = indent + 1;
    int nested = 0;
    for (i = 0; i < comment->replyCount; i++) {
        current = comment->replies[i];
        line = commentLineNew();
        line->text = createCommentLine(current, width, indentCur);
        line->indentCount = indentCur;
        line->comment = current;
        commentScreenAddLine(screen, line);
        if (current->replyCount) {
            line->foldCount = getCommentScreenRecurse(screen, current, width, indentCur);
            nested += line->foldCount;
        }
        nested++;
    }
    return nested;
}

void commentScreenRenderLines (CommentScreen *screen)
{
    if (screen->lines)
        commentScreenFreeLines(screen);

    getCommentScreenRecurse(screen, screen->list->baseComment, screen->width, -1);
}

CommentScreen *getCommentScreenFromCommentList(RedditCommentList *list, int width)
{
    CommentScreen *screen;
    if (list == NULL)
        return NULL;

    screen = commentScreenNew();

    getCommentScreenRecurse(screen, list->baseComment, width, -1);

    return screen;
}

void commentScreenDown(CommentScreen *screen)
{
    screen->selected++;
    if (screen->selected > screen->offset + screen->displayed) {
        if (screen->offset + screen->displayed + 1 < screen->lineCount)
            screen->offset++;
        else
            screen->selected--;
    } else if (screen->selected > screen->lineCount - 1) {
        screen->selected--;
    }
}

void commentScreenUp(CommentScreen *screen)
{
    screen->selected--;
    if (screen->selected < 0)
        screen->selected++;
    if (screen->selected < screen->offset)
        screen->offset--;
}

void commentScreenDisplay(CommentScreen *screen)
{
    int i, screenLines;
    wchar_t *tmpbuf;
    int bufSize, lastLine, bufLen;

    if (screen == NULL)
        return ;

    bufSize = sizeof(wchar_t) * (COLS + 1);
    bufLen = COLS;
    tmpbuf = malloc(bufSize);
    memset(tmpbuf, 0, bufSize);

    for (i = 0; i < bufLen; i++)
        tmpbuf[i] = L' ';

    screenLines = screen->offset + screen->displayed + 1;

    for (i = 0; i < screen->displayed + screen->offset + 1 + screen->commentOpenSize; i++)
        mvaddwstr(i, 0, tmpbuf);

    attron(COLOR_PAIR(1));

    for(i = screen->offset; i < screenLines; i++) {
        if(i == screen->selected)
            attron(COLOR_PAIR(2));

        if (i < screen->lineCount && screen->lines[i]->text != NULL)
            mvaddwstr(i - screen->offset, 0, screen->lines[i]->text);
        else
            mvaddwstr(i - screen->offset, 0, tmpbuf);

        if (i == screen->selected)
            attron(COLOR_PAIR(1));
    }

    if (screen->commentOpen) {
        RedditComment *current;
        lastLine = screenLines - screen->offset;

        for (i = lastLine; i < lastLine + screen->commentOpenSize; i++)
            mvaddwstr(i, 0, tmpbuf);

        for (i = 0; i < bufLen; i++)
            tmpbuf[i] = L'-';

        tmpbuf[bufLen] = (wchar_t)0;
        attron(COLOR_PAIR(2));
        mvaddwstr(lastLine, 0, tmpbuf);

        attron(COLOR_PAIR(1));
        if (screen->lineCount >= screen->selected) {
            current = screen->lines[screen->selected]->comment;
            if (current != NULL) {
                swprintf(tmpbuf, bufLen, L"%s - %d Up / %d Down", current->author, current->ups, current->downs);
                mvaddwstr(lastLine + 1, 0, tmpbuf);
                swprintf(tmpbuf, bufLen, L"-------");
                mvaddwstr(lastLine + 2, 0, tmpbuf);

                mvaddwstr(lastLine + 3, 0, current->wbodyEsc);
            }
        }
    }

    free(tmpbuf);
    refresh();
}

void commentScreenOpenComment(CommentScreen *screen)
{
    if (!screen->commentOpen) {
        screen->commentOpen = 1;
        screen->displayed -= screen->commentOpenSize - 1;
        if (screen->selected > screen->displayed + screen->offset)
            screen->offset = screen->selected - screen->displayed;
    }
}

void commentScreenCloseComment(CommentScreen *screen)
{
    if (screen->commentOpen) {
        screen->commentOpen = 0;
        screen->displayed += screen->commentOpenSize - 1;
    }
}

void commentScreenToggleComment(CommentScreen *screen)
{
    if (screen->commentOpen)
        commentScreenCloseComment(screen);
    else
        commentScreenOpenComment(screen);
}


