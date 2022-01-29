#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <synchapi.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif

#include "common.h"

struct args {
    SCARDCONTEXT context;
    const char *reader_name;
};

static
#ifdef _WIN32
DWORD
#else
void *
#endif
run(void *arg)
{
    struct args *args = arg;

    SCARDHANDLE card;
    DWORD activeProtocol;
    fprintf(stderr, "connecting...\n");
    CHECK(SCardConnect(args->context, args->reader_name, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &card, &activeProtocol));

    fprintf(stderr, "connected\n");
    CHECK(SCardDisconnect(card, SCARD_LEAVE_CARD));
    fprintf(stderr, "disconnected\n");
    return 0;
}

int main(int argc, char **argv)
{
    SCARDCONTEXT context;

    CHECK(SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &context));

    if (argc != 2) {
        char readers[1024];
        DWORD listSize = sizeof(readers);
        CHECK(SCardListReaders(context, NULL, readers, &listSize));

        for (const char *p = readers; *p; p += strlen(p) + 1) {
            printf("%s\n", p);
        }

        fprintf(stderr, "Usage: %s <reader-name>\n", argv[0]);
        return 1;
    }


    struct args thread_args = {
        .context = context,
        .reader_name = argv[1],
    };

#ifdef _WIN32
    DWORD thread_id;
    HANDLE thread = CreateThread(NULL, 0, run, &thread_args, 0, &thread_id);
    if (thread == NULL) {
        fprintf(stderr, "CreateThread failed\n");
        return 10;
    }
    Sleep(5000);
#else
    pthread_t thread;
    pthread_attr_t attr;

    int ret = pthread_attr_init(&attr);
    if (ret != 0) {
        perror("pthread_arg_init");
        return 3;
    }

    ret = pthread_create(&thread, &attr, run, &thread_args);
    if (ret != 0) {
        perror("pthread_create");
        return 3;
    }

    sleep(5);
#endif
    fprintf(stderr, "cancelling\n");
    CHECK(SCardCancel(context));
    CHECK(SCardReleaseContext(context));
    fprintf(stderr, "done\n");

#ifdef _WIN32
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
#else
    pthread_join(thread, NULL);
#endif

    return 0;
}
