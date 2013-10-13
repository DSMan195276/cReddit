#define _XOPEN_SOURCE_EXTENDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "reddit.h"
#include <ncurses.h>
#include <form.h>
#include <locale.h>

#include "comment_screen.h"
#include "link_screen.h"

#define SIZEOFELEM(x)  (sizeof(x) / sizeof(x[0]))




RedditState *globalState;

wchar_t *linkScreenHelp[19] = {
    L"Keypresses:",
    L"Link Screen:",
    L"- k / UP -- Move up one link in the list",
    L"- j / DOWN -- Move down one link in the list",
    L"- L -- Get the next list of Links from Reddit",
    L"- u -- Update the list (Clears the list of links, and then gets a new list from Reddit)",
    L"- l / ENTER -- Open the selected link",
    L"- c -- Display the comments for the selected link",
    L"- q -- Close open Link, or exit program if no link is open",
    L"- ? -- Opens the help(but you wouldn't be reading this if you couldn't figure that out would you?)",
    L"",
    L"Comment screen:",
    L"- k / UP -- Move up one comment in the list",
    L"- j / DOWN -- Move down one comment in the list",
    L"- l / ENTER -- Open the selected comment",
    L"- q / h -- Close the open comment, or close the comment screen if no comment is open",
    L"",
    L"To report any bugs, submit patches, etc. Please see the github page at:",
    L"http://www.github.com/Cotix/cReddit\n"
};

void showThread(RedditLink *link)
{
    CommentScreen *screen = NULL;
    RedditCommentList *list = NULL;
    RedditErrno err;

    if (link == NULL)
        return ;

    list = redditCommentListNew();
    list->permalink = redditCopyString(link->permalink);

    err = redditGetCommentList(list);
    if (err != REDDIT_SUCCESS || list->baseComment->replyCount == 0)
        goto cleanup;

    screen = getCommentScreenFromCommentList(list, COLS);

    screen->offset = 0;
    screen->selected = 0;
    screen->displayed = LINES - 1;
    screen->commentOpenSize = (screen->displayed / 5) * 4;
    commentScreenDisplay(screen);
    int c;
    while((c = wgetch(stdscr))) {
        switch(c) {
            case 'j': case KEY_DOWN:
                commentScreenDown(screen);
                commentScreenDisplay(screen);
                break;
            case 'k': case KEY_UP:
                commentScreenUp(screen);
                commentScreenDisplay(screen);
                break;

            case 'l': case '\n': case KEY_ENTER:
                commentScreenToggleComment(screen);
                commentScreenDisplay(screen);
                break;
            case 'q': case 'h':
                if (screen->commentOpen) {
                    commentScreenCloseComment(screen);
                    commentScreenDisplay(screen);
                } else {
                    goto cleanup;
                }
                break;
        }
    }

cleanup:;
    redditCommentListFree(list);
    commentScreenFree(screen);
    return ;
}

void showSubreddit(const char *subreddit)
{
    LinkScreen *screen;
    screen = linkScreenNew();

    screen->list = redditLinkListNew();
    screen->list->subreddit = redditCopyString(subreddit);
    screen->list->type = REDDIT_HOT;

    redditGetListing(screen->list);

    screen->displayed = LINES - 1;
    screen->linkOpenSize = (screen->displayed / 5) * 4;

    screen->offset = 0;
    screen->selected = 0;

    /* Assign help-screen text */
    screen->helpText = linkScreenHelp;
    screen->helpLineCount = 19;


    drawScreen(screen); //And print the screen!

    int c;
    while((c = wgetch(stdscr))) {
        switch(c) {
            case 'k': case KEY_UP:
                linkScreenUp(screen);
                drawScreen(screen);
                break;

            case 'j': case KEY_DOWN:
                linkScreenDown(screen);
                drawScreen(screen);
                break;
            case 'q':
                if (screen->linkOpen) {
                    linkScreenCloseLink(screen);
                    drawScreen(screen);
                } else {
                    goto cleanup;
                }
                break;
            case 'u':
                redditLinkListFreeLinks(screen->list);
                redditGetListing(screen->list);
                screen->offset = 0;
                screen->selected = 0;
                drawScreen(screen);
                break;
            case 'l': case '\n': case KEY_ENTER:
                if (screen->helpOpen)
                    linkScreenCloseHelp(screen);
                else
                    linkScreenToggleLink(screen);
                drawScreen(screen);
                break;
            case 'L':
                redditGetListing(screen->list);
                drawScreen(screen);
                break;
            case 'c':
                showThread(screen->list->links[screen->selected]);
                drawScreen(screen);
                break;
            case '?':
                linkScreenToggleHelp(screen);
                if (screen->helpOpen)
                    linkScreenOpenLink(screen);
                drawScreen(screen);
                break;
        }
    }

cleanup:;
    redditLinkListFree(screen->list);
    linkScreenFree(screen);
}

int startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre), lenstr = strlen(str);
    return (lenstr < lenpre) ? 0 : strncmp(pre, str, lenpre) == 0;
}

/*
 * Prepends 'pre' onto the front of the string buffer 'str'
 * Make sure 'str' is large enough to fit 'pre' on it.
 */
void prepend(const char *pre, char *str)
{
    size_t lenpre = strlen(pre), lenstr = strlen(str);

    /* Move the Strings memory forward */
    memcpy(str + lenpre, str, lenstr + 1);
    /* Copy pre into the new space */
    memcpy(str, pre, lenpre);
}

int main(int argc, char *argv[])
{
    RedditUserLogged *user = NULL;
    char *subreddit = NULL;

    if (argc > 1) {
        subreddit = argv[1];
        /* Display a simple help screen */
        if (!startsWith("/r/", subreddit) && strcmp("/", subreddit) != 0) {
            subreddit = malloc((strlen(argv[1]) + 4) * sizeof(char));
            strcpy(subreddit, argv[1]);
            prepend("/r/", subreddit);
        }

        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "help") == 0) {
            printf("Usage: %s [subreddit] [username] [password]\n", argv[0]);
            printf("\n");
            printf(" subreddit -- The name of a subreddit you want to view, Ex. '/r/coding', '/r/linux'\n");
            printf(" username  -- The username of a user you want to login as.\n");
            printf(" password  -- The password for the username, if one was given\n");
            printf("\n");
            printf("To report any bugs, submit patches, etc. Please see the github page at:\n");
            printf("http://www.github.com/Cotix/cReddit\n");
            return 0;
        }
    } else {
        subreddit = "/";
    }


    setlocale(LC_CTYPE, "");


    initscr();
    raw();//We want character for character input
    keypad(stdscr,1);//Enable extra keys like arrowkeys
    noecho();
    start_color();
    use_default_colors();
    init_pair(1, -1, -1);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);

    /* Start libreddit */
    redditGlobalInit();

    globalState = redditStateNew();

    redditStateSet(globalState);

    if (argc == 4) {
        user = redditUserLoggedNew();
        redditUserLoggedLogin(user, argv[2], argv[3]);
    }
    showSubreddit(subreddit);

    redditUserLoggedFree(user);
    redditStateFree(globalState);
    redditGlobalCleanup();
    endwin();
    return 0;
}


