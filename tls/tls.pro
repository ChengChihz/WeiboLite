TEMPLATE = app
TARGET = weibomm-tls
CONFIG += console release
CONFIG -= app_bundle qt
QMAKE_CFLAGS += -std=c99 -D_POSIX_C_SOURCE=200112L
INCLUDEPATH += ../third_party/mbedtls-3.6.7/include ../third_party/mbedtls-3.6.7/library
SOURCES += tls.c $$files(../third_party/mbedtls-3.6.7/library/*.c)
LIBS += -lpthread
LIBS += -lrt
