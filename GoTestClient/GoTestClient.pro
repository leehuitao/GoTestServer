QT       += core gui network concurrent sql

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
    File/file_thread.cpp \
    Network/packet_process.cpp \
    Network/tcp_client.cpp \
    app_cache.cpp \
    emotion_widget.cpp \
    main.cpp \
    mainwindow.cpp \
    message_box_widget.cpp \
    sql/sqlite.cpp

HEADERS += \
    File/file_thread.h \
    Network/packet_define.h \
    Network/packet_process.h \
    Network/tcp_client.h \
    app_cache.h \
    emotion_widget.h \
    mainwindow.h \
    message_box_widget.h \
    sql/db_define.h \
    sql/sqlite.h

FORMS += \
    emotion_widget.ui \
    mainwindow.ui \
    message_box_widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
