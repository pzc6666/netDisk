#include "log.h"

Log::Log()
{
    qInstallMessageHandler(outputMessage);
    qDebug() << "主线程ID：" << QThread::currentThreadId() << endl;
}

QMutex Log::mutex; //分配空间

void Log::outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString qstrText;
    QString msgType;

    switch (type)
    {
    case QtDebugMsg:
        msgType = "Debug:";
        break;
    case QtInfoMsg:
        msgType = "Info:";
        break;
    case QtWarningMsg:
        msgType = "Warning:";
        break;
    case QtCriticalMsg:
        msgType = "Critical:";
        break;
    case QtFatalMsg:
        msgType = "Fatal:";
        break;
    }

    qstrText = QString(msgType + "%1 [%2:%3] %4").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                                .arg(context.file)
                                                .arg(context.line)
                                                .arg(msg);
    QString logFile = QDateTime::currentDateTime().toString("yyyy_MM_dd") + ".log";
    mutex.lock();
    if(LogWorker::getInstance().getFilePath()!=logFile)
    {
        LogWorker::getInstance().setFilePath(logFile);
    }
    QStringList list=LogWorker::getInstance().getList();
    list.append(qstrText);
    LogWorker::getInstance().setList(list);
    if(LogWorker::getInstance().getList().size()==1)
    {
        emit LogWorker::getInstance().sWriteLog();
    }
    mutex.unlock();
}

Log &Log::getInstance()
{
    static Log instance;
    return instance;
}
