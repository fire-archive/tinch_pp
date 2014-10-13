SET(Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0" "1.43" "1.43.0" "1.44" "1.45" "1.46" "1.46.1" )
find_package( Boost 1.42.0 REQUIRED )

SET(CPP_SOURCE
  actual_mailbox.cpp
  actual_node.cpp
  constants.cpp
  control_msgs_impl.cpp
  ctrl_msg_dispatcher.cpp
  epmd_requestor.cpp
  erlang_value_types.cpp
  erl_any.cpp
  erl_string.cpp
  exceptions.cpp
  ext_message_builder.cpp
  link_policies.cpp
  linker.cpp
  matchable_range.cpp
  matchable_seq.cpp
  md5.cpp
  node_async_tcp_ip.cpp
  node_connection.cpp
  node_connection_state.cpp
  node_connector.cpp
  node.cpp
  rpc.cpp
  type_makers.cpp
  types.cpp
  utils.cpp
)

add_definitions(-DBOOST_RESULT_OF_USE_DECLTYPE)
add_definitions(-DBOOST_SPIRIT_USE_PHOENIX_V3)
add_definitions(-DBOOST_ASIO_HAS_BOOST_BIND)
add_definitions(-DBOOST_BIND_NO_PLACEHOLDERS)
add_definitions(-DTR2_OPTIONAL_DISABLE_EMULATION_OF_TYPE_TRAITS)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

include_directories(${CMAKE_SOURCE_DIR} ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/ThirdParty/Optional)
add_library(tinch++ ${CPP_SOURCE})

install(TARGETS tinch++ 
  DESTINATION ${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}/lib )

file(GLOB HEADER_FILES *.h)
install(FILES 
    ${HEADER_FILES}
  DESTINATION ${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}/impl
  )

