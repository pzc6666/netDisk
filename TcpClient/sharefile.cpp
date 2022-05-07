#include "sharefile.h"
#include "tcpclient.h"
#include "operatewidget.h"

ShareFile::ShareFile(QWidget *parent) : QWidget(parent)
{
    m_pSelectAllPB = new QPushButton("全选");
    m_pCancelSelectPB = new QPushButton("取消选择");

    m_pOKPB = new QPushButton("确定");
    m_pCancelPB = new QPushButton("取消");

    m_pSA = new QScrollArea;
    m_pFriendW = new QWidget;
    //m_pSA->setWidget(m_pFriendW);
    m_pFriendWVBL = new QVBoxLayout(m_pFriendW);
    m_pButtonGroup = new QButtonGroup(m_pFriendW);
    m_pButtonGroup->setExclusive(false);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancelSelectPB);
    pTopHBL->addStretch();

    QHBoxLayout *pDownHBL = new QHBoxLayout;
    pDownHBL->addWidget(m_pOKPB);
    pDownHBL->addWidget(m_pCancelPB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pSA);
    pMainVBL->addLayout(pDownHBL);
    setLayout(pMainVBL);

    connect(m_pSelectAllPB,SIGNAL(clicked(bool)),this,SLOT(selectAll()));
    connect(m_pCancelSelectPB,SIGNAL(clicked(bool)),this,SLOT(cancelSelect()));
    connect(m_pOKPB,SIGNAL(clicked(bool)),this,SLOT(okSahre()));
    connect(m_pCancelPB,SIGNAL(clicked(bool)),this,SLOT(cancelShare()));
}

ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;
}

void ShareFile::updateFriend(QStringList onlineFriend)
{
    if (0 == onlineFriend.size())
    {
        return;
    }
    QCheckBox *pCB = NULL;
    for (int i = 0; i<10; i++)
    {
        pCB = new QCheckBox("zhichen");
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);

    QAbstractButton *tmp = NULL;
    QList<QAbstractButton*> preFriendList = m_pButtonGroup->buttons();
    while(preFriendList.size()>0)
    {
        tmp = preFriendList[0];
        m_pFriendWVBL->removeWidget(tmp);
        m_pButtonGroup->removeButton(tmp);
        preFriendList.removeOne(tmp);
        delete tmp;
        tmp = NULL;
    }

    for (int i = 0; i<onlineFriend.size(); i++)
    {
        pCB = new QCheckBox(onlineFriend[i]);
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
        m_pFriendW->setFixedSize(100, 50 + i * 25);
    }
    m_pSA->setWidget(m_pFriendW);
}

void ShareFile::test(int count)
{
    QAbstractButton *tmp = NULL;
    QList<QAbstractButton*> preFriendList = m_pButtonGroup->buttons();
    while(preFriendList.size()>0)
    {
        tmp = preFriendList[0];
        m_pFriendWVBL->removeWidget(tmp);
        m_pButtonGroup->removeButton(tmp);
        preFriendList.removeOne(tmp);
        delete tmp;
        tmp = NULL;
    }
    QCheckBox *pCB = NULL;
    for (int i = 0; i<count; i++)
    {
        pCB = new QCheckBox("zhichen");
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
        m_pFriendW->setFixedSize(100, 50 + i * 25);
    }
    m_pSA->setWidget(m_pFriendW);
    // m_pFriendW->setLayout(m_pFriendWVBL);
}

void ShareFile::cancelSelect()
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for (int i=0; i<cbList.size(); i++)
    {
        if (cbList[i]->isChecked())
        {
            cbList[i]->setChecked(false);
        }
    }
}

void ShareFile::selectAll()
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for (int i=0; i<cbList.size(); i++)
    {
        if (!cbList[i]->isChecked())
        {
            cbList[i]->setChecked(true);
        }
    }
}

void ShareFile::okSahre()
{
    QString strName = TcpClient::getInstance().getLoginName();
    QString strCurPath = TcpClient::getInstance().curPath();
    QString strShareFileName = OperateWidget::getInstance().getBook()->getShareFileName();
    QString strFilePath=strCurPath+'/'+strShareFileName;
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    int num = 0;
    for (int i=0; i<cbList.size(); i++)
    {
        if (cbList[i]->isChecked())
        {
            num++;
        }
    }
    PDU *pdu = mkPDU(32*num+strFilePath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->data, "%s %d", strName.toStdString().c_str(), num);
    int j = 0;
    for (int i=0; i<cbList.size(); i++)
    {
        if (cbList[i]->isChecked())
        {
            memcpy((char*)(pdu->caMsg)+j*32, cbList[i]->text().toStdString().c_str(), cbList[i]->text().size());
            j++;
        }
    }

    memcpy((char*)(pdu->caMsg)+num*32, strFilePath.toStdString().c_str(), strFilePath.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void ShareFile::cancelShare()
{
    hide();
}
