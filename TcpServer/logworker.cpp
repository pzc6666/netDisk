#include "logworker.h"
#include "tcpserver.h"

LogWorker::LogWorker(QObject *parent) : QObject(parent)
{
    t1=new QThread;
    this->moveToThread(t1);
    t1->start();
    connect(this,&LogWorker::sWriteLog,this,&LogWorker::writeLog);
    connect(this,&LogWorker::destroyed,this,[=](){
        t1->quit();
        t1->wait(2000);
        t1->deleteLater();
    });
}

void LogWorker::writeLog()
{
    while(List.size()>0)
    {
        QFile file(filepath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts << List[0] << " id:" << QThread::currentThreadId() << endl;
            mutex.lock();
            List.removeFirst();
            mutex.unlock();
            file.flush();
            file.close();
        }
        else
        {
            qDebug() << "write log unsuceessful!";
        }
    }
}

LogWorker &LogWorker::getInstance()
{
    static LogWorker instance;
    return instance;
}

QStringList LogWorker::getList()
{
    return List;
}

void LogWorker::setList(QStringList list)
{
    List=list;
}

void LogWorker::setFilePath(QString filepath)
{
    this->filepath=filepath;
}

QString LogWorker::getFilePath()
{
    return filepath;
}
