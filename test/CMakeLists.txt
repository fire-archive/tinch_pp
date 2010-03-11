set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
include(Erlang)

option(BUILD_TEST "Build test applications" ON)
option(INSTALL_TEST "Install test applications" ON)

if(BUILD_TEST)
  SET(Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0")

  find_package( Boost 1.42.0 
    COMPONENTS
      thread
      system
      regex
      signals
      program_options
    REQUIRED )
 
  find_program( ERLANG_ERLC erlc
    DOC "Erlang compiler")

  link_directories(${Boost_LIBRARY_DIRS})

  add_erlang(reflect_msg chat_server)

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
  
  add_executable(chat_client chat_client.cpp)
  target_link_libraries(chat_client tinch++ ${Boost_LIBRARIES})

  if(INSTALL_TEST)
    install(TARGETS net_kernel_sim patterns rpc_test thread_safe_queue chat_client
      DESTINATION ${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}/test )

    if(ERLANG_OUTPUT_FILES)
      install(
	FILES 
	  ${ERLANG_OUTPUT_FILES}
	DESTINATION ${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}/test )
    endif(ERLANG_OUTPUT_FILES)
  endif(INSTALL_TEST)
endif(BUILD_TEST)
