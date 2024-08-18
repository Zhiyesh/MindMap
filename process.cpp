#include "process.h"

Process::Process()
{

}

void Process::continueSleep(const int &ms)
{
    QTime targetTime = QTime::currentTime().addMSecs(ms);
    while (targetTime > QTime::currentTime())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
}

