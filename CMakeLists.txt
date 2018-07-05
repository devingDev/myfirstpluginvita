cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(MyFirstPluginVita)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3 -std=gnu11 -fpermissive")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")

include_directories(
)

link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

if (NOT ${RELEASE})
  add_definitions(-DENABLE_LOGGING)
endif()

add_executable(myfirstpluginvita
  main.c
  log.c
)

target_link_libraries(myfirstpluginvita
  taihen_stub
#  k
  SceLibKernel_stub 
  SceIofilemgr_stub
  SceSysmodule_stub
  SceNet_stub
  SceNetCtl_stub
  gcc
  ScePower_stub
  SceKernelModulemgr_stub
)

set_target_properties(myfirstpluginvita
  PROPERTIES LINK_FLAGS "-nostdlib"
)

vita_create_self(myfirstpluginvita.suprx myfirstpluginvita
  UNSAFE
  CONFIG ${CMAKE_SOURCE_DIR}/myfirstpluginvita.yml
)
