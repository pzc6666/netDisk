#ifndef MYTHEADPOOL_H
#define MYTHEADPOOL_H

#include <QObject>
#include <QRunnable>
#include "mytcpsocket.h"

class myThreadPool : public QObject,public QRunnable
{
    Q_OBJECT
public:
    explicit myThreadPool(QObject *parent = 0);
    void run() override;
signals:
    void sendTcpSock(MyTcpSocket* mysocket);

public slots:
    void recvCfd(qintptr socketDescriptor);

private:
    qintptr m_socketDescriptor;
};

#endif // MYTHEADPOOL_H
