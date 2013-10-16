#ifndef _SRC_SCREEN_H_
#define _SRC_SCREEN_H_

typedef struct ScreenLine ScreenLine;
typedef struct Screen     Screen;

struct WindowLine {

};

struct Window {
    int bufCount;
    int bufWidth;
    wchar_t *buffer[];

    int minRows;
    int minCols;

    unsigned int open    :1;
    unsigned int updated :1;
    void *extra;
};

struct Screen {
    int rows;
    int cols;
    int windowCount;
    Window *windows[];
    int    *curRows;
    int    *curCols;
};




#endif
