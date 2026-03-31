#include <limits.h>

#include "winusb/devnode.h"
#include "test_helpers.h"

static int g_failures = 0;

static void test_query_instance_string_with_invalid_handle(void) {
    SP_DEVINFO_DATA devinfo;
    wchar_t out[32];
    StringCchCopyW(out, ARRAYSIZE(out), L"seed");
    ZeroMemory(&devinfo, sizeof(devinfo));
    devinfo.cbSize = sizeof(devinfo);
    EXPECT_FALSE(query_instance_string(
        INVALID_HANDLE_VALUE, &devinfo, SPDRP_DEVICEDESC, out, ARRAYSIZE(out)));
    EXPECT_WSTREQ(L"", out);
}

static void test_devnode_registry_helpers_with_invalid_devinst(void) {
    DEVINST invalid_devinst = (DEVINST)ULONG_MAX;
    DWORD value = 42;
    BYTE dummy = 0;
    BYTE* buffer = &dummy;
    ULONG reg_type = 123;
    ULONG size = 999;
    char bcd[5];
    int bus_number = 7;

    StringCchCopyA(bcd, ARRAYSIZE(bcd), "FFFF");

    EXPECT_FALSE(get_devnode_reg_dword(invalid_devinst, CM_DRP_BUSNUMBER, &value));
    EXPECT_EQ_INT(42, (int)value);
    EXPECT_FALSE(
        get_devnode_reg_property(invalid_devinst, CM_DRP_LOCATION_PATHS, &reg_type, &buffer, &size));
    EXPECT_TRUE(buffer == NULL);
    EXPECT_TRUE(reg_type == 0);
    EXPECT_TRUE(size == 0);
    EXPECT_FALSE(parse_rev_from_hardware_ids(invalid_devinst, bcd));
    EXPECT_TRUE(bcd[0] == '\0');
    EXPECT_FALSE(derive_bus_number_from_location_paths(invalid_devinst, &bus_number));
    EXPECT_EQ_INT(7, bus_number);

    buffer = &dummy;
    reg_type = 111;
    size = 222;
    EXPECT_FALSE(get_devnode_reg_property(invalid_devinst, CM_DRP_HARDWAREID, &reg_type, &buffer, &size));
    EXPECT_TRUE(buffer == NULL);
    EXPECT_TRUE(reg_type == 0);
    EXPECT_TRUE(size == 0);
}

static void test_populate_driver_info_defaults(void) {
    char provider[128];
    char version[64];
    char inf[128];
    StringCchCopyA(provider, ARRAYSIZE(provider), "seed");
    StringCchCopyA(version, ARRAYSIZE(version), "seed");
    StringCchCopyA(inf, ARRAYSIZE(inf), "seed");
    populate_driver_info((DEVINST)ULONG_MAX, provider, version, inf);
    EXPECT_STREQ("N/A", provider);
    EXPECT_STREQ("N/A", version);
    EXPECT_STREQ("N/A", inf);
}

int main(void) {
    test_query_instance_string_with_invalid_handle();
    test_devnode_registry_helpers_with_invalid_devinst();
    test_populate_driver_info_defaults();

    if (g_failures != 0) {
        fprintf(stderr, "Unit tests failed: %d\n", g_failures);
        return 1;
    }
    puts("All devnode unit tests passed.");
    return 0;
}
