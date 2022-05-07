#include "friend.h"
#include "tcpclient.h"

Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pShowMsgTE=new QTextEdit;
    m_pFriendListWidget=new QListWidget;
    m_pInputMsgLE=new QLineEdit;

    m_pDelFriendPB=new QPushButton("删除好友");
    m_pFlushFriendPB=new QPushButton("刷新好友");
    m_pShowOnlineUsrPB=new QPushButton("显示在线用户");
    m_pSearchUsrPB=new QPushButton("查找用户");
    m_pMsgSendPB=new  QPushButton("群发消息");
    m_pPrivateChatPB=new QPushButton("私聊");

    QVBoxLayout *pRightPBVBL=new QVBoxLayout;
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL=new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL=new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline=new Online;

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);
    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool)),this,SLOT(showOnline()));
    connect(m_pSearchUsrPB,SIGNAL(clicked(bool)),this,SLOT(searchUser()));
    connect(m_pFlushFriendPB,SIGNAL(clicked(bool)),this,SLOT(flushFriend()));
    connect(m_pDelFriendPB,SIGNAL(clicked(bool)),this,SLOT(deleteFriend()));
    connect(m_pPrivateChatPB,SIGNAL(clicked(bool)),this,SLOT(privateChat()));
    connect(m_pMsgSendPB,SIGNAL(clicked(bool)),this,SLOT(massTexting()));
}

void Friend::showAllOnlineUser(PDU *pdu)
{
    if(NULL==pdu)
    {
        return ;
    }
    m_pOnline->showUsr(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    m_pFriendListWidget->clear();
    uint uiSize = pdu->uiMsgLen/32;
    char msg[32] = {'\0'};
    for (uint i=0; i<uiSize; i++)
    {
        memcpy(msg, (char*)(pdu->caMsg)+i*32, 32);
        m_pFriendListWidget->addItem(msg);
    }
}

void Friend::showMyMsg(PDU *pdu)
{
    QString strMsg = QString("my says: %1").arg((char*)pdu->caMsg);
    m_pShowMsgTE->append(strMsg);
}

void Friend::showMassMsg(PDU *pdu)
{
    if(pdu==NULL) return ;
    char caSendName[32] = {'\0'};
    memcpy(caSendName, pdu->data, 32);
    QString strMsg = QString("%1 says: %2").arg(caSendName).arg((char*)(pdu->caMsg));
    m_pShowMsgTE->append(strMsg);
}

void Friend::acceptOnlineFriend(PDU *pdu)
{
    m_onlineFriend.clear();
    uint uiSize = pdu->uiMsgLen/32;
    char msg[32] = {'\0'};
    for (uint i=0; i<uiSize; i++)
    {
        memcpy(msg, (char*)(pdu->caMsg)+i*32, 32);
        m_onlineFriend.append(QString(msg));
    }
    OperateWidget::getInstance().getBook()->ShareFile(m_onlineFriend);
}

QStringList Friend::getOnlineFriend()
{
    return m_onlineFriend;
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden())
    {
        m_pOnline->show();
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        m_pOnline->hide();
    }
}

void Friend::searchUser()
{
    m_strname=QInputDialog::getText(this,"搜索","用户名：");
    if(!m_strname.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USER_REQUEST;
        memcpy(pdu->data,m_strname.toStdString().c_str(),m_strname.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Friend::flushFriend()
{
    QString strLoginUsrName=TcpClient::getInstance().getLoginName();
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->data,strLoginUsrName.toStdString().c_str(),strLoginUsrName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::deleteFriend()
{
    if(NULL!=m_pFriendListWidget->currentItem())
    {
        QString strLoginUsrName=TcpClient::getInstance().getLoginName();
        QString data=m_pFriendListWidget->currentItem()->text();
        QStringList list=data.split(" ");
        QString delName=list[0];
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
        memcpy(pdu->data,strLoginUsrName.toStdString().c_str(),strLoginUsrName.size());
        memcpy(pdu->data+32,delName.toStdString().c_str(),delName.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Friend::privateChat()
{
    if(NULL!=m_pFriendListWidget->currentItem())
    {
        QString data=m_pFriendListWidget->currentItem()->text();
        QStringList list=data.split(" ");
        QString chatName=list[0];
        PrivateChat::getInstance().setName(chatName);
        if(PrivateChat::getInstance().isHidden())
        {
            PrivateChat::getInstance().show();
        }
    }
    else
    {
        QMessageBox::warning(this,"私聊","未选择私聊的好友！");
    }
}

void Friend::massTexting()
{
    if(!m_pInputMsgLE->text().isEmpty())
    {
        QString send_msg=m_pInputMsgLE->text();
        m_pInputMsgLE->clear();
        PDU *pdu=mkPDU(send_msg.size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_MASSTEXT_FRIEND_REQUEST;
        strncpy((char*)pdu->data,TcpClient::getInstance().getLoginName().toStdString().c_str(),32);
        strcpy((char*)pdu->caMsg,send_msg.toStdString().c_str());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::warning(this,"群发消息","输入信息为空！");
    }
}
