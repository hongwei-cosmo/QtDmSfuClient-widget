cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(QtDmSfuClient)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(libwebrtc REQUIRED)
find_package(dm-sfu-signaling REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(lib
  lib/controller.cpp
  lib/qt_sfu_signaling.cpp
  )
set(ui
  ui/mainwindow.cpp
  ui/main.cpp
  ui/dialogconnetsfu.cpp
  ui/dialogcreateroom.cpp
  ui/dialogparticipant.cpp
  ui/dialogseek.cpp
  ui/dialoglimit.cpp
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
add_executable(QtDmSfuClient MACOSX_BUNDLE ${lib} ${ui} ${resources})
if(APPLE)
  set_target_properties(QtDmSfuClient PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/ui/Info.plist"
  )
endif()
target_compile_definitions(QtDmSfuClient PUBLIC
  QT_NO_KEYWORDS
  _WEBSOCKETPP_CPP11_STL_
  ASIO_STANDALONE
  )
target_link_libraries(QtDmSfuClient
  Qt5::Widgets
  ${WEBRTC_LIBRARIES}
  dm-sfu-signaling::dm-sfu-signaling
  )

target_include_directories(QtDmSfuClient PUBLIC
  external/websocketpp
  external/asio/asio/include
  )
