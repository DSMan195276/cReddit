#ifndef _SRC_LINK_SCREEN_C_
#define _SRC_LINK_SCREEN_C_

#define _XOPEN_SOURCE_EXTENDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "link_screen.h"

LinkScreen *linkScreenNew()
{
    LinkScreen *screen = malloc(sizeof(LinkScreen));
    memset(screen, 0, sizeof(LinkScreen));
    return screen;
}

void linkScreenFree(LinkScreen *screen)
{
    int i;
    if (screen == NULL)
        return ;
    for (i = 0; i < screen->allocLineCount; i++)
        free(screen->screenLines[i]);

    free(screen->screenLines);
    free(screen);
}

/*
 * This function renders a single line on a link screen into that LinkScreen's
 * line buffer. If this is called on an already rendered line, it will be
 * rendered again.
 *
 * 'line' should be zero-based.
 * 'width' is one-based, should be the width to render the line at.
 */
static void linkScreenRenderLine (LinkScreen *screen, int line, int width)
{
    size_t tmp, title;
    size_t offset;
    if (screen->allocLineCount <= line) {
        screen->allocLineCount = line + 100;
        screen->screenLines = realloc(screen->screenLines, screen->allocLineCount * sizeof(wchar_t*));
        memset(screen->screenLines + screen->allocLineCount - 100, 0, sizeof(wchar_t*) * 100);
    }

    if (screen->screenLineCount < line)
        screen->screenLineCount = line;

    screen->screenLines[line] = realloc(screen->screenLines[line], (width + 1) * sizeof(wchar_t));

    swprintf(screen->screenLines[line], width + 1, L"%d. [%4d] %20s - ", line + 1, screen->list->links[line]->score, screen->list->links[line]->author);

    offset = wcslen(screen->screenLines[line]);
    title = wcslen(screen->list->links[line]->wtitleEsc);
    for (tmp = 0; tmp <= width - offset; tmp++)
        if (tmp >= title)
            screen->screenLines[line][tmp + offset] = (wchar_t)32;
        else
            screen->screenLines[line][tmp + offset] = screen->list->links[line]->wtitleEsc[tmp];

    screen->screenLines[line][width] = (wchar_t)0;
}

static void linkScreenSetupSplit (LinkScreen *screen, wchar_t *tmpbuf, int bufLen, int lastLine)
{
    int i;

    for (i = 0; i < bufLen; i++)
        tmpbuf[i] = L'-';

    tmpbuf[bufLen] = (wchar_t)0;
    attron(COLOR_PAIR(2));
    mvaddwstr(lastLine, 0, tmpbuf);
}

static void linkScreenRenderLinkText (LinkScreen *screen, wchar_t *tmpbuf, int bufLen, int screenLines)
{
    RedditLink *current;
    int lastLine = screenLines - screen->offset;

    linkScreenSetupSplit(screen, tmpbuf, bufLen, lastLine);

    attron(COLOR_PAIR(1));
    if (screen->list->linkCount >= screen->selected) {
        current = screen->list->links[screen->selected];
        if (current != NULL) {
            swprintf(tmpbuf, bufLen, L"%s - %d Score / %d Up / %d Down / %d Comments\nTitle: ", current->author, current->score, current->ups, current->downs, current->numComments);
            mvaddwstr(lastLine + 1, 0, tmpbuf);
            addwstr(current->wtitleEsc);
            addch('\n');
            swprintf(tmpbuf, bufLen, L"-------\n");
            addwstr(tmpbuf);

            if (current->flags & REDDIT_LINK_IS_SELF)
                addwstr(current->wselftextEsc);
            else
                addstr(current->url);
        }
    }
}

static void linkScreenRenderHelpText (LinkScreen *screen, wchar_t *tmpbuf, int bufLen, int screenLines)
{
    int i;
    int lastLine = screenLines - screen->offset;

    linkScreenSetupSplit(screen, tmpbuf, bufLen, lastLine);

    attron(COLOR_PAIR(1));

    for(i = 0; i < screen->helpLineCount; i++)
        mvaddwstr(lastLine + 1 + i, 0, screen->helpText[i]);

}

/*
 * Prints a list of posts to the screen
 */
void drawScreen(LinkScreen *screen)
{
    int i, screenLines;
    wchar_t *tmpbuf;
    int bufSize, bufLen;

    if (screen == NULL)
        return ;

    bufSize = sizeof(wchar_t) * (COLS + 1);
    bufLen = COLS;
    tmpbuf = malloc(bufSize);
    memset(tmpbuf, 0, bufSize);

    for (i = 0; i < bufLen; i++)
        tmpbuf[i] = L' ';

    screenLines = screen->offset + screen->displayed + 1;

    attron(COLOR_PAIR(1));

    for(i = screen->offset; i < screenLines; i++) {
        if(i == screen->selected)
            attron(COLOR_PAIR(2));

        if (i < screen->list->linkCount) {
            if (screen->screenLineCount <= i)
                linkScreenRenderLine(screen, i, COLS);

            mvaddwstr(i - screen->offset, 0, screen->screenLines[i]);
        } else {
            mvaddwstr(i - screen->offset, 0, tmpbuf);
        }

        if (i == screen->selected)
            attron(COLOR_PAIR(1));
    }



    for (i = screenLines; i < screen->displayed + screen->offset + 1 + screen->linkOpenSize; i++)
        mvaddwstr(i, 0, tmpbuf);

    if (screen->linkOpen) {
        if (screen->helpOpen == 0)
            linkScreenRenderLinkText(screen, tmpbuf, bufLen, screenLines);
        else
            linkScreenRenderHelpText(screen, tmpbuf, bufLen, screenLines);
    }

    free(tmpbuf);

    refresh();
}

void linkScreenDown(LinkScreen *screen)
{
    screen->selected++;
    if (screen->selected > screen->offset + screen->displayed) {
        if (screen->offset + screen->displayed + 1 < screen->list->linkCount)
            screen->offset++;
        else
            screen->selected--;
    } else if (screen->selected + 1 > screen->list->linkCount) {
        screen->selected--;
    }
}

void linkScreenUp(LinkScreen *screen)
{
    screen->selected--;
    if (screen->selected < 0)
        screen->selected++;
    if (screen->selected < screen->offset)
        screen->offset--;
}

void linkScreenOpenLink(LinkScreen *screen)
{
    if (!screen->linkOpen) {
        screen->linkOpen = 1;
        screen->displayed -= screen->linkOpenSize - 1;
        if (screen->selected > screen->displayed + screen->offset)
            screen->offset = screen->selected - screen->displayed;
    }
}

void linkScreenCloseLink(LinkScreen *screen)
{
    if (screen->linkOpen) {
        screen->linkOpen = 0;
        screen->displayed += screen->linkOpenSize - 1;
    }
}

void linkScreenToggleLink(LinkScreen *screen)
{
    if (screen->linkOpen)
        linkScreenCloseLink(screen);
    else
        linkScreenOpenLink(screen);
}

void linkScreenOpenHelp(LinkScreen *screen)
{
    screen->helpOpen = 1;
}

void linkScreenCloseHelp(LinkScreen *screen)
{
    screen->helpOpen = 0;
}

void linkScreenToggleHelp(LinkScreen *screen)
{
    if (screen->helpOpen)
        linkScreenCloseHelp(screen);
    else
        linkScreenOpenHelp(screen);
}




#endif
