message(STATUS "src ...")


# get list of all source files in this directory.
AUX_SOURCE_DIRECTORY(.              MIC11API_SOURCE_FILES)

############### build the library #####################

#set(CMAKE_CXX_FLAGS)
add_library (mic11api   SHARED     ${MIC11API_SOURCE_FILES})

if (ROOT_FOUND)

  target_link_libraries(mic11api     ${ROOT_LIBRARIES} )

endif (ROOT_FOUND)