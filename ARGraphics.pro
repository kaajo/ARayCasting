#-------------------------------------------------
#
# Project created by QtCreator 2015-09-27T00:00:50
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ARGraphics
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -pedantic -Wall -Wextra

SOURCES += main.cpp\
        mainwindow.cpp \
    arglscene.cpp \
    armarker.cpp \
    ray.cpp \
    DrawText.cxx \
    DrawScene.cxx \
    targa.cxx \
    SetCamera.cxx \
    SceneContext.cxx \
    SceneCache.cxx \
    GlFunctions.cxx \
    GetPosition.cxx \
    Common.cxx \
    settings.cpp \
    addmarkerdialog.cpp

HEADERS  += mainwindow.h \
    arglscene.h \
    armarker.h \
    ray.h \
    DrawText.h \
    DrawScene.h \
    targa.h \
    SetCamera.h \
    SceneContext.h \
    SceneCache.h \
    GlFunctions.h \
    GetPosition.h \
    Common.h \
    settings.h \
    addmarkerdialog.h

FORMS    += mainwindow.ui \
    arglscene.ui \
    settings.ui \
    addmarkerdialog.ui

INCLUDEPATH += $$(ARTOOLKIT5_ROOT)/include

CONFIG += no_lflags_merge

LIBS += -L$$(ARTOOLKIT5_ROOT)/lib/linux-x86_64 -L$$(ARTOOLKIT5_ROOT)/lib -lARosg -lARgsub_lite -lARvideo -lAR -lARICP -lAR -lARosg\
-losg -lOpenThreads -losgDB -losgFX -losgGA -losgViewer -losgUtil -lstdc++\
-lglut -lGLU -lGL -lX11 -lm -lpthread -ljpeg -pthread -lgstreamer-0.10 \
 -lgmodule-2.0 -lgthread-2.0 -lxml2 -lgobject-2.0 -lglib-2.0 -ldc1394 -lraw1394 \
#fbx
 -lGLEW

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/lib/release/ -lfbxsdk
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/lib/debug/ -lfbxsdk
else:unix: LIBS += -L$$PWD/../../../../../../usr/lib/ -lfbxsdk

INCLUDEPATH += $$PWD/../../../../../../usr/include
DEPENDPATH += $$PWD/../../../../../../usr/include
