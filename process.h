#ifndef PROCESS_H
#define PROCESS_H

#include <QTime>
#include <QCoreApplication>


class Process
{
public:
    Process();

    static void continueSleep(const int &ms);
};

#endif // PROCESS_H
