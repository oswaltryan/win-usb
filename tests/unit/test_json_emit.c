#include <io.h>

#include "winusb/json_emit.h"
#include "test_helpers.h"

static int g_failures = 0;

static char* read_entire_file(const char* path) {
    FILE* fp = NULL;
    long file_size = 0;
    size_t read_count = 0;
    char* content = NULL;

    if (fopen_s(&fp, path, "rb") != 0 || fp == NULL) {
        return NULL;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }
    file_size = ftell(fp);
    if (file_size < 0) {
        fclose(fp);
        return NULL;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return NULL;
    }

    content = (char*)malloc((size_t)file_size + 1);
    if (content == NULL) {
        fclose(fp);
        return NULL;
    }

    read_count = fread(content, 1, (size_t)file_size, fp);
    fclose(fp);
    if (read_count != (size_t)file_size) {
        free(content);
        return NULL;
    }
    content[file_size] = '\0';
    return content;
}

static char* capture_emit_json_output(const DeviceVec* devices,
                                      const ProfileStats* stats,
                                      bool include_profile) {
    char temp_dir[MAX_PATH];
    char temp_file[MAX_PATH];
    FILE* redirected = NULL;
    int saved_stdout_fd = -1;
    char* output = NULL;

    if (GetTempPathA(ARRAYSIZE(temp_dir), temp_dir) == 0) {
        return NULL;
    }
    if (GetTempFileNameA(temp_dir, "wju", 0, temp_file) == 0) {
        return NULL;
    }
    if (fopen_s(&redirected, temp_file, "wb") != 0 || redirected == NULL) {
        DeleteFileA(temp_file);
        return NULL;
    }

    saved_stdout_fd = _dup(_fileno(stdout));
    if (saved_stdout_fd < 0) {
        fclose(redirected);
        DeleteFileA(temp_file);
        return NULL;
    }

    fflush(stdout);
    if (_dup2(_fileno(redirected), _fileno(stdout)) < 0) {
        _close(saved_stdout_fd);
        fclose(redirected);
        DeleteFileA(temp_file);
        return NULL;
    }

    emit_json(devices, stats, include_profile);
    fflush(stdout);
    _dup2(saved_stdout_fd, _fileno(stdout));
    _close(saved_stdout_fd);
    fclose(redirected);

    output = read_entire_file(temp_file);
    DeleteFileA(temp_file);
    return output;
}

static void expect_contains(const char* haystack, const char* needle) {
    EXPECT_TRUE(haystack != NULL);
    EXPECT_TRUE(needle != NULL);
    if (haystack != NULL && needle != NULL) {
        EXPECT_TRUE(strstr(haystack, needle) != NULL);
    }
}

static void expect_not_contains(const char* haystack, const char* needle) {
    EXPECT_TRUE(haystack != NULL);
    EXPECT_TRUE(needle != NULL);
    if (haystack != NULL && needle != NULL) {
        EXPECT_TRUE(strstr(haystack, needle) == NULL);
    }
}

static void test_compare_devices_by_drive_num(void) {
    ApricornDevice a;
    ApricornDevice b;
    ApricornDevice c;
    ApricornDevice d;

    ZeroMemory(&a, sizeof(a));
    ZeroMemory(&b, sizeof(b));
    ZeroMemory(&c, sizeof(c));
    ZeroMemory(&d, sizeof(d));

    a.physical_drive_num = 5;
    b.physical_drive_num = 2;
    StringCchCopyA(a.serial, ARRAYSIZE(a.serial), "B-serial");
    StringCchCopyA(b.serial, ARRAYSIZE(b.serial), "A-serial");
    EXPECT_TRUE(compare_devices_by_drive_num(&a, &b) > 0);
    EXPECT_TRUE(compare_devices_by_drive_num(&b, &a) < 0);

    c.physical_drive_num = -1;
    d.physical_drive_num = 9;
    StringCchCopyA(c.serial, ARRAYSIZE(c.serial), "z");
    StringCchCopyA(d.serial, ARRAYSIZE(d.serial), "a");
    EXPECT_TRUE(compare_devices_by_drive_num(&c, &d) > 0);

    c.physical_drive_num = 3;
    d.physical_drive_num = 3;
    StringCchCopyA(c.serial, ARRAYSIZE(c.serial), "abc");
    StringCchCopyA(d.serial, ARRAYSIZE(d.serial), "ABC");
    EXPECT_EQ_INT(0, compare_devices_by_drive_num(&c, &d));
}

