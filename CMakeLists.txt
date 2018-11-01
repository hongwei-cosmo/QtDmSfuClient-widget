cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(QtDmSfuClient)

set(BUILD_SHARED_LIBS True)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#if(WIN32)
#  # Custom source list for Windows Desktop
#  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
#  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
#endif()

find_package(libwebrtc REQUIRED)
find_package(QtWebrtc  REQUIRED)
find_package(dm-sfu-signaling REQUIRED)
find_package(Qt5 COMPONENTS Quick Widgets WebSockets REQUIRED)

if(WIN32)
  if(BUILD_SHARED_LIBS)
    if(QtWebrtc_EXPORT_LIB)
      set(QtWebrtc_LIBRARIES ${QtWebrtc_EXPORT_LIB})
    else()
      message(ERROR "You want a shared build, but I don't have QtWebrtc Export lib.")
    endif()
  else()
    if(QtWebrtc_STATIC_LIB)
      set(QtWebrtc_LIBRARIES ${QtWebrtc_STATIC_LIB})
    else()
      message(ERROR "You want a static build, but I don't have QtWebrtc static lib.")
    endif()
  endif()
else(UNIX)
  message(STATUS ${QtWebrtc_SHARED_LIB})
  message(STATUS ${QtWebrtc_STATIC_LIB})
  if(BUILD_SHARED_LIBS)
    if(QtWebrtc_SHARED_LIB)
      set(QtWebrtc_LIBRARIES ${QtWebrtc_SHARED_LIB})
    else()
      message(ERROR "You want a shared build, but i don't have QtWebrtc shared lib.")
    endif()
  else()
    if(QtWebrtc_STATIC_LIB)
      set(QtWebrtc_LIBRARIES ${QtWebrtc_STATIC_LIB})
    else()
      message(ERROR "You want a static build, but i don't have QtWebrtc static lib.")
    endif()
  endif()
endif()

set(sources
  ui/mainwindow.cpp
  ui/main.cpp
  ui/dialogconnetsfu.cpp
  ui/dialogcreateroom.cpp
  ui/dialogparticipant.cpp
  ui/dialogseek.cpp
  ui/dialoglimit.cpp
  lib/controller.cpp
  lib/qt_sfu_signaling.cpp
  )
set(headers
  ui/mainwindow.h
  ui/dialogconnetsfu.h
  ui/dialogcreateroom.h
  ui/dialogparticipant.h
  ui/dialogseek.h
  ui/dialoglimit.h
  lib/controller.h
  lib/qt_sfu_signaling.h
  )
set(uis
  ui/mainwindow.ui
  ui/dialogconnetsfu.ui
  ui/dialogcreateroom.ui
  ui/dialogparticipant.ui
  ui/dialogseek.ui
  ui/dialoglimit.ui
  )
set(resources
  ui/main.qrc
  )
add_executable(QtDmSfuClient MACOSX_BUNDLE ${sources} ${headers} ${uis} ${resources})
if(APPLE)
  set_target_properties(QtDmSfuClient PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/ui/Info.plist"
  )
endif()
target_compile_definitions(QtDmSfuClient PUBLIC QT_NO_KEYWORDS)
target_link_libraries(QtDmSfuClient
  Qt5::Widgets
  Qt5::Quick
  Qt5::WebSockets
  ${WEBRTC_LIBRARIES}
  ${QtWebrtc_LIBRARIES}
  dm-sfu-signaling::dm-sfu-signaling
  )

