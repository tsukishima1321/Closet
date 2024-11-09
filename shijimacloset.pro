#-------------------------------------------------
#
# Project created by QtCreator 2022-12-30T20:52:51
#
#-------------------------------------------------

QT       += core gui
QT       += sql
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
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES -= QT_NO_DEBUG_OUTPUT

CONFIG+=c++2a

CONFIG += file_copies
COPIES += pyScript
pyScript.files = $$files(doOcr.py)
pyScript.path = $$OUT_PWD
pyScript.base = $$PWD

INCLUDEPATH += \
        $$PWD/xcombobox \
        $$PWD/pagenavigator \

include     ($$PWD/xcombobox/xcombobox.pri)
include     ($$PWD/pagenavigator/pagenavigator.pri)

SOURCES += \
        dbinstance.cpp \
        detailview.cpp \
        iconresources.cpp \
        imgpreviewform.cpp \
        imageview.cpp \
        itemmodel.cpp \
        labelcommit.cpp \
        labeling.cpp \
        login.cpp \
        main.cpp \
        ocrmenu.cpp \
        imgsearch.cpp \
        typeeditmenu.cpp \
        welcome.cpp \
        textsearch.cpp \
        textpreviewform.cpp \
        textdetailview.cpp \

HEADERS += \
        dbinstance.h \
        detailview.h \
        iconresources.h \
        imgpreviewform.h \
        imageview.h \
        item.h \
        itemmodel.h \
        labelcommit.h \
        labeling.h \
        login.h \
        ocrmenu.h \
        imgsearch.h \
        typeeditmenu.h \
        welcome.h \
        window.h \
        textsearch.h \
        textpreviewform.h \
        textdetailview.h \

FORMS += \
        detailview.ui \
        imgpreviewform.ui \
        labelcommit.ui \
        labeling.ui \
        login.ui \
        ocrmenu.ui \
        imgsearch.ui \
        typeeditmenu.ui \
        welcome.ui \
        textsearch.ui \
        textpreviewform.ui \
        textdetailview.ui \

RESOURCES += \
    resource.qrc

RC_FILE += logo.rc


DISTFILES += \
    doOcr.py \
    icon.ico \
    logo.rc \
