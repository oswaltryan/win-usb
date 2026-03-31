#include <stdlib.h>

#include "winusb/storage.h"
#include "test_helpers.h"

static int g_failures = 0;

static bool get_windows_drive_letter_token(char out[3], wchar_t out_wide[3]) {
    wchar_t windows_dir[MAX_PATH];
    UINT len = GetWindowsDirectoryW(windows_dir, ARRAYSIZE(windows_dir));
    if (len < 3) {
        return false;
    }
    if (!iswalpha(windows_dir[0]) || windows_dir[1] != L':') {
        return false;
    }
    out[0] = (char)windows_dir[0];
    out[1] = ':';
    out[2] = '\0';
    out_wide[0] = windows_dir[0];
    out_wide[1] = L':';
    out_wide[2] = L'\0';
    return true;
}

static void test_lookup_drive_letters(void) {
    DriveLetterEntry entries[2];
    DriveLetterVec map;

    ZeroMemory(entries, sizeof(entries));
    entries[0].disk_number = 3;
    StringCchCopyW(entries[0].letters, ARRAYSIZE(entries[0].letters), L"E:, F:");
    entries[1].disk_number = 4;
    entries[1].letters[0] = L'\0';

    map.items = entries;
    map.count = ARRAYSIZE(entries);
    map.cap = ARRAYSIZE(entries);

    EXPECT_WSTREQ(L"E:, F:", lookup_drive_letters(&map, 3));
    EXPECT_WSTREQ(L"Not Formatted", lookup_drive_letters(&map, 4));
    EXPECT_WSTREQ(L"Not Formatted", lookup_drive_letters(&map, 99));
}

static void test_free_drive_letter_map(void) {
    DriveLetterVec map;
    map.items = (DriveLetterEntry*)malloc(2 * sizeof(DriveLetterEntry));
    map.count = 2;
    map.cap = 2;
    EXPECT_TRUE(map.items != NULL);
    if (map.items != NULL) {
        free_drive_letter_map(&map);
        EXPECT_TRUE(map.items == NULL);
        EXPECT_TRUE(map.count == 0);
        EXPECT_TRUE(map.cap == 0);
    }
}

static void test_storage_negative_paths(void) {
    int disk_number = -1;
    double size_gib = 0.0;
    double raw_gib = 0.0;
    bool oob_mode = false;
    int read_only = 0;
    double usb_bcd = 0.0;
    wchar_t fs[64];

    EXPECT_FALSE(get_disk_number_from_path(NULL, &disk_number));
    EXPECT_FALSE(get_disk_number_from_path(L"\\\\.\\DefinitelyNotARealDiskPath", &disk_number));
    EXPECT_FALSE(get_device_path_metrics(NULL, &raw_gib, &oob_mode, &read_only));
    EXPECT_FALSE(get_device_path_metrics(L"", &raw_gib, &oob_mode, &read_only));
    EXPECT_FALSE(get_physical_drive_metrics(-999, &raw_gib, &oob_mode, &read_only, &usb_bcd));
    EXPECT_FALSE(get_size_from_drive_letters("", &size_gib));
    EXPECT_FALSE(get_size_from_drive_letters("!:", &size_gib));
    EXPECT_FALSE(get_size_from_drive_letters(" , ,", &size_gib));

    EXPECT_TRUE(derive_filesystem_for_disk(1, L"Not Formatted", fs, ARRAYSIZE(fs)));
    EXPECT_WSTREQ(L"Unallocated", fs);
    EXPECT_TRUE(derive_filesystem_for_disk(1, NULL, fs, ARRAYSIZE(fs)));
    EXPECT_WSTREQ(L"Unallocated", fs);
    EXPECT_TRUE(derive_filesystem_for_disk(1, L"invalid", fs, ARRAYSIZE(fs)));
    EXPECT_WSTREQ(L"Unallocated", fs);
    EXPECT_FALSE(derive_filesystem_for_disk(1, NULL, NULL, 0));
}

static void test_storage_with_real_system_drive(void) {
    char drive_token[3];
    wchar_t drive_token_w[3];
    double size_gib = 0.0;
    wchar_t fs[64];

    if (!get_windows_drive_letter_token(drive_token, drive_token_w)) {
        return;
    }

    EXPECT_TRUE(get_size_from_drive_letters(drive_token, &size_gib));
    EXPECT_TRUE(size_gib > 0.0);

    EXPECT_TRUE(derive_filesystem_for_disk(0, drive_token_w, fs, ARRAYSIZE(fs)));
    EXPECT_TRUE(fs[0] != L'\0');

    EXPECT_TRUE(derive_filesystem_for_disk(0, L" c:, C: ", fs, ARRAYSIZE(fs)));
    EXPECT_TRUE(fs[0] != L'\0');
}

int main(void) {
    test_lookup_drive_letters();
    test_free_drive_letter_map();
    test_storage_negative_paths();
    test_storage_with_real_system_drive();

    if (g_failures != 0) {
        fprintf(stderr, "Unit tests failed: %d\n", g_failures);
        return 1;
    }
    puts("All storage unit tests passed.");
    return 0;
}
