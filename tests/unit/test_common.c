#include <stdbool.h>

#include "winusb/common.h"
#include "test_helpers.h"

static int g_failures = 0;

static void test_is_excluded_pid(void) {
    EXPECT_TRUE(is_excluded_pid("0221"));
    EXPECT_TRUE(is_excluded_pid("0211"));
    EXPECT_TRUE(is_excluded_pid("0301"));
    EXPECT_FALSE(is_excluded_pid("9999"));
}

static void test_normalize_size_gb(void) {
    EXPECT_EQ_INT(512, normalize_size_gb("1408", 513.2));
    EXPECT_EQ_INT(500, normalize_size_gb("1408", 500.1));
    EXPECT_EQ_INT(32, normalize_size_gb("ABCD", 31.6));
    EXPECT_EQ_INT(31, normalize_size_gb("ABCD", 31.4));
    EXPECT_EQ_INT(0, normalize_size_gb("1408", 0.0));
    EXPECT_EQ_INT(0, normalize_size_gb("1408", -10.0));
}

static void test_now_ms_monotonic(void) {
    double t1 = now_ms();
    double t2 = now_ms();
    EXPECT_TRUE(t1 >= 0.0);
    EXPECT_TRUE(t2 >= t1);
}

static void test_wide_to_utf8(void) {
    char out[32];
    char tiny[1];

    StringCchCopyA(out, ARRAYSIZE(out), "seed");
    wide_to_utf8(L"Apricorn SSD", out, ARRAYSIZE(out));
    EXPECT_STREQ("Apricorn SSD", out);

    StringCchCopyA(out, ARRAYSIZE(out), "seed");
    wide_to_utf8(NULL, out, ARRAYSIZE(out));
    EXPECT_STREQ("", out);

    StringCchCopyA(out, ARRAYSIZE(out), "seed");
    wide_to_utf8(L"", out, ARRAYSIZE(out));
    EXPECT_STREQ("", out);

    tiny[0] = 'X';
    wide_to_utf8(L"A", tiny, ARRAYSIZE(tiny));
    EXPECT_TRUE(tiny[0] == '\0');

    StringCchCopyA(out, ARRAYSIZE(out), "keep");
    wide_to_utf8(L"A", out, 0);
    EXPECT_STREQ("keep", out);
}

static void test_parse_hex4_after_token(void) {
    char out[5] = {0};
    EXPECT_TRUE(parse_hex4_after_token(L"USB\\VID_04d8&PID_0310", L"PID_", out));
    EXPECT_STREQ("0310", out);

    EXPECT_TRUE(parse_hex4_after_token(L"USB\\VID_04d8&PID_ab1f", L"PID_", out));
    EXPECT_STREQ("AB1F", out);

    EXPECT_FALSE(parse_hex4_after_token(L"USB\\VID_04d8", L"PID_", out));
    EXPECT_FALSE(parse_hex4_after_token(L"USB\\PID_03G0", L"PID_", out));
    EXPECT_FALSE(parse_hex4_after_token(L"USB\\PID_03", L"PID_", out));
    EXPECT_FALSE(parse_hex4_after_token(NULL, L"PID_", out));
    EXPECT_FALSE(parse_hex4_after_token(L"USB\\PID_0310", NULL, out));
}

static void test_extract_serial_segment(void) {
    char out[128] = {0};

    extract_serial_segment(L"USB\\VID_04D8&PID_0310\\ABCD1234&0", out);
    EXPECT_STREQ("ABCD1234", out);

    extract_serial_segment(L"USBVID_04D8&PID_0310", out);
    EXPECT_STREQ("", out);

    extract_serial_segment(NULL, out);
    EXPECT_STREQ("", out);

    extract_serial_segment(L"USB\\VID_04D8&PID_0310\\SERIALONLY", out);
    EXPECT_STREQ("SERIALONLY", out);
}

static void test_extract_product_hint(void) {
    char out[128] = {0};
    extract_product_hint(L"USB\\VID_04D8&PID_0310&PROD_APRICORN_SSD#A1", out);
    EXPECT_STREQ("APRICORN SSD", out);

    extract_product_hint(L"USB\\VID_04D8&PID_0310&PROD_APRICORN_DEVICE&MI_00", out);
    EXPECT_STREQ("APRICORN DEVICE", out);

    extract_product_hint(L"USB\\VID_04D8&PID_0310", out);
    EXPECT_STREQ("", out);

    extract_product_hint(NULL, out);
    EXPECT_STREQ("", out);
}

static void test_vec_push_device(void) {
    DeviceVec vec = {0};
    ApricornDevice item;
    size_t i = 0;

    ZeroMemory(&item, sizeof(item));
    StringCchCopyA(item.id_product, ARRAYSIZE(item.id_product), "1408");

    for (i = 0; i < 10; ++i) {
        EXPECT_TRUE(vec_push_device(&vec, &item));
    }
    EXPECT_EQ_INT(10, (int)vec.count);
    EXPECT_TRUE(vec.cap >= vec.count);
    EXPECT_TRUE(vec.items != NULL);

    free(vec.items);
}

int main(void) {
    test_is_excluded_pid();
    test_normalize_size_gb();
    test_now_ms_monotonic();
    test_wide_to_utf8();
    test_parse_hex4_after_token();
    test_extract_serial_segment();
    test_extract_product_hint();
    test_vec_push_device();

    if (g_failures != 0) {
        fprintf(stderr, "Unit tests failed: %d\n", g_failures);
        return 1;
    }
    puts("All unit tests passed.");
    return 0;
}
