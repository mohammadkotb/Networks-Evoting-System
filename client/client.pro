#-------------------------------------------------
#
# Project created by QtCreator 2011-11-06T21:58:00
#
#-------------------------------------------------

QT       += core gui webkit

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gui_renderer.cpp \
    gui_builder.cpp \
    http_get_request_builder.cpp \
    http_response_parser.cpp \
    client_socket.cpp

HEADERS  += mainwindow.h \
    gui_renderer.h \
    gui_builder.h \
    http_get_request_builder.h \
    http_response_parser.h \
    client_socket.h

FORMS    += mainwindow.ui










