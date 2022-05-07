#ifndef LOG_H
#define LOG_H

#include <QtDebug>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include <QString>
#include <qapplication.h>
#include "logworker.h"

class Log
{
public:
    Log();
    static void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static Log& getInstance();

private:
    static QMutex mutex;
};

#endif // LOG_H
