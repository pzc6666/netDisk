#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QThread>
#include <QEventLoop>
#include "protocol.h"
#include "opendb.h"
//#include "log.h"

class MyTcpSocket :public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();
    void copyDir(QString strSrcDir, QString strDestDir);
    void recvMesg();
signals:
    void offline(MyTcpSocket *mysocket);
public slots:
    void clientOffline();
    void sendFileToClient();
    void recvTcp(MyTcpSocket *mysocket);
private:
    QString m_strName;
    QFile m_file;

    qint64 m_iTotal;
    qint64 m_iRecved;
    bool m_bUpload;

    QTimer *m_timer;
    MyTcpSocket *tcp;
};

#endif // MYTCPSOCKET_H
