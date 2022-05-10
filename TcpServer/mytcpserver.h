#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QDebug>
#include <QList>
#include <QThreadPool>
#include "mytcpsocket.h"
#include "mythreadpool.h"
//#include "log.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();

    static MyTcpServer &getInstance();


    void incomingConnection(qintptr socketDescriptor);

    void resend(const char * oppname,PDU *pdu);
private:
    QList<MyTcpSocket*> m_tcpSocketList;
    MyTcpSocket *m_tcpSock;
signals:
    void sendCfd(qintptr socketDescriptor);
    void resendThread(PDU *pdu);

public slots:
    void deleteScoket(MyTcpSocket *mysocket);

};

#endif // MYTCPSERVER_H
