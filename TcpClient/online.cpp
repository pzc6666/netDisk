#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    ui->listWidget->clear();
    if(NULL==pdu) return;
    uint uiSize=pdu->uiMsgLen/32;
    char charTemp[32];
    for(uint i=0;i<uiSize;i++)
    {
        memcpy(charTemp,(char*)(pdu->caMsg)+i*32,32);
        ui->listWidget->addItem(charTemp);
    }
}

void Online::on_addFriend_btn_clicked()
{
    QString strPerUsrName=ui->listWidget->currentItem()->text();
    QString strLoginUsrName=TcpClient::getInstance().getLoginName();
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->data,strLoginUsrName.toStdString().c_str(),strLoginUsrName.size());
    memcpy(pdu->data+32,strPerUsrName.toStdString().c_str(),strPerUsrName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}
