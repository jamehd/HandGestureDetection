#include "ShareGesture.h"
#include <QDebug>
#include <QBuffer>
#include <QDataStream>

ShareGesture::ShareGesture()
    : sharedMemory_("GESTURE_DETECTOR")
{
    if (sharedMemory_.isAttached()) {
        qDebug() << "Unable to detach from shared memory.";
    } else {
        int defaultGesture = 0;
        QBuffer buffer;
        buffer.open(QBuffer::ReadWrite);
        QDataStream out(&buffer);
        out << defaultGesture;
        int size = buffer.size();
        if (!sharedMemory_.create(size)) {
            qDebug() << "Unable to create shared memory segment.";
        } else {
            write(defaultGesture);
        }
    }
}

void ShareGesture::write(const int gesture)
{
    qDebug() << "write: " << gesture;
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QDataStream out(&buffer);
    out << gesture;
    int size = buffer.size();

    sharedMemory_.lock();
    char *to = (char*)sharedMemory_.data();
    const char *from = buffer.data().data();
    memcpy(to, from, qMin(sharedMemory_.size(), size));
    sharedMemory_.unlock();
}

int ShareGesture::read()
{
//    qDebug() << "read: ";
    int gesture = 0;
    if (!sharedMemory_.attach()) {
//        qDebug() << "Unable to attach to shared memory segment.";
    } else {
        QBuffer buffer;
        QDataStream in(&buffer);
        sharedMemory_.lock();
        buffer.setData((char*)sharedMemory_.constData(), sharedMemory_.size());
        buffer.open(QBuffer::ReadOnly);
        in >> gesture;
        sharedMemory_.unlock();
        if (!sharedMemory_.detach()) {
            qDebug() << "Unable to detach from shared memory.";
            gesture = 0;
        }
    }
    return gesture;
}