static void test_emit_json_empty_payload(void) {
    DeviceVec devices = {0};
    ProfileStats stats;
    char* output = NULL;

    ZeroMemory(&stats, sizeof(stats));
    stats.drive_letter_ms = 1.2;
    stats.enumeration_ms = 3.4;
    stats.total_ms = 5.6;

    output = capture_emit_json_output(&devices, &stats, false);
    EXPECT_TRUE(output != NULL);
    if (output != NULL) {
        expect_contains(output, "\"devices\": []");
        expect_not_contains(output, "\"profile\":");
    }
    free(output);

    output = capture_emit_json_output(&devices, &stats, true);
    EXPECT_TRUE(output != NULL);
    if (output != NULL) {
        expect_contains(output, "\"devices\": []");
        expect_contains(output, "\"profile\":");
        expect_contains(output, "\"driveLettersMs\": 1.20");
        expect_contains(output, "\"enumerationMs\": 3.40");
        expect_contains(output, "\"totalMs\": 5.60");
    }
    free(output);
}

static void test_emit_json_device_payload_branches(void) {
    ApricornDevice device;
    DeviceVec devices;
    ProfileStats stats;
    char* output = NULL;

    ZeroMemory(&device, sizeof(device));
    ZeroMemory(&stats, sizeof(stats));

    StringCchCopyA(device.id_vendor, ARRAYSIZE(device.id_vendor), "0984");
    StringCchCopyA(device.id_product, ARRAYSIZE(device.id_product), "1408");
    StringCchCopyA(device.bcd_device, ARRAYSIZE(device.bcd_device), "0310");
    StringCchCopyA(device.product, ARRAYSIZE(device.product), "Apricorn \"SSD\"");
    StringCchCopyA(device.serial, ARRAYSIZE(device.serial), "SER\\IAL");
    StringCchCopyA(device.driver_transport, ARRAYSIZE(device.driver_transport), "BOT");
    StringCchCopyA(device.media_type, ARRAYSIZE(device.media_type), "Basic Disk");
    StringCchCopyA(device.drive_letter, ARRAYSIZE(device.drive_letter), "C:");
    StringCchCopyA(device.file_system, ARRAYSIZE(device.file_system), "NTFS");
    StringCchCopyA(device.usb_driver_provider, ARRAYSIZE(device.usb_driver_provider), "Provider");
    StringCchCopyA(device.usb_driver_version, ARRAYSIZE(device.usb_driver_version), "1.0");
    StringCchCopyA(device.usb_driver_inf, ARRAYSIZE(device.usb_driver_inf), "usb.inf");
    StringCchCopyA(device.disk_driver_provider, ARRAYSIZE(device.disk_driver_provider), "Provider");
    StringCchCopyA(device.disk_driver_version, ARRAYSIZE(device.disk_driver_version), "2.0");
    StringCchCopyA(device.disk_driver_inf, ARRAYSIZE(device.disk_driver_inf), "disk.inf");
    StringCchCopyA(device.usb_controller, ARRAYSIZE(device.usb_controller), "Intel");
    device.bcd_usb = 3.2;
    device.bus_number = 2;
    device.device_address = 8;
    device.physical_drive_num = 1;
    device.read_only = 0;
    device.has_drive_size_gb = false;
    device.oob_mode = false;

    devices.items = &device;
    devices.count = 1;
    devices.cap = 1;

    output = capture_emit_json_output(&devices, &stats, false);
    EXPECT_TRUE(output != NULL);
    if (output != NULL) {
        expect_contains(output, "\"driveSizeGB\": \"N/A\"");
        expect_contains(output, "\"fileSystem\": \"NTFS\"");
        expect_contains(output, "\"iProduct\": \"Apricorn \\\"SSD\\\"\"");
        expect_contains(output, "\"iSerial\": \"SER\\\\IAL\"");
        expect_not_contains(output, "\"scbPartNumber\": \"N/A\"");
    }
    free(output);

    device.oob_mode = false;
    device.has_drive_size_gb = true;
    device.drive_size_gb = 4000;
    output = capture_emit_json_output(&devices, &stats, false);
    EXPECT_TRUE(output != NULL);
    if (output != NULL) {
        expect_contains(output, "\"driveSizeGB\": 4000");
        expect_contains(output, "\"fileSystem\": \"NTFS\"");
    }
    free(output);

    device.oob_mode = true;
    device.drive_size_gb = 2000;
    output = capture_emit_json_output(&devices, &stats, false);
    EXPECT_TRUE(output != NULL);
    if (output != NULL) {
        expect_contains(output, "\"driveSizeGB\": \"N/A (OOB Mode)\"");
        expect_contains(output, "\"scbPartNumber\": \"N/A\"");
        expect_contains(output, "\"hardwareVersion\": \"N/A\"");
        expect_not_contains(output, "\"fileSystem\":");
    }
    free(output);
}

int main(void) {
    test_compare_devices_by_drive_num();
    test_emit_json_empty_payload();
    test_emit_json_device_payload_branches();

    if (g_failures != 0) {
        fprintf(stderr, "Unit tests failed: %d\n", g_failures);
        return 1;
    }
    puts("All json_emit unit tests passed.");
    return 0;
}
