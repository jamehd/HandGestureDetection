#ifndef SHAREGESTURE_H
#define SHAREGESTURE_H

#include <QSharedMemory>

class ShareGesture
{
public:
    ShareGesture();
    void write(const int gesture);
    int read();

private:
    QSharedMemory sharedMemory_;
};

#endif // SHAREGESTURE_H
