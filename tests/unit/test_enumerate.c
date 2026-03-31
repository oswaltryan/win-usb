#include <stdlib.h>

#include "winusb/enumerate.h"
#include "test_helpers.h"

static int g_failures = 0;

static void assert_device_invariants(const ApricornDevice* d) {
    EXPECT_TRUE(d->id_vendor[0] != '\0');
    EXPECT_TRUE(d->id_product[0] != '\0');
    EXPECT_TRUE(d->driver_transport[0] != '\0');
    EXPECT_TRUE(d->physical_drive_num >= -1);
    EXPECT_TRUE(d->bus_number >= -1);
    EXPECT_TRUE(d->device_address >= -1);
    if (d->has_drive_size_gb) {
        EXPECT_TRUE(d->drive_size_gb > 0);
    }
    if (d->oob_mode) {
        EXPECT_FALSE(d->has_drive_size_gb);
    }
}

static void test_enumerate_smoke_and_invariants(void) {
    DeviceVec devices = {0};
    ProfileStats stats;
    bool ok = false;
    size_t i = 0;

    ZeroMemory(&stats, sizeof(stats));

    ok = enumerate_apricorn_devices(&devices, &stats);
    EXPECT_TRUE(devices.count <= devices.cap);
    EXPECT_TRUE(stats.drive_letter_ms >= 0.0);
    EXPECT_TRUE(stats.enumeration_ms >= 0.0);
    if (!ok) {
        EXPECT_TRUE(devices.count == 0 || devices.items != NULL);
    }

    for (i = 0; i < devices.count; ++i) {
        assert_device_invariants(&devices.items[i]);
    }

    free(devices.items);
}

static void test_enumerate_repeatable_calls(void) {
    DeviceVec devices1 = {0};
    DeviceVec devices2 = {0};
    ProfileStats stats1;
    ProfileStats stats2;
    bool ok1 = false;
    bool ok2 = false;

    ZeroMemory(&stats1, sizeof(stats1));
    ZeroMemory(&stats2, sizeof(stats2));

    ok1 = enumerate_apricorn_devices(&devices1, &stats1);
    ok2 = enumerate_apricorn_devices(&devices2, &stats2);

    EXPECT_TRUE(stats1.drive_letter_ms >= 0.0);
    EXPECT_TRUE(stats2.drive_letter_ms >= 0.0);
    EXPECT_TRUE(stats1.enumeration_ms >= 0.0);
    EXPECT_TRUE(stats2.enumeration_ms >= 0.0);

    if (ok1 && ok2) {
        EXPECT_TRUE(devices1.count <= devices1.cap);
        EXPECT_TRUE(devices2.count <= devices2.cap);
    }

    free(devices1.items);
    free(devices2.items);
}

int main(void) {
    test_enumerate_smoke_and_invariants();
    test_enumerate_repeatable_calls();

    if (g_failures != 0) {
        fprintf(stderr, "Unit tests failed: %d\n", g_failures);
        return 1;
    }
    puts("All enumerate unit tests passed.");
    return 0;
}
