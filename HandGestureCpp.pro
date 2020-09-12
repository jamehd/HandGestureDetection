TEMPLATE = app
QT += core
CONFIG += console c++11


SOURCES += \
        ShareGesture.cpp \
        main.cpp \
        Frame.cpp \
        HandDetection.cpp

HEADERS += \
        Frame.h \
        HandDetection.h \
        ShareGesture.h

DISTFILES += \
        gesture.xml

unix {
    INCLUDEPATH += /usr/local/include/opencv4
    LIBS += /usr/local/lib/libopencv*
    DBUS_INTERFACES += gesture.xml
} else {
    LIBS += -LD:/3thparty/opencv/x64/mingw/lib/ -llibopencv_core420.dll
    LIBS += -LD:/3thparty/opencv/x64/mingw/lib/ -llibopencv_highgui420.dll
    LIBS += -LD:/3thparty/opencv/x64/mingw/lib/ -llibopencv_videoio420.dll
    LIBS += -LD:/3thparty/opencv/x64/mingw/lib/ -llibopencv_imgproc420.dll

    INCLUDEPATH += D:/3thparty/opencv/include
    DEPENDPATH += D:/3thparty/opencv/include
}

