include_directories(include)

set(TEST_SOURCES
    "src/test_audiobuffer.c"
    "src/test_stretch.c"
    "src/test_fft.c"
    "src/test_full_algorithm.c"
    )

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable("${TEST_NAME}" ${TEST_FILE})
    target_link_libraries("${TEST_NAME}" pstretch)
    add_test("${TEST_NAME}" "${TEST_NAME}")
endforeach(TEST_FILE)

