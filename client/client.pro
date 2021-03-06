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
    client_socket.cpp \
    ftp_command_builder.cpp \
    ftp_file.cpp \
    ftp_list_parser.cpp \
    test_main.cpp \
    ftp_file_parser.cpp \
    ftp_client.cpp \
    ftp_response_parser.cpp \
    packet.cpp \
    bernoulli_trial.cpp

HEADERS  += mainwindow.h \
    gui_renderer.h \
    gui_builder.h \
    http_get_request_builder.h \
    http_response_parser.h \
    client_socket.h \
    ftp_command_builder.h \
    ftp_file.h \
    ftp_file_parser.h \
    ftp_list_parser.h \
    ftp_client.h \
    ftp_response_parser.h \
    packet.h \
    bernoulli_trial.h

FORMS    += mainwindow.ui










