#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QTcpSocket>
#include <QHostAddress>
#include "protocol.h"
#include "operatewidget.h"
//#include "online.h"
//#include "friend.h"
//#include "operatewidget.h"

namespace Ui {
class TcpClient;
}

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = 0);
    ~TcpClient();
    void loadConfig();
    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket();
    QString getLoginName();
    QString curPath();
    void setCurPath(QString str);

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;

    //连接服务器，和服务器数据交互
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;

    QString m_strCurPath;
    QFile m_file;
public slots:
    void showConnect();
    void recvMsg();
private slots:
    void on_login_btn_clicked();
    void on_regist_btn_clicked();
    void on_cancel_btn_clicked();
};

#endif // TCPCLIENT_H
