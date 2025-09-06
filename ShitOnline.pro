QT       += core gui network gui-private

win32 {
    QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addnewappdialog.cpp \
    appitem.cpp \
    configmanager.cpp \
    downloadframe.cpp \
    httpapiclient.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    addnewappdialog.h \
    appitem.h \
    configmanager.h \
    downloadframe.h \
    httpapiclient.h \
    widget.h

FORMS += \
    addnewappdialog.ui \
    appitem.ui \
    downloadframe.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    LICENSE \
    README.md

RC_ICONS = icon.ico

RESOURCES += \
    res.qrc
