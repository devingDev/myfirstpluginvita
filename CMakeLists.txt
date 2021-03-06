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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3 -std=c99 -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")

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
  SceLibc_stub_weak
  SceNet_stub_weak
  SceNetCtl_stub_weak
  SceSysmem_stub_weak
  SceDisplay_stub_weak
  SceSysmodule_stub_weak
  SceIofilemgr_stub_weak
  SceLibKernel_stub_weak
  SceKernelThreadMgr_stub_weak
  SceKernelModulemgr_stub_weak
)

set_target_properties(myfirstpluginvita
  PROPERTIES LINK_FLAGS "-nostdlib"
)

vita_create_self(myfirstpluginvita.suprx myfirstpluginvita
  CONFIG ${CMAKE_SOURCE_DIR}/myfirstpluginvita.yml
  UNSAFE
)
