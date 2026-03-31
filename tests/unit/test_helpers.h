#ifndef WINUSB_TEST_HELPERS_H
#define WINUSB_TEST_HELPERS_H

#include <stdio.h>
#include <string.h>
#include <wchar.h>

#define EXPECT_TRUE(cond)                                                             \
    do {                                                                              \
        if (!(cond)) {                                                                \
            fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond);         \
            ++g_failures;                                                             \
        }                                                                             \
    } while (0)

#define EXPECT_FALSE(cond) EXPECT_TRUE(!(cond))

#define EXPECT_EQ_INT(expected, actual)                                               \
    do {                                                                               \
        int expected_val = (expected);                                                 \
        int actual_val = (actual);                                                     \
        if (expected_val != actual_val) {                                              \
            fprintf(stderr,                                                             \
                    "FAIL: %s:%d: expected %d, got %d\n",                              \
                    __FILE__,                                                           \
                    __LINE__,                                                           \
                    expected_val,                                                       \
                    actual_val);                                                        \
            ++g_failures;                                                              \
        }                                                                              \
    } while (0)

#define EXPECT_STREQ(expected, actual)                                                \
    do {                                                                               \
        if (strcmp((expected), (actual)) != 0) {                                       \
            fprintf(stderr,                                                             \
                    "FAIL: %s:%d: expected \"%s\", got \"%s\"\n",                      \
                    __FILE__,                                                           \
                    __LINE__,                                                           \
                    (expected),                                                         \
                    (actual));                                                          \
            ++g_failures;                                                              \
        }                                                                              \
    } while (0)

#define EXPECT_WSTREQ(expected, actual)                                               \
    do {                                                                               \
        if (wcscmp((expected), (actual)) != 0) {                                       \
            fwprintf(stderr,                                                            \
                     L"FAIL: %S:%d: expected \"%ls\", got \"%ls\"\n",                  \
                     __FILE__,                                                          \
                     __LINE__,                                                          \
                     (expected),                                                        \
                     (actual));                                                         \
            ++g_failures;                                                              \
        }                                                                              \
    } while (0)

#endif
