#include "tcpclient.h"
#include "ui_tcpclient.h"

TcpClient::TcpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    this->loadConfig();
    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
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

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::getLoginName()
{
    return m_strLoginName;
}

QString TcpClient::curPath()
{
    return m_strCurPath;
}

void TcpClient::setCurPath(QString str)
{
    m_strCurPath=str;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功！！");
}

void TcpClient::recvMsg()
{
    if(!OperateWidget::getInstance().getBook()->getDownloadStatus())
    {
        qDebug() << m_tcpSocket.bytesAvailable();
        uint uiPDULen=0;
        m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));
        //qDebug() << uiPDULen ;
        uint uiMsgLen=uiPDULen-sizeof(PDU);
        PDU *pdu=mkPDU(uiMsgLen);
        m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
        //qDebug() << pdu->uiMsgType << " " << (char *)pdu->caMsg;
        //qDebug() << name << password << pdu->uiMsgType ;
        switch(pdu->uiMsgType)
        {
        case ENUM_MSG_TYPE_REGIST_RESPOND:
        {
             if(0==strcmp(pdu->data,REGIST_OK))
             {
                 QMessageBox::information(this,"注册","注册成功！");
             }
             else if(0==strcmp(pdu->data,REGIST_FAILED))
             {
                 QMessageBox::warning(this,"注册","注册失败！");
             }
             break;
        }
        case ENUM_MSG_TYPE_LOGIN_RESPOND:
        {
             if(0==strcmp(pdu->data,LOGIN_OK))
             {
                 QMessageBox::information(this,"登入",LOGIN_OK);
                 m_strCurPath=QString("./%1").arg(m_strLoginName);
                 OperateWidget::getInstance().show();
                 this->hide();
                 OperateWidget::getInstance().getBook()->flushFile();
             }
             else if(0==strcmp(pdu->data,LOGIN_FAILED))
             {
                 QMessageBox::warning(this,"登入",LOGIN_FAILED);
             }
             break;
        }
        case ENUM_MSG_TYPE_CANCEL_RESPOND:
        {
             if(0==strcmp(pdu->data,CANCEL_OK))
             {
                 QMessageBox::information(this,"注销",CANCEL_OK);
             }
             else if(0==strcmp(pdu->data,CANCEL_FAILED))
             {
                 QMessageBox::warning(this,"注销",CANCEL_FAILED);
             }
             break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
        {
            OperateWidget::getInstance().getFriend()->showAllOnlineUser(pdu);
             break;
        }
        case ENUM_MSG_TYPE_SEARCH_USER_RESPOND:
        {
            if(0==strcmp(SEARCH_USR_NO,pdu->data))
            {
                QMessageBox::information(this,"搜索",QString("%1:not exist").arg(OperateWidget::getInstance().getFriend()->m_strname));
            }
            else if(0==strcmp(SEARCH_USR_ONLINE,pdu->data))
            {
                QMessageBox::information(this,"搜索",QString("%1:online").arg(OperateWidget::getInstance().getFriend()->m_strname));
            }
            else if(0==strcmp(SEARCH_USR_OFFLINE,pdu->data))
            {
                QMessageBox::information(this,"搜索",QString("%1:offline").arg(OperateWidget::getInstance().getFriend()->m_strname));
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
        {
            if(0==strcmp(ALREADY_FRIEND,pdu->data))
            {
                QMessageBox::warning(this,"加好友",ALREADY_FRIEND);
            }
            else if(0==strcmp(UNKNOW_ERR,pdu->data))
            {
                QMessageBox::warning(this,"加好友",UNKNOW_ERR);
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char selfname[32]={'\0'};
            char oppname[32]={'\0'};
            strncpy(selfname,pdu->data,32);
            strncpy(oppname,pdu->data+32,32);
            int ret=QMessageBox::information(this, "添加好友", QString("%1 want to add you as friend ?").arg(selfname)
                                     , QMessageBox::Yes, QMessageBox::No);
            PDU *respdu=mkPDU(0);
            memcpy(respdu->data, pdu->data, 64);
            if (QMessageBox::Yes == ret)
            {
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;
            }
            else
            {
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
            }
            m_tcpSocket.write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
        {
            char oppName[32] = {'\0'};
            memcpy(oppName, pdu->data+32, 32);
            QMessageBox::information(this, "添加好友", QString("添加%1好友成功").arg(oppName));
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        {
            char oppName[32] = {'\0'};
            memcpy(oppName, pdu->data+32, 32);
            QMessageBox::information(this, "添加好友", QString("添加%1好友成功").arg(oppName));
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_UNKNOW_ERR:
        {
            QMessageBox::warning(this, "添加好友", UNKNOW_ERR);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
        {
            OperateWidget::getInstance().getFriend()->updateFriendList(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
        {
            if(0==strcmp(DELETE_ERR,pdu->data))
            {
                QMessageBox::warning(this,"删除好友",DELETE_ERR);
            }
            else if(0==strcmp(DELETE_OK,pdu->data))
            {
                QMessageBox::warning(this,"删除好友",DELETE_OK);
            }
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        {
            char name[32]={'\0'};
            strncpy(name,pdu->data,32);
            QMessageBox::information(this, "删除好友", QString("%1 already delete you!").arg(name));
            break;
        }
        case ENUM_MSG_TYPE_PRIVATECHAT_FRIEND_REQUEST:
        {
            if (PrivateChat::getInstance().isHidden())
            {
                PrivateChat::getInstance().show();
            }
            char caSendName[32] = {'\0'};
            memcpy(caSendName, pdu->data, 32);
            QString strSendName = caSendName;
            PrivateChat::getInstance().setName(strSendName);

            PrivateChat::getInstance().updateMsg(pdu);

            break;
        }
        case ENUM_MSG_TYPE_PRIVATECHAT_FRIEND_RESPOND:
        {
            if (PrivateChat::getInstance().isHidden())
            {
                PrivateChat::getInstance().show();
            }
            PrivateChat::getInstance().myMsgShow(pdu);
            break;
        }
        case ENUM_MSG_TYPE_MASSTEXT_FRIEND_RESPOND:
        {
           OperateWidget::getInstance().getFriend()->showMyMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_MASSTEXT_FRIEND_REQUEST:
        {
            OperateWidget::getInstance().getFriend()->showMassMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:
        {
            QMessageBox::information(this,"创建文件",pdu->data);
            if(0==strcmp(DIR_CREATE_OK,pdu->data))
            {
                OperateWidget::getInstance().getBook()->flushFile();
            }
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
        {
            OperateWidget::getInstance().getBook()->updateFileList(pdu);
            QString strNewName=OperateWidget::getInstance().getBook()->enterDir();
            if(!strNewName.isEmpty())
            {
                m_strCurPath=QString("%1/%2").arg(m_strCurPath).arg(strNewName);
                qDebug() << m_strCurPath;
                OperateWidget::getInstance().getBook()->clearEnterDir();
            }
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_RESPOND:
        {
            if(0==strcmp(DEL_DIR_OK,pdu->data))
            {
                QMessageBox::information(this,"删除文件夹",DEL_DIR_OK);
                OperateWidget::getInstance().getBook()->flushFile();
            }
            else if(0==strcmp(DEL_DIR_FAILURED,pdu->data))
            {
                QMessageBox::warning(this,"删除文件夹",DEL_DIR_FAILURED);
            }
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:
        {
            if(0==strcmp(RENAME_OK,pdu->data))
            {
                QMessageBox::information(this,"文件重命名",RENAME_OK);
                OperateWidget::getInstance().getBook()->flushFile();
            }
            else if(0==strcmp(RENAME_FAILURED,pdu->data))
            {
                QMessageBox::warning(this,"文件重命名",RENAME_FAILURED);
            }
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_RESPOND:
        {
            QMessageBox::warning(this,"进入文件夹",ENTER_DIR_FAILURED);
            OperateWidget::getInstance().getBook()->clearEnterDir();
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
        {
            if(0==strcmp(UPLOAD_FILE_OK,pdu->data))
            {
                QMessageBox::information(this,"文件上传",UPLOAD_FILE_OK);
                OperateWidget::getInstance().getBook()->flushFile();
            }
            else if(0==strcmp(UPLOAD_FILE_ERR,pdu->data))
            {
                QMessageBox::warning(this,"文件上传",UPLOAD_FILE_ERR);
            }
            break;
        }
        case ENUM_MSG_TYPE_DEL_FILE_RESPOND:
        {
            if(0==strcmp(DEL_FILE_OK,pdu->data))
            {
                QMessageBox::information(this,"删除文件",DEL_FILE_OK);
                OperateWidget::getInstance().getBook()->flushFile();
            }
            else if(0==strcmp(DEL_FILE_FAILURED,pdu->data))
            {
                QMessageBox::warning(this,"删除文件",DEL_FILE_FAILURED);
            }
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:
        {
            qDebug() << pdu->data;
            if(0==strcmp(DOWNLOAD_FILE_ERR,pdu->data))
            {
                QMessageBox::warning(this,"下载文件",DOWNLOAD_FILE_ERR);
            }
            else
            {
                char fileName[32]={'\0'};
                sscanf(pdu->data,"%s %lld",fileName,&(OperateWidget::getInstance().getBook()->m_iTotal));
                qDebug() << fileName;
                qDebug() << OperateWidget::getInstance().getBook()->m_iTotal;
                if(strlen(fileName)>0&&OperateWidget::getInstance().getBook()->m_iTotal>0)
                {
                    m_file.setFileName(OperateWidget::getInstance().getBook()->getSaveFilePath());
                    if(!m_file.open(QIODevice::WriteOnly))
                    {
                        QMessageBox::warning(this,"下载文件","找不到文件所存放的路径");
                    }
                    OperateWidget::getInstance().getBook()->setDownloadStatus(true);
                    qDebug() << "already responed";
                }
            }
            break;
        }
        case ENUM_MSG_TYPE_ONLINE_FRIEND_RESPOND:
        {
            OperateWidget::getInstance().getFriend()->acceptOnlineFriend(pdu);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:
        {
            QMessageBox::information(this,"分享文件",pdu->data);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE_REQUEST:
        {
            char pPath[pdu->uiMsgLen] = {'\0'};
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
            // //aa//bb/cc
            char *pos = strrchr(pPath, '/');
            if (NULL != pos)
            {
                pos++;
                QString strNote = QString("%1 share file->%2 \n Do you accept ?").arg(pdu->data).arg(pos);
                int ret = QMessageBox::question(this, "共享文件", strNote);
                if (QMessageBox::Yes == ret)
                {
                    PDU *respdu = mkPDU(pdu->uiMsgLen);
                    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
                    memcpy(respdu->caMsg, pdu->caMsg, pdu->uiMsgLen);
                    QString strName = TcpClient::getInstance().getLoginName();
                    strcpy(respdu->data, strName.toStdString().c_str());
                    m_tcpSocket.write((char*)respdu, respdu->uiPDULen);
                }
            }
            break;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_RESPOND:
        {
            if(0==strcmp(MOVE_FILE_OK,pdu->data))
            {
                QMessageBox::information(this,"文件移动",MOVE_FILE_OK);
                OperateWidget::getInstance().getBook()->flushFile();
            }
            else if(0==strcmp(MOVE_FILE_FAILURED,pdu->data))
            {
                QMessageBox::warning(this,"文件移动",MOVE_FILE_FAILURED);
            }
            else if(0==strcmp(COMMON_ERR,pdu->data))
            {
                QMessageBox::warning(this,"文件移动",COMMON_ERR);
            }
            break;
        }
        default:
             break;
        }
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QByteArray buffer=m_tcpSocket.readAll();
        m_file.write(buffer);
        Book *pBook=OperateWidget::getInstance().getBook();
        pBook->m_iRecved+=buffer.size();
        if(pBook->m_iRecved==pBook->m_iTotal)
        {
            m_file.close();
            pBook->m_iRecved=0;
            pBook->m_iTotal=0;
            pBook->setDownloadStatus(false);
            QMessageBox::information(this,"下载文件","下载文件成功！");
        }
        else if(pBook->m_iRecved>pBook->m_iTotal)
        {
            m_file.close();
            pBook->m_iRecved=0;
            pBook->m_iTotal=0;
            pBook->setDownloadStatus(false);
            QMessageBox::warning(this,"下载文件","下载文件出错！");
        }
    }
}

//void TcpClient::on_sendBtn_clicked()
//{
//    QString strMsg=ui->lineEdit->text();
//    if(!strMsg.isEmpty())
//    {
//        PDU *pdu=mkPDU(strMsg.size());
//        pdu->uiMsgType=6666;
//        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
//        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
//        free(pdu);
//        pdu=NULL;
//    }
//    else
//    {
//        QMessageBox::warning(this,"信息发送","发送的信息不能为空！");
//    }
//}

void TcpClient::on_login_btn_clicked()
{
    QString strName=ui->lineEdit_name->text();
    QString strPwd=ui->lineEdit_pwd->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        m_strLoginName=strName;
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->data,strName.toStdString().c_str(),32);
        strncpy(pdu->data+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
        if(!ui->lock_user->isChecked())
        {
            ui->lineEdit_name->clear();
            ui->lineEdit_pwd->clear();
        }
    }
    else
    {
        QMessageBox::critical(this,"错误提示","用户名和密码不能为空!");
    }
}

void TcpClient::on_regist_btn_clicked()
{
    QString strName=ui->lineEdit_name->text();
    QString strPwd=ui->lineEdit_pwd->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->data,strName.toStdString().c_str(),32);
        strncpy(pdu->data+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
        if(!ui->lock_user->isChecked())
        {
            ui->lineEdit_name->clear();
            ui->lineEdit_pwd->clear();
        }
    }
    else
    {
        QMessageBox::critical(this,"错误提示","用户名和密码不能为空!");
    }
}

void TcpClient::on_cancel_btn_clicked()
{
    QString strName=ui->lineEdit_name->text();
    QString strPwd=ui->lineEdit_pwd->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_CANCEL_REQUEST;
        strncpy(pdu->data,strName.toStdString().c_str(),32);
        strncpy(pdu->data+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
        if(!ui->lock_user->isChecked())
        {
            ui->lineEdit_name->clear();
            ui->lineEdit_pwd->clear();
        }
    }
    else
    {
        QMessageBox::critical(this,"错误提示","用户名和密码不能为空!");
    }
}
