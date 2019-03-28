#-------------------------------------------------
#
# Project created by QtCreator 2018-05-12T20:18:22
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT = core gui widgets


TARGET = View1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        dialog.cpp \
    checker_vis.cpp \
    boardview.cpp \
    game.cpp

HEADERS += \
    itemlist.h \
    dialog.h \
    game.h \
<<<<<<< HEAD
    checker_vis.h \
    sortlist.h \

=======
    boardview.h \
    checker_vis.h \
    fastlist.h
>>>>>>> parent of f375763... solving some problems

FORMS += \
        dialog.ui

DISTFILES += \
    res/Слой 1.svg \
    res/Слой 2.svg \
    res/Слой 1.svg \
    res/Слой 2.svg

RESOURCES += \
    res.qrc
