#define _XOPEN_SOURCE_EXTENDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "reddit.h"
#include <ncurses.h>
#include <form.h>
#include <locale.h>

#include "global.h"
#include "opt.h"
#include "screens/link_screen.h"
#include "screens/comment_screen.h"


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

    /*screen = getCommentScreenFromCommentList(list, COLS); */
    screen = commentScreenNew();

    screen->offset = 0;
    screen->selected = 0;
    screen->displayed = LINES - 1;
    screen->commentOpenSize = (screen->displayed / 5) * 4;
    screen->list = list;
    screen->width = COLS;

    commentScreenRenderLines(screen);
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

            case 'm':
                redditGetCommentChildren(screen->list, screen->lines[screen->selected]->comment);
                commentScreenRenderLines(screen);
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
    DEBUG_PRINT(L"Loading Subreddit %s\n", subreddit);
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
    memmove(str + lenpre, str, lenstr + 1);
    /* Copy pre into the new space */
    memmove(str, pre, lenpre);
}

#define MOPT_SUBREDDIT 0
#define MOPT_USERNAME  1
#define MOPT_PASSWORD  2
#define MOPT_HELP      3
#define MOPT_ARG_COUNT 4

optOption mainOptions[MOPT_ARG_COUNT] = {
    OPT_STRING("subreddit", 's', "The name of a subreddit you want to open", ""),
    OPT_STRING("username",  'u', "A Reddit username to login as",            ""),
    OPT_STRING("password",  'p', "Password for the provided username",       ""),
    OPT       ("help",      'h', "Display command-line arguments help-text")
};

char *getPassword()
{
    const int MAX_PASSWD_SIZE = 200;
    char *password = malloc(MAX_PASSWD_SIZE);

    clear();
    mvprintw(0, 0, "Password: ");
    refresh();

    getnstr(password, MAX_PASSWD_SIZE);
    password[MAX_PASSWD_SIZE - 1] = '\0';
    return password;
}

void handleArguments (optParser *parser)
{
    optResponse res;
    int unusedCount = 0;
    int optUnused[3] = { MOPT_SUBREDDIT, MOPT_USERNAME, MOPT_PASSWORD };

    do {
       res = optRunParser(parser);
       if (res == OPT_UNUSED) {
           if (unusedCount < (sizeof(optUnused)/sizeof(optUnused[0]))) {
               int optC = optUnused[unusedCount];
               strcpy(mainOptions[optC].svalue, parser->argv[parser->curopt]);
               mainOptions[optC].isSet = 1;
           }

           unusedCount++;
       }

    } while (res != OPT_SUCCESS);
}

void displayCmd (optOption *option)
{
    printf("   ");
    if (option->opt_long[0] != '\0')
        printf("--%-10s ", option->opt_long);
    else
        printf("             ");

    if (option->opt_short != '\0')
        printf("-%c ", option->opt_short);
    else
        printf("   ");

    switch(option->arg) {
    case OPT_STRING:
        printf("[String] ");
        break;
    case OPT_INT:
        printf("[Int]    ");
        break;
    case OPT_NONE:
        printf("         ");
        break;
    }

    printf(" : %s\n", option->helpText);
}

void displayHelp (optParser *parser)
{
    optOption **option;

    printf("Usage: %s [flags] [subreddit] [username] [password] \n\n", parser->argv[0]);
    printf("Below is a list of all the flags reconized by creddit.\n");
    printf(" [String] --> argument takes a string\n");
    printf(" [Int]    --> argument takes an integer value\n");

    printf("\n");
    for (option = parser->options; *option != NULL; option++)
        displayCmd(*option);

    printf("\nYou will be prompted for a password if you don't include one\n");

    printf("\nTo report any bugs, submit patches, etc. Please see the github page at:\n");
    printf("http://www.github.com/Cotix/cReddit\n");
}

int main(int argc, char *argv[])
{
    RedditUserLogged *user = NULL;
    char *subreddit = NULL;
    char *password = NULL, *username = NULL;
    optParser parser;

    DEBUG_START(DEBUG_FILE, DEBUG_FILENAME);

    memset(&parser, 0, sizeof(optParser));

    parser.argc = argc;
    parser.argv = argv;

    optAddOptions (&parser, mainOptions, MOPT_ARG_COUNT);

    handleArguments(&parser);

    if (mainOptions[MOPT_HELP].isSet) {
        displayHelp(&parser);
        return 0;
    }

    optClearParser(&parser);

    setlocale(LC_CTYPE, "");


    initscr();
    raw();//We want character for character input
    keypad(stdscr,1);//Enable extra keys like arrowkeys
    noecho();
    start_color();
    use_default_colors();
    init_pair(1, -1, -1);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);

    DEBUG_PRINT(L"Starting...\n");

    /* Start libreddit */
    redditGlobalInit();

    globalState = redditStateNew();

    globalState->userAgent = redditCopyString("cReddit/0.0.1");

    redditStateSet(globalState);

    if (mainOptions[MOPT_USERNAME].isSet) {
        username = mainOptions[MOPT_USERNAME].svalue;
        if (!mainOptions[MOPT_PASSWORD].isSet)
            password = getPassword();
        else
            password = mainOptions[MOPT_PASSWORD].svalue;

        user = redditUserLoggedNew();
        redditUserLoggedLogin(user, username, password);

        /* Don't want to leave that important Reddit password in memory */
        memset(password, 0, strlen(password));
        if (!mainOptions[MOPT_PASSWORD].isSet)
            free(password);
    }
    if (mainOptions[MOPT_SUBREDDIT].isSet) {
        subreddit = mainOptions[MOPT_SUBREDDIT].svalue;
        if (!startsWith("/r/", subreddit) && strcmp("/", subreddit) != 0)
            prepend("/r/", subreddit);

    } else {
        subreddit = "/";
    }
    showSubreddit(subreddit);

    redditUserLoggedFree(user);
    redditStateFree(globalState);
    redditGlobalCleanup();
    endwin();

    DEBUG_END(DEBUG_FILE);
    return 0;
}


