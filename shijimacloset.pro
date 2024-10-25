#-------------------------------------------------
#
# Project created by QtCreator 2022-12-30T20:52:51
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ShijimaCloset
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        dbinstance.cpp \
        detailview.cpp \
        imagepreviewform.cpp \
        imageview.cpp \
        itemmodel.cpp \
        labelcommit.cpp \
        labeling.cpp \
        login.cpp \
        main.cpp \
        ocrmenu.cpp \
        pagenavigator.cpp \
        qtsinglecoreapplication/qtlocalpeer.cpp \
        qtsinglecoreapplication/qtlockedfile.cpp \
        qtsinglecoreapplication/qtlockedfile_unix.cpp \
        qtsinglecoreapplication/qtlockedfile_win.cpp \
        qtsinglecoreapplication/qtsingleapplication.cpp \
        qtsinglecoreapplication/qtsinglecoreapplication.cpp \
        search.cpp \
        typeeditmenu.cpp \
        welcome.cpp

HEADERS += \
        dbinstance.h \
        detailview.h \
        imagepreviewform.h \
        imageview.h \
        item.h \
        itemmodel.h \
        labelcommit.h \
        labeling.h \
        login.h \
        ocrmenu.h \
        pagenavigator.h \
        qtsinglecoreapplication/QtLockedFile \
        qtsinglecoreapplication/QtSingleApplication \
        qtsinglecoreapplication/qtlocalpeer.h \
        qtsinglecoreapplication/qtlockedfile.h \
        qtsinglecoreapplication/qtsingleapplication.h \
        qtsinglecoreapplication/qtsinglecoreapplication.h \
        search.h \
        typeeditmenu.h \
        welcome.h \
        window.h

FORMS += \
        detailview.ui \
        imagepreviewform.ui \
        labelcommit.ui \
        labeling.ui \
        login.ui \
        ocrmenu.ui \
        pagenavigator.ui \
        search.ui \
        typeeditmenu.ui \
        welcome.ui

RESOURCES += \
    resource.qrc

RC_FILE += logo.rc


DISTFILES += \
    icon.ico \
    logo.rc \
    qtsinglecoreapplication/qtsingleapplication.pri \
    qtsinglecoreapplication/qtsinglecoreapplication.pri
