QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp\
    MainWindow.cpp

HEADERS += \
    MainWindow.h

FORMS     += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    dll.qrc \
    lib.qrc \
    src.qrc

QMAKE_CXXFLAGS+=/execution-charset:utf-8

RC_CODEPAGE = 65001

VERSION = 1.0.2a

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QMAKE_TARGET_PRODUCT = "CSGO ToolBox"
QMAKE_PROJECT_NAME = "CSGO工具箱"

RC_ICONS = logo.ico

SUBDIRS += \

INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib
INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lquazipd
win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libquazip.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libquazipd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/quazip.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/quazipd.lib
