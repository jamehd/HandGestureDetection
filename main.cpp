#include "Frame.h"
#include "HandDetection.h"

#ifdef __linux__
#include "gesture_interface.h"
#else
#include "ShareGesture.h"
#endif
#include <iostream>

//int main(int argc, char* argv[])
int main(void)
{
#ifdef __linux__
    // Dbus
    petproject::opencvInterface interface("petproject.opencvInterface",
                                          "/petproject",
                                          QDBusConnection::sessionBus(),
                                          nullptr);
#else
    ShareGesture shareMem;
#endif
    // OpenCV
    auto camera = cv::VideoCapture(0);
#ifdef __linux__
    camera.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
#endif
    camera.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, 960);

    while (true) {
        cv::Mat cap;
        auto res = camera.read(cap);
        if (res)
        {
            Frame fr(cap);
            HandDetection detection(fr);
            detection.initBkg();
            detection.processing();
            detection.drawResult();
#ifdef __linux__
            interface.gesture(int(fr.gesture));
#else
            shareMem.write(int(fr.gesture));
#endif
            auto c = cv::waitKey(20);
            if (char(c) == 27)
            {
                break;
            }
        }
    }

    camera.release();
    cv::destroyAllWindows();
}
