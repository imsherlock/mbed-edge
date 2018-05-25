if (NOT TARGET_GROUP STREQUAL test)
  file (GLOB SOURCES ./*.cpp)

  include_directories (${CMAKE_CURRENT_LIST_DIR}/include)

  add_executable (ble-scanner ${SOURCES})

  add_definitions(-DMBED_CONF_MBED_TRACE_ENABLE=1)

  target_include_directories (ble-scanner PUBLIC ${CMAKE_CURRENT_LIST_DIR})

  target_link_libraries (ble-scanner pthread pt-client bluetooth)
endif()
