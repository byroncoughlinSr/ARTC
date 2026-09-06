#-------------------------------------------------
#
# Project created by QtCreator 2018-02-17T06:34:24
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ARTC
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
        mainwindow.cpp \
    hostdlg.cpp \
    databasehelper.cpp \
    person.cpp \
    pedigree.cpp \
    uitheme.cpp \
    homewidget.cpp \
    signinwidget.cpp \
    registerwidget.cpp \
    passwordhasher.cpp \
    accountrepository.cpp \
    familytree.cpp \
    pedigreeview.cpp

HEADERS += \
        mainwindow.h \
    hostdlg.h \
    databasehelper.h \
    person.h \
    pedigree.h \
    uitheme.h \
    homewidget.h \
    signinwidget.h \
    registerwidget.h \
    passwordhasher.h \
    accountrepository.h \
    familytree.h \
    pedigreeview.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    resources.qrc

# Argon2id password hashing. OpenSSL 3.2 and later ship it as a KDF in the
# default provider, so no separate Argon2 library is needed.
LIBS += -lcrypto

