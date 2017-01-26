message(STATUS "src ...")

# get list of all source files in this directory.
AUX_SOURCE_DIRECTORY(.              MIC11API_SOURCE_FILES)

############### build the library #####################

if (_STATIC)

  MESSAGE(STATUS "building static library\n")
  add_library(mic11api        ${MIC11API_SOURCE_FILES})

else (_STATIC)

  MESSAGE(STATUS "building shered library\n")
  add_library(mic11api SHARED ${MIC11API_SOURCE_FILES})

endif()
