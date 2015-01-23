#-------------------------------------------------
#
# Project created by QtCreator 2014-01-06T17:37:09
#
#-------------------------------------------------

QT       += core gui xml opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fvisualizer
TEMPLATE = app

LIBS += -lGLU

SOURCES += main.cpp\
        fvmainwindow.cpp \
    plotter3d.cpp \
    plotter2d.cpp \
    Operator.cpp \
    Node.cpp \
    helpdialog.cpp \
    helpbrowser.cpp \
    graphwindow3d.cpp \
    graphwindow2d.cpp \
    graphwindow.cpp \
    graphsettings3ddialog.cpp \
    graphsettings2ddialog.cpp \
    fvformathandler.cpp \
    functionslistview.cpp \
    function3ddialog.cpp \
    function3d.cpp \
    function2ddialog.cpp \
    function2d.cpp \
    function.cpp \
    ExpressionEvaluator.cpp \
    derivativedialog.cpp \
    commands.cpp \
    colorbutton.cpp \
    ArcBall.cpp

HEADERS  += fvmainwindow.h \
    ArcBall.h \
    plotter3d.h \
    plotter2d.h \
    Operator.h \
    Node.h \
    helpdialog.h \
    helpbrowser.h \
    graphwindow3d.h \
    graphwindow2d.h \
    graphwindow.h \
    graphsettings3ddialog.h \
    graphsettings2ddialog.h \
    fvformathandler.h \
    functionslistview.h \
    function3ddialog.h \
    function3d.h \
    function2ddialog.h \
    function2d.h \
    function.h \
    ExpressionEvaluator.h \
    derivativedialog.h \
    commands.h \
    colorbutton.h

RESOURCES += \
    resources.qrc

FORMS += \
    helpdialog.ui \
    graphsettings3ddialog.ui \
    graphsettings2ddialog.ui \
    function3ddialog.ui \
    function2ddialog.ui \
    derivativedialog.ui

OTHER_FILES += \
    splash.psd \
    icon.psd \
    FVisualizer.vcxproj.user \
    FVisualizer.vcxproj.filters \
    FVisualizer.vcxproj \
    fvisualizer.pro.user \
    fv.rc \
    fv.ico \
    fv.aps
