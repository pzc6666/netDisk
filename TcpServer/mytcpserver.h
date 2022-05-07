#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QDebug>
#include <QList>
#include "mytcpsocket.h"
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
signals:
    void sendTcp(MyTcpSocket* pTcpSocket);

public slots:
    void deleteScoket(MyTcpSocket *mysocket);

};

#endif // MYTCPSERVER_H
