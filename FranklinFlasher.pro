TEMPLATE = app

QT += qml quick
CONFIG += c++11

#DEFINES += NO_INTERNET

SOURCES += main.cpp \
    runguard.cpp \
    fileutils.cpp \
    flashworker.cpp \
    webutils.cpp

HEADERS += runguard.h \
    fileutils.h \
    flashworker.h \
    webutils.h

RESOURCES += qml.qrc \
    res.qrc

win32: RC_FILE = res/program.rc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
