#include "tcpserver.h"
#include "ui_tcpserver.h"

TcpServer::TcpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpServer)
{
    //Log::getInstance();
    ui->setupUi(this);
    this->loadConfig();
    bool ret=MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
    if(!ret)
    {
        qDebug() << "监听失败！";
    }
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray byteData=file.readAll();
        QString strData=byteData.toStdString().c_str();
        file.close();

        strData.replace("\r\n"," ");
        QStringList strList=strData.split(" ");
//        for(int i=0;i<strList.size();i++)
//        {
//            qDebug() << strList[i];
//        }
        m_strIP=strList.at(0);
        m_usPort=strList.at(1).toUShort();
        qDebug() << "ip:" << m_strIP;
        qDebug() << "port:" << m_usPort;
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed!");
    }
}

TcpServer::~TcpServer()
{
    delete ui;
}
