#-------------------------------------------------
#
# Project created by QtCreator 2012-10-06T16:37:20
#
#-------------------------------------------------

QT       += core gui xml

TARGET = PlistED
TEMPLATE = app

TRANSLATIONS = translation_ru.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    domparser.cpp \
    dommodel.cpp \
    domitem.cpp \
    comboboxdelegate.cpp \
    lineeditdelegate.cpp \
    editortab.cpp \
    editortabswidget.cpp \
    commands.cpp \
    itemstate.cpp

HEADERS  += mainwindow.h \
    domparser.h \
    dommodel.h \
    domitem.h \
    comboboxdelegate.h \
    lineeditdelegate.h \
    editortab.h \
    editortabswidget.h \
    commands.h \
    itemstate.h

FORMS    += mainwindow.ui \
    editortab.ui

RESOURCES += \
    res.qrc
