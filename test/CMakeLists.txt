SET(Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0")

find_package( Boost 1.42.0 
  COMPONENTS 
    thread 
    system 
    regex 
    signals
    program_options
  REQUIRED )

add_definitions(-DBOOST_ALL_NO_LIB)
include_directories(${CMAKE_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

add_executable(net_kernel_sim net_kernel_sim.cpp)
target_link_libraries(net_kernel_sim tinch++ ${Boost_LIBRARIES})

add_executable(patterns patterns.cpp)
target_link_libraries(patterns tinch++ ${Boost_LIBRARIES})

add_executable(patterns_testing_any patterns_testing_any.cpp)
target_link_libraries(patterns_testing_any tinch++ ${Boost_LIBRARIES})

add_executable(patterns_testing_assign patterns_testing_assign.cpp)
target_link_libraries(patterns_testing_assign tinch++ ${Boost_LIBRARIES})

add_executable(rpc_test rpc_test.cpp)
target_link_libraries(rpc_test tinch++ ${Boost_LIBRARIES})

add_executable(thread_safe_queue thread_safe_queue.cpp)
target_link_libraries(thread_safe_queue tinch++ ${Boost_LIBRARIES})

install(TARGETS net_kernel_sim patterns rpc_test 
  DESTINATION ${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}/test )