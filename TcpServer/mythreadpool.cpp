#include "mythreadpool.h"

myThreadPool::myThreadPool(QObject *parent) : QObject(parent),QRunnable()
{
    setAutoDelete(true);
}

void myThreadPool::run()
{
    QEventLoop eventLoop;
    MyTcpSocket *pTcpSocket=new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(m_socketDescriptor);
    emit sendTcpSock(pTcpSocket);
    qDebug() << "子线程执行";
    eventLoop.exec();
}

void myThreadPool::recvCfd(qintptr socketDescriptor)
{
    m_socketDescriptor=socketDescriptor;
}
