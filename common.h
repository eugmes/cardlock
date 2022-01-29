#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#ifdef __APPLE__
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#else
#include <winscard.h>
#endif

#ifdef _WIN32
#define CHECK(call) \
do { \
    LONG rv = call; \
    if (rv != SCARD_S_SUCCESS) { \
        fprintf(stderr, "%s:%d - %08lx\n", __FILE__, __LINE__, (unsigned long)rv); \
        exit(2); \
    } \
} while (0)
#else
#define CHECK(call) \
do { \
    LONG rv = call; \
    if (rv != SCARD_S_SUCCESS) { \
        fprintf(stderr, "%s:%d - %s\n", __FILE__, __LINE__, pcsc_stringify_error(rv)); \
        exit(2); \
    } \
} while (0)
#endif

#endif
