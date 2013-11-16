#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef REDDIT_DEBUG
# include <stdio.h>
# include <wchar.h>
#endif

/* Quick Macros to get a quoted version of the contents of the macro x */
#define QQ(x) #x
#define Q(x) QQ(x)

/*
 * If you compile with -DREDDIT_DEBUG, then debugging will be turned-on program-wide
 *
 * DEBUG_FILE is a module-specefic define macro that is set per-module
 *   representing the FILE* to send debug information to (Ex. libreddit sets it
 *   differently then creddit does)
 * DEBUG_MODULE is a module-specefic defined macro that expands to a string
 *   with that modules name
 * DEBUG_PRINT is a macro that expands into a wide-character print to this file
 */
#ifdef REDDIT_DEBUG
# define DEBUG_PRINT(...) \
    do { \
        fwprintf(DEBUG_FILE, L"%s: %s: %d: ", DEBUG_MODULE, __FILE__, __LINE__); \
        fwprintf(DEBUG_FILE, __VA_ARGS__); \
        fflush(DEBUG_FILE); \
    } while (0)
#else
# define DEBUG_PRINT(...) do { ; } while (0)
#endif


#endif
