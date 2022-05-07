#include "mytcpserver.h"

MyTcpServer::MyTcpServer()
{
    //Log::getInstance();
}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "new a connect!";
    qDebug() << "主线程线程号为：" << QThread::currentThreadId() ;
    MyTcpSocket *pTcpSocket=new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);
    connect(pTcpSocket,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteScoket(MyTcpSocket*)));
}

void MyTcpServer::resend(const char *oppname, PDU *pdu)
{
    if(oppname==NULL||pdu==NULL) return ;
    QString strName=oppname;
    for(int i=0;i<m_tcpSocketList.size();i++)
    {
        if(m_tcpSocketList.at(i)->getName()==strName)
        {
            m_tcpSocketList.at(i)->write((char*)pdu,pdu->uiPDULen);
            break;
        }
    }
}

void MyTcpServer::deleteScoket(MyTcpSocket *mysocket)
{
    QList<MyTcpSocket*>::iterator iter=m_tcpSocketList.begin();
    for(;iter!=m_tcpSocketList.end();iter++)
    {
        if(mysocket==*iter)
        {
            delete *iter;
            *iter=NULL;
            m_tcpSocketList.erase(iter);
            break;
        }
    }
    for(int i=0;i<m_tcpSocketList.size();i++)
    {
        qDebug() << m_tcpSocketList.at(i)->getName();
    }
}

