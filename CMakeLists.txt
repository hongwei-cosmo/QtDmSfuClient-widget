cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(QtDmSfuClient)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(libwebrtc REQUIRED)
find_package(dm-sfu-signaling REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(sources
  lib/controller.cpp
  lib/dm_sfu_signaling.cpp
  lib/dm_peer_connection.cpp
  lib/dm_video_observer.cpp
  lib/ChecksumMediaCrypto.cpp
  lib/frame_widget.cpp
  ui/mainwindow.cpp
  ui/main.cpp
  ui/dialogconnetsfu.cpp
  ui/dialogcreateroom.cpp
  ui/dialogparticipant.cpp
  ui/dialogseek.cpp
  ui/dialoglimit.cpp
  )
set(forms
  ui/mainwindow.ui
  ui/dialogconnetsfu.ui
  ui/dialogcreateroom.ui
  ui/dialogparticipant.ui
  ui/dialogseek.ui
  ui/dialoglimit.ui
  )
add_executable(QtDmSfuClient MACOSX_BUNDLE ${sources} ${forms})
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
