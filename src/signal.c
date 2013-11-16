
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "signal.h"

int signalConnect (Signal *sig, void (*callback) (va_list args))
{
    int c = sig->callbackCount++;
    Callback *call;
    if (c >= sig->callbackAllocCount) {
        sig->callbackAllocCount += 10;
        sig->callbackList = realloc(sig->callbackList, sig->callbackAllocCount * sizeof(Callback));
    }

    call = sig->callbackList + c;
    call->callback = callback;
    call->id = sig->nextId++;

    return call->id;
}

void signalDisconnect (Signal *sig, int id)
{
    int i, end = sig->callbackCount;
    for (i = 0; i < end; i++) {
        if (sig->callbackList[i].id == id) {
            memmove(sig->callbackList + i, sig->callbackList + i + 1, (end - i) * sizeof(Callback));
            sig->callbackCount--;
            return ;
        }
    }
}

void signalEmitv (Signal *sig, va_list args)
{
    int i, end = sig->callbackCount;
    va_list v;
    for (i = 0; i < end; i++) {
        va_copy(v, args);
        (sig->callbackList[i].callback) (v);
        va_end(v);
    }
}

void signalEmit (Signal *sig, ...)
{
    va_list v;
    va_start(v, sig);
    signalEmitv(sig, v);
    va_end(v);
}

void signalClear (Signal *sig)
{
    free(sig->callbackList);
}
    
