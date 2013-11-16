#ifndef _SRC_SIGNAL_H_
#define _SRC_SIGNAL_H_

typedef struct Callback Callback;
typedef struct Signal   Signal;

struct Callback {
    void (*callback) (va_list args);
    int id;
};

struct Signal {
    int nextId, callbackCount, callbackAllocCount;
    Callback *callbackList;
};

int  signalConnect (Signal *sig, void (*callback) (va_list args));
void signalDisconnect (Signal *sig, int callbackId);

void signalEmit  (Signal *sig, ...);
void signalEmitv (Signal *sig, va_list args);

void signalClear (Signal *sig);

#endif
