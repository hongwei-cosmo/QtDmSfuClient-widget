cmake_minimum_required( VERSION 3.2.0 FATAL_ERROR )
project( QtDmSfuClient )

set( BUILD_SHARED_LIBS True )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
  # Custom source list for Windows Desktop
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

find_package( libwebrtc REQUIRED )
find_package( QtWebrtc  REQUIRED )

if( WIN32 )
  if( BUILD_SHARED_LIBS )
    if( QtWebrtc_EXPORT_LIB )
      set( QtWebrtc_EXPORTLIB ${QtWebrtc_EXPORT_LIB} )
    else()
      message( ERROR "You want a shared build, but I don't have QtWebrtc Export lib." )
    endif()
    if( QtWebrtc_SHARED_LIB )
      set( QtWebrtc_LIBRARIES ${QtWebrtc_SHARED_LIB} )
    else()
      message( ERROR "You want a shared build, but I don't have QtWebrtc dll.")
    endif()
  else()
    if( QtWebrtc_STATIC_LIB )
      set( QtWebrtc_LIBRARIES ${QtWebrtc_STATIC_LIB} )
    else()
      message( ERROR "You want a static build, but I don't have QtWebrtc static lib." )
    endif()
  endif()
else( UNIX )
  message( STATUS ${QtWebrtc_SHARED_LIB} )
  message( STATUS ${QtWebrtc_STATIC_LIB} )
  if( BUILD_SHARED_LIBS )
    if( QtWebrtc_SHARED_LIB )
      set( QtWebrtc_LIBRARIES ${QtWebrtc_SHARED_LIB} )
    else()
      message( ERROR "You want a shared build, but i don't have QtWebrtc shared lib." )
    endif()
  else( )
    if( QtWebrtc_STATIC_LIB )
      set( QtWebrtc_LIBRARIES ${QtWebrtc_STATIC_LIB} )
    else()
      message( ERROR "You want a static build, but i don't have QtWebrtc static lib." )
    endif()
  endif( )
endif()

find_package( Qt5 COMPONENTS Quick Widgets WebSockets REQUIRED )

# NOTE ALEX: this should not be needed, double check on linux
if( UNIX AND NOT APPLE )
  find_package( X11 REQUIRED )
  include_directories( ${X11_INCLUDE_DIR} )
endif()

set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DQT_QML_DEBUG" )
if( NOT WIN32 )
  set( CMAKE_AUTOMOC_MOC_OPTIONS "-f${QtWebrtc_MOC_DEPS}" )
endif()
set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )

add_subdirectory( ${CMAKE_CURRENT_LIST_DIR}/sfulib )

set( sources
 controller.cpp
 mainwindow.cpp
 main.cpp
)
set( headers
 controller.h
 mainwindow.h
)
set( uis
 mainwindow.ui
)
set (resources
 main.qrc
)
add_executable( QtDmSfuClient ${sources} ${headers} ${uis} ${resources} )
target_link_libraries( QtDmSfuClient
  Qt5::Widgets
  Qt5::Quick
  Qt5::WebSockets
  ${WEBRTC_LIBRARIES}
  QtDmSfuClientLib
 )
if( WIN32 )
 target_link_libraries( QtDmSfuClient ${QtWebrtc_EXPORTLIB} )
  # Copy the Qt Wrapper dll
  add_custom_command(
   TARGET QtDmSfuClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${QtWebrtc_SHARED_LIB}"
            $<TARGET_FILE_DIR:QtDmSfuClient>
  )
elseif( UNIX AND NOT APPLE )
  # Linux
  target_link_libraries( QtDmSfuClient
    ${QtWebrtc_STATIC_LIB}
    ${WEBRTC_LIBRARIES}
    ${X11_LIBRARIES}
    ${X11_Xcomposite_LIB}
    ${X11_Xdamage_LIB}
    ${X11_Xfixes_LIB}
  )
elseif( UNIX )
  # Mac
  target_link_libraries( QtDmSfuClient ${QtWebrtc_LIBRARIES} )
else()
  message( WARNING "You should not be here." )
endif()

#-------------------------------------------------------------------------------
# only runtime items, this is not a dev package
#
include( InstallRequiredSystemLibraries )

if( WIN32 AND NOT CMAKE_CROSSCOMPILING AND NOT CMAKE_VERSION VERSION_LESS 3.1 )
  # Handling flags for windeployqt
  # -- handle Debug / Release dlls
  get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)

  get_filename_component (QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
  # --- locate the windeployqt tool
  find_program( QT_WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}" )
  if( CMAKE_BUILD_TYPE STREQUAL Debug )
    list( APPEND JanusApp_WINDEPLOYQT_FLAGS --debug )
  else()
    list( APPEND JanusApp_WINDEPLOYQT_FLAGS --release )
  endif()
  list (APPEND JanusApp_WINDEPLOYQT_FLAGS --no-compiler-runtime)
  list (APPEND JanusApp_WINDEPLOYQT_FLAGS --qmldir ${CMAKE_SOURCE_DIR})

  # Deploying
  add_custom_command(
   TARGET QtDmSfuClient
    COMMAND ${QT_WINDEPLOYQT_EXECUTABLE} ${JanusApp_WINDEPLOYQT_FLAGS} QtDmSfuClient.exe
    )
endif()

enable_testing()
include( CTest )

add_test( TestJanusApp QtDmSfuClient )
