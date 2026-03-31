cmake_minimum_required(VERSION 3.20)

project(win_usb C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(
    winusb_core
    src/common.c
    src/devnode.c
    src/enumerate.c
    src/json_emit.c
    src/storage.c
    src/topology.c
)

target_include_directories(winusb_core PUBLIC "${PROJECT_SOURCE_DIR}/include")

if(MSVC)
    target_compile_options(winusb_core PRIVATE /W4 /WX-)
else()
    target_compile_options(winusb_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(winusb_core PUBLIC setupapi cfgmgr32)

add_executable(win_usb src/main.c)
target_link_libraries(win_usb PRIVATE winusb_core)

set_target_properties(win_usb PROPERTIES OUTPUT_NAME "win-usb")

enable_testing()

function(add_winusb_unit_test test_name test_source)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE winusb_core)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

add_winusb_unit_test(winusb_unit_common tests/unit/test_common.c)
add_winusb_unit_test(winusb_unit_devnode tests/unit/test_devnode.c)
add_winusb_unit_test(winusb_unit_enumerate tests/unit/test_enumerate.c)
add_winusb_unit_test(winusb_unit_json_emit tests/unit/test_json_emit.c)
add_winusb_unit_test(winusb_unit_storage tests/unit/test_storage.c)
add_winusb_unit_test(winusb_unit_topology tests/unit/test_topology.c)

set(
    WINUSB_UNIT_TEST_TARGETS
    winusb_unit_common
    winusb_unit_devnode
    winusb_unit_enumerate
    winusb_unit_json_emit
    winusb_unit_storage
    winusb_unit_topology
)

add_custom_target(winusb_unit_all DEPENDS ${WINUSB_UNIT_TEST_TARGETS})
