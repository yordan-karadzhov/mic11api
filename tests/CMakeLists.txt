message(STATUS "mic11tests lib ...")

# get list of all source files in this directory.
AUX_SOURCE_DIRECTORY(.              TESTS_SOURCE_FILES)
# message(STATUS "TESTS_SOURCE_FILES " ${TESTS_SOURCE_FILES})

add_library (mic11tests      ${TESTS_SOURCE_FILES}
                             ${Data_DICTIONARY})

target_link_libraries(mic11tests     mic11api cppunit)

ADD_EXECUTABLE(unit_test              UnitTests.cpp)
TARGET_LINK_LIBRARIES(unit_test       mic11tests)

