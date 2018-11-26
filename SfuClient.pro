#-------------------------------------------------
#
# Project created by QtCreator 2018-11-15T13:01:24
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SfuClient
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += _WEBSOCKETPP_CPP11_STL_
DEFINES += ASIO_STANDALONE
DEFINES += JSON_USE_EXCEPTION=0
DEFINES += UNICODE
DEFINES += _UNICODE
DEFINES += WEBRTC_WIN
DEFINES += NOMINMAX
DEFINES += _CRT_SECURE_NO_WARNINGS
DEFINES += WIN32_LEAN_AND_MEAN

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14
CONFIG += no_keywords

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = stable.h

SOURCES += \
        lib/controller.cpp \
        lib/dm_sfu_signaling.cpp \
        lib/dm_peer_connection.cpp \
        lib/dm_video_observer.cpp \
        lib/ChecksumMediaCrypto.cpp \
        lib/frame_widget.cpp \
        ui/main.cpp \
        ui/mainwindow.cpp \
        ui/dialogconnetsfu.cpp \
        ui/dialogcreateroom.cpp \
        ui/dialoglimit.cpp \
        ui/dialogparticipant.cpp \
        ui/dialogseek.cpp \
        ui/main.cpp \
        ui/mainwindow.cpp

HEADERS += \
        lib/controller.h \
        lib/dm_sfu_signaling.h \
        lib/dm_peer_connection.h \
        lib/dm_video_observer.h \
        lib/ChecksumMediaCrypto.h \
        lib/frame_widget.h \
        ui/mainwindow.h \
        ui/dialogconnetsfu.h \
        ui/dialogcreateroom.h \
        ui/dialoglimit.h \
        ui/dialogparticipant.h \
        ui/dialogseek.h \
        ui/mainwindow.h \
    stable.h

FORMS += \
        ui/mainwindow.ui \
        ui/dialogconnetsfu.ui \
        ui/dialogcreateroom.ui \
        ui/dialoglimit.ui \
        ui/dialogparticipant.ui \
        ui/dialogseek.ui \
        ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
  COSMOROOT=C:/cosmo/
} else {
  COSMOROOT=$$(HOME)/cosmo/
}
LIBS += -L$$COSMOROOT/libwebrtc/lib/ -lwebrtc -llibssl -llibcrypto -lsrtp2 -lrtc_json -ljsoncpp -lSecur32 -lMsdmo -lDmoguids -lStrmiids -lWinmm -lwmcodecdspuuid -lWs2_32 -lcrypt32
win32 {
  LIBS += -ld3d11
  LIBS += -lwindowscodecs
  LIBS += -lDXGI
}
LIBS += -L$$COSMOROOT/dm-sfu-signaling/lib/ -ldm-sfu-signaling -lsfu-sdp -ltransaction-manager

INCLUDEPATH += $$COSMOROOT/libwebrtc/include
INCLUDEPATH += $$COSMOROOT/libwebrtc/include/third_party
INCLUDEPATH += $$COSMOROOT/dm-sfu-signaling/include
INCLUDEPATH += $$PWD/external/asio/asio/include
INCLUDEPATH += $$PWD/external/websocketpp
DEPENDPATH += $$COSMOROOT/libwebrtc/include
DEPENDPATH += $$COSMOROOT/libwebrtc/include/third_party
DEPENDPATH += $$COSMOROOT/dm-sfu-signaling/include
DEPENDPATH += $$PWD/external/asio/asio/include
DEPENDPATH += $$PWD/external/websocketpp

macx {
    QMAKE_INFO_PLIST = ui/Info.plist
}
