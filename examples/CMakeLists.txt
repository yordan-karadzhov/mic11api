message(STATUS "examples ...")

ADD_EXECUTABLE(integration_test_up            API_IntegrationTest_Up.cpp)
TARGET_LINK_LIBRARIES(integration_test_up     mic11api)

ADD_EXECUTABLE(integration_test_tr            API_IntegrationTest_Tr.cpp)
TARGET_LINK_LIBRARIES(integration_test_tr     mic11api)



