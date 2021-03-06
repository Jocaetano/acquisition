QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = acquisition
TEMPLATE = app

unix {
    LIBS += -ldl
}

include(deps/QsLog/QsLog.pri)

INCLUDEPATH += src deps

SOURCES += \
    src/main.cpp\
    src/mainwindow.cpp \
    src/item.cpp \
    deps/jsoncpp/jsoncpp.cpp \
    src/column.cpp \
    src/filters.cpp \
    src/search.cpp \
    src/items_model.cpp \
    src/flowlayout.cpp \
    src/imagecache.cpp \
    src/logindialog.cpp \
    src/itemsmanager.cpp \
    src/datamanager.cpp \
    deps/sqlite/sqlite3.c \
    src/buyoutmanager.cpp \
    src/util.cpp \
    src/shop.cpp \
    src/tabbuyoutsdialog.cpp

HEADERS += \
    src/item.h \
    deps/jsoncpp/json.h \
    deps/jsoncpp/json-forwards.h \
    src/column.h \
    src/filters.h \
    src/search.h \
    src/mainwindow.h \
    src/items_model.h \
    src/flowlayout.h \
    src/imagecache.h \
    src/logindialog.h \
    src/itemsmanager.h \
    src/datamanager.h \
    deps/sqlite/sqlite3.h \
    src/buyoutmanager.h \
    src/util.h \
    src/shop.h \
    src/tabbuyoutsdialog.h \
    src/version.h

FORMS += \
    forms/mainwindow.ui \
    forms/logindialog.ui \
    forms/tabbuyoutsdialog.ui

CONFIG += c++11

DEPENDPATH *= $${INCLUDEPATH}

RESOURCES += resources.qrc

win32:RC_ICONS += assets/icon.ico
