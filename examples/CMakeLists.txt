message(STATUS "examples ...")

ADD_EXECUTABLE(factory_up            UpdateFactoryExample.cpp)
TARGET_LINK_LIBRARIES(factory_up     mic11api)

ADD_EXECUTABLE(factory_tr            TransformFactoryExample.cpp)
TARGET_LINK_LIBRARIES(factory_tr     mic11api)

ADD_EXECUTABLE(fsm_up                FSMExample.cpp)
TARGET_LINK_LIBRARIES(fsm_up          mic11api)


