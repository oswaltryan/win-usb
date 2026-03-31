#include <limits.h>

#include "winusb/topology.h"
#include "test_helpers.h"

static int g_failures = 0;

static void test_classify_transport(void) {
    char out[16];

    classify_transport(L"SCSI\\DISK&VEN_X", NULL, out);
    EXPECT_STREQ("UAS", out);

    classify_transport(L"USBSTOR\\DISK&VEN_X", NULL, out);
    EXPECT_STREQ("BOT", out);

    classify_transport(L"scsi\\disk&ven_x", NULL, out);
    EXPECT_STREQ("UAS", out);

    classify_transport(L"usbstor\\disk&ven_x", NULL, out);
    EXPECT_STREQ("BOT", out);

    classify_transport(NULL, L"uaspstor", out);
    EXPECT_STREQ("UAS", out);

    classify_transport(NULL, L"usbstor.sys", out);
    EXPECT_STREQ("BOT", out);

    classify_transport(NULL, L"USBSTOR", out);
    EXPECT_STREQ("BOT", out);

    classify_transport(L"PCI\\VEN_1234", L"unknown", out);
    EXPECT_STREQ("Unknown", out);
}

static void test_find_usb_identity_invalid_start(void) {
    char vid[5];
    char pid[5];
    char bcd[5];
    char serial[128];
    DEVINST usb_devinst = (DEVINST)1;

    StringCchCopyA(vid, ARRAYSIZE(vid), "FFFF");
    StringCchCopyA(pid, ARRAYSIZE(pid), "FFFF");
    StringCchCopyA(bcd, ARRAYSIZE(bcd), "FFFF");
    StringCchCopyA(serial, ARRAYSIZE(serial), "SERIAL");

    EXPECT_FALSE(
        find_usb_identity((DEVINST)ULONG_MAX, vid, pid, bcd, serial, &usb_devinst));
    EXPECT_TRUE(vid[0] == '\0');
    EXPECT_TRUE(pid[0] == '\0');
    EXPECT_TRUE(bcd[0] == '\0');
    EXPECT_TRUE(serial[0] == '\0');
    EXPECT_TRUE(usb_devinst == 0);

    EXPECT_FALSE(find_usb_identity((DEVINST)ULONG_MAX, vid, pid, bcd, serial, NULL));
    EXPECT_TRUE(vid[0] == '\0');
    EXPECT_TRUE(pid[0] == '\0');
    EXPECT_TRUE(bcd[0] == '\0');
    EXPECT_TRUE(serial[0] == '\0');
}

static void test_classify_usb_controller_invalid(void) {
    char out[32];
    StringCchCopyA(out, ARRAYSIZE(out), "UNKNOWN");
    classify_usb_controller((DEVINST)ULONG_MAX, out);
    EXPECT_STREQ("N/A", out);
}

static void test_query_connection_metrics_invalid(void) {
    double bcd_usb = -1.0;
    int device_address = -123;
    char bcd_device[5];

    StringCchCopyA(bcd_device, ARRAYSIZE(bcd_device), "FFFF");

    EXPECT_FALSE(query_connection_metrics_from_parent_hub(
        (DEVINST)ULONG_MAX, "0984", "1408", &bcd_usb, &device_address, bcd_device));
    EXPECT_EQ_INT(-123, device_address);
    EXPECT_STREQ("FFFF", bcd_device);
}

int main(void) {
    test_classify_transport();
    test_find_usb_identity_invalid_start();
    test_classify_usb_controller_invalid();
    test_query_connection_metrics_invalid();

    if (g_failures != 0) {
        fprintf(stderr, "Unit tests failed: %d\n", g_failures);
        return 1;
    }
    puts("All topology unit tests passed.");
    return 0;
}
