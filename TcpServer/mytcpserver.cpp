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
    qDebug() << "主线程线程号为：" << QThread::currentThreadId();
    myThreadPool *mythread=new myThreadPool;
    connect(this,&MyTcpServer::sendCfd,mythread,&myThreadPool::recvCfd);
    connect(mythread,&myThreadPool::sendTcpSock,this,[=](MyTcpSocket* mysocket){
        m_tcpSocketList.append(mysocket);
        connect(mysocket,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteScoket(MyTcpSocket*)));
    });
    emit sendCfd(socketDescriptor);
    QThreadPool::globalInstance()->start(mythread);
}

void MyTcpServer::resend(const char *oppname, PDU *pdu)
{
    if(oppname==NULL||pdu==NULL) return ;
    QString strName=oppname;
    qDebug() << "resent 执行 id:" << QThread::currentThreadId();
    for(int i=0;i<m_tcpSocketList.size();i++)
    {
        if(m_tcpSocketList.at(i)->getName()==strName)
        {
            char oppname[32]={'\0'};
            strncpy(oppname,pdu->data+32,32);
            qDebug() << "2----------" << oppname;
            PDU *resendPdu=mkPDU(pdu->uiMsgLen);
            resendPdu->uiMsgType=pdu->uiMsgType;
            strncpy((char*)resendPdu->data,(char*)pdu->data,32);
            strncpy((char*)resendPdu->data+32,(char*)pdu->data+32,32);
            strncpy((char*)resendPdu->caMsg,(char*)pdu->caMsg,pdu->uiMsgLen);
            connect(this,&MyTcpServer::resendThread,m_tcpSocketList.at(i),&MyTcpSocket::resendWrite,Qt::QueuedConnection);
            emit resendThread(resendPdu);
            break;
        }
    }
}

void MyTcpServer::deleteScoket(MyTcpSocket *mysocket)
{
    qDebug() << "delete scoket id:" << QThread::currentThreadId();
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

