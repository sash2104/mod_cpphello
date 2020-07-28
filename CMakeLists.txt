cmake_minimum_required(VERSION 3.6)

get_filename_component(src_dir ${CMAKE_CURRENT_LIST_DIR}/src ABSOLUTE)

set(target "mod_cpphello")
function(apxs_deplibs target)
  execute_process(COMMAND apxs -q INCLUDEDIR OUTPUT_VARIABLE apxs_includedir OUTPUT_STRIP_TRAILING_WHITESPACE)
  target_include_directories(${target} PUBLIC ${apxs_includedir} /usr/include/apr-1)
  target_link_libraries(${target} PRIVATE apr-1 aprutil-1)
  execute_process(COMMAND apxs -q CFLAGS OUTPUT_VARIABLE apxs_cflags OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${apxs_cflags}")
endfunction()

file(GLOB src *.cpp)
add_library(${target} SHARED ${src})
set_target_properties(${target} PROPERTIES PREFIX "")
apxs_deplibs(${target})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPR_POOL_DEBUG -g")
