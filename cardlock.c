#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <synchapi.h>
#else
#include <unistd.h>
#endif

#include "common.h"

int main(int argc, char **argv)
{
    SCARDCONTEXT context;
    SCARDHANDLE card;

    DWORD activeProtocol;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <reader-name>\n", argv[0]);
        return 1;
    }
    const char *reader_name = argv[1];

    CHECK(SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &context));
    fprintf(stderr, "connecting...\n");
    CHECK(SCardConnect(context, reader_name, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &card, &activeProtocol));

    fprintf(stderr, "beginning transaction...\n");
    CHECK(SCardBeginTransaction(card));

    while (1) {
#ifdef _WIN32
        Sleep(2000);
#else
        sleep(2);
#endif
        fprintf(stderr, "status check\n");
        CHECK(SCardStatus(card, NULL, NULL, NULL, NULL, NULL, NULL));
    }

    return 0;
}
