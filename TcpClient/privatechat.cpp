#include "privatechat.h"
#include "ui_privatechat.h"
#include "tcpclient.h"

PrivateChat::PrivateChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
    this->setWindowTitle("私聊");
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::setName(QString privateName)
{
    m_strPrivateName=privateName;
    m_strLoginName=TcpClient::getInstance().getLoginName();
}

void PrivateChat::updateMsg(PDU *pdu)
{
    if(pdu==NULL) return ;
    char caSendName[32] = {'\0'};
    memcpy(caSendName, pdu->data, 32);
    QDateTime timeCurrent = QDateTime::currentDateTime();
    QString time = timeCurrent.toString("hh:mm");
    QString strMsg = QString("(%1)%2 says: %3").arg(time).arg(caSendName).arg((char*)(pdu->caMsg));
    ui->msg_show->append(strMsg);
}

void PrivateChat::myMsgShow(PDU *pdu)
{
    qDebug() << (char*)pdu->caMsg;
    QDateTime timeCurrent = QDateTime::currentDateTime();
    QString time = timeCurrent.toString("hh:mm");
    QString strMsg = QString("(%1)my says: %2").arg(time).arg((char*)pdu->caMsg);
    qDebug() << strMsg;
    ui->msg_show->append(strMsg);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

void PrivateChat::on_send_btn_clicked()
{
    QString send_msg=ui->input_msg->text();
    ui->input_msg->clear();
    if(!send_msg.isEmpty())
    {
        uint msg_size=send_msg.size();
        PDU *pdu=mkPDU(msg_size+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_PRIVATECHAT_FRIEND_REQUEST;
        memcpy(pdu->data,m_strLoginName.toStdString().c_str(),32);
        memcpy(pdu->data+32,m_strPrivateName.toStdString().c_str(),32);
        strcpy((char*)pdu->caMsg,send_msg.toStdString().c_str());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::warning(this,"私聊好友","发送信息不能为空！");
    }
}
