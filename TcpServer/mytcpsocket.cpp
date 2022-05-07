#include "mytcpsocket.h"
#include "mytcpserver.h"

MyTcpSocket::MyTcpSocket(): QTcpSocket()
{
    m_bUpload=false;
    m_timer=new QTimer;
//    setAutoDelete(true);
    connect(this,&MyTcpSocket::readyRead,this,&MyTcpSocket::recvMesg);
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(sendFileToClient()));
    //Log::getInstance();
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::copyDir(QString strSrcDir, QString strDestDir)
{
    QDir dir;
    dir.mkdir(strDestDir);

    dir.setPath(strSrcDir);
    QFileInfoList fileInfoList = dir.entryInfoList();

    QString srcTmp;
    QString destTmp;
    for (int i=0; i<fileInfoList.size(); i++)
    {
        qDebug() << "fileName:" << fileInfoList[i].fileName();
        if (fileInfoList[i].isFile())
        {
            srcTmp = strSrcDir+'/'+fileInfoList[i].fileName();
            destTmp = strDestDir+'/'+fileInfoList[i].fileName();
            QFile::copy(srcTmp, destTmp);
        }
        else if (fileInfoList[i].isDir())
        {
            if (QString(".") == fileInfoList[i].fileName()
                    || QString("..") == fileInfoList[i].fileName())
            {
                continue;
            }
            srcTmp = strSrcDir+'/'+fileInfoList[i].fileName();
            destTmp = strDestDir+'/'+fileInfoList[i].fileName();
            copyDir(srcTmp, destTmp);
        }
    }
}

void MyTcpSocket::recvMesg()
{
    qDebug() << "run：" << QThread::currentThreadId() ;
    //QEventLoop eventLoop;
//    connect(tcp,&MyTcpSocket::readyRead,tcp,[=](){
    if(!m_bUpload)
    {
        qDebug() << "接受数据线程号为：" << QThread::currentThreadId() ;
        qDebug() << "收到：" << this->bytesAvailable();
        uint uiPDULen=0;
        this->read((char*)&uiPDULen,sizeof(uint));
        //qDebug() << uiPDULen ;
        uint uiMsgLen=uiPDULen-sizeof(PDU);
        PDU *pdu=mkPDU(uiMsgLen);
        this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
        //qDebug() << pdu->uiMsgType << " " << (char *)pdu->caMsg;
        //    qDebug() << name << password << pdu->uiMsgType ;
        switch(pdu->uiMsgType)
        {
        case ENUM_MSG_TYPE_REGIST_REQUEST:
        {
            char name[32]={'\0'};
            char password[32]={'\0'};
            strncpy(name,pdu->data,32);
            strncpy(password,pdu->data+32,32);
            bool ret=OpenDb::getInstance().handleRegist(name,password);
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;
            if(ret)
            {
                strcpy(respdu->data,REGIST_OK);
                QDir dir;
                dir.mkdir(QString("./%1").arg(name));
            }
            else
            {
                strcpy(respdu->data,REGIST_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST:
        {
            char name[32]={'\0'};
            char password[32]={'\0'};
            strncpy(name,pdu->data,32);
            strncpy(password,pdu->data+32,32);
            qDebug() << "name:" << name << " password" << password;
            bool ret=OpenDb::getInstance().handleLogin(name,password);
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;
            qDebug() << "------1";
            if(ret)
            {
                strcpy(respdu->data,LOGIN_OK);
                m_strName=name;
            }
            else
            {
                strcpy(respdu->data,LOGIN_FAILED);
            }
            qDebug() << "------2";
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            qDebug() << "------3";
            break;
        }
        case ENUM_MSG_TYPE_CANCEL_REQUEST:
        {
            char name[32]={'\0'};
            char password[32]={'\0'};
            strncpy(name,pdu->data,32);
            strncpy(password,pdu->data+32,32);
            bool ret=OpenDb::getInstance().handleCancel(name,password);
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_CANCEL_RESPOND;
            if(ret)
            {
                QDir dir;
                dir.setPath(QString("./%1").arg(name));
                int flag=dir.removeRecursively();
                if(flag)
                {
                    strcpy(respdu->data,CANCEL_OK);
                }
                else
                {
                    strcpy(respdu->data,CANCEL_FAILED);
                }
            }
            else
            {
                strcpy(respdu->data,CANCEL_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
        {
            QStringList ret=OpenDb::getInstance().handleAllOnline();
            uint uiMsgLen=ret.size()*32;
            PDU *respdu=mkPDU(uiMsgLen);
            respdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for(int i=0;i<ret.size();i++)
            {
                memcpy((char*)respdu->caMsg+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:
        {
            int ret=OpenDb::getInstance().handleSearchUser(pdu->data);
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USER_RESPOND;
            if(-1==ret)
            {
                strcpy(respdu->data,SEARCH_USR_NO);
            }
            else if(1==ret)
            {
                strcpy(respdu->data,SEARCH_USR_ONLINE);
            }
            else if(0==ret)
            {
                strcpy(respdu->data,SEARCH_USR_OFFLINE);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char selfname[32]={'\0'};
            char oppname[32]={'\0'};
            strncpy(selfname,pdu->data,32);
            strncpy(oppname,pdu->data+32,32);
            int ret=OpenDb::getInstance().handleAddFriend(selfname,oppname);
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            if(ret==0)
            {
                strcpy(respdu->data,ALREADY_FRIEND);
            }
            else if(ret==-1)
            {
                strcpy(respdu->data,UNKNOW_ERR);
            }
            else if(ret==1)
            {
                MyTcpServer::getInstance().resend(oppname,pdu);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
        {
            char selfname[32]={'\0'};
            char oppname[32]={'\0'};
            strncpy(selfname,pdu->data,32);
            strncpy(oppname,pdu->data+32,32);
            int ret=OpenDb::getInstance().handleAddFriendAgree(selfname,oppname);
            if(!ret)
            {
                pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_UNKNOW_ERR;
            }
            MyTcpServer::getInstance().resend(selfname, pdu);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        {
            char selfname[32]={'\0'};
            strncpy(selfname,pdu->data,32);
            MyTcpServer::getInstance().resend(selfname, pdu);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
        {
            char name[32]={'\0'};
            strncpy(name,pdu->data,32);
            QStringList ret=OpenDb::getInstance().handleFlushFriend(name);
            uint uiMsgLen = ret.size()*32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for (int i=0; i<ret.size(); i++)
            {
                memcpy((char*)(respdu->caMsg)+i*32
                       , ret.at(i).toStdString().c_str()
                       , ret.at(i).size());
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        {
            char selfname[32]={'\0'};
            char oppname[32]={'\0'};
            strncpy(selfname,pdu->data,32);
            strncpy(oppname,pdu->data+32,32);
            int ret=OpenDb::getInstance().handleDelFriend(selfname,oppname);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            if(!ret)
            {
                strcpy(respdu->data,DELETE_ERR);
            }
            else
            {
                strcpy(respdu->data,DELETE_OK);
                MyTcpServer::getInstance().resend(oppname,pdu);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_PRIVATECHAT_FRIEND_REQUEST:
        {
            char oppname[32]={'\0'};
            strncpy(oppname,pdu->data+32,32);
            qDebug() << oppname;
            MyTcpServer::getInstance().resend(oppname,pdu);
            PDU *respdu=pdu;
            respdu->uiMsgType=ENUM_MSG_TYPE_PRIVATECHAT_FRIEND_RESPOND;
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_MASSTEXT_FRIEND_REQUEST:
        {
            char name[32]={'\0'};
            strncpy(name,pdu->data,32);
            QStringList ret=OpenDb::getInstance().requestOnlineFriend(name);
            for(int i=0;i<ret.size();i++)
            {
                QString temp=ret.at(i);
                MyTcpServer::getInstance().resend(temp.toStdString().c_str(),pdu);
            }
            PDU *respdu=pdu;
            respdu->uiMsgType=ENUM_MSG_TYPE_MASSTEXT_FRIEND_RESPOND;
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
        {
            QDir dir;
            QString strCurPath=QString("%1").arg((char*)pdu->caMsg);
            bool ret=dir.exists(strCurPath);
            PDU *repdu=NULL;
            if(ret)
            {
                char dirName[32]={'\0'};
                strncpy(dirName,pdu->data+32,32);
                QString newDirPath=strCurPath+'/'+dirName;
                ret=dir.exists(newDirPath);
                if(ret)
                {
                    repdu=mkPDU(0);
                    repdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(repdu->data,FILE_EXIST);
                }
                else
                {
                    dir.mkdir(newDirPath);
                    repdu=mkPDU(0);
                    repdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(repdu->data,DIR_CREATE_OK);
                }
            }
            else
            {
                repdu=mkPDU(0);
                repdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(repdu->data,DIR_NOT_EXIST);
            }
            write((char*)repdu,repdu->uiPDULen);
            free(repdu);
            repdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
        {
            char strCurPath[pdu->uiMsgLen]={'\0'};
            strncpy(strCurPath,(char*)pdu->caMsg,pdu->uiMsgLen);
            qDebug() << strCurPath ;
            QDir dir(strCurPath);
            QFileInfoList FileInfoList=dir.entryInfoList();
            int fileCount=FileInfoList.size();
            PDU *repdu=mkPDU(sizeof(FileInfo)*fileCount);
            repdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
            FileInfo *pFileInfo=NULL;
            for(int i=0;i<fileCount;i++)
            {
                pFileInfo=(FileInfo*)repdu->caMsg+i;
                QString strFileName=FileInfoList.at(i).fileName();
                qDebug() << strFileName ;
                memcpy(pFileInfo->fileName,strFileName.toStdString().c_str(),strFileName.size());
                if(FileInfoList.at(i).isDir())
                {
                    pFileInfo->fileType=0;
                }
                else if(FileInfoList.at(i).isFile())
                {
                    pFileInfo->fileType=1;
                }
            }
            write((char*)repdu,repdu->uiPDULen);
            free(repdu);
            repdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_REQUEST:
        {
            char name[32]={'\0'};
            char strCurPath[pdu->uiMsgLen]={'\0'};
            strncpy(name,pdu->data,32);
            strncpy(strCurPath,(char*)pdu->caMsg,pdu->uiMsgLen);
            QString strPath=QString("%1/%2").arg(strCurPath).arg(name);
            QFileInfo fileInfo(strPath);
            bool ret=false;
            if(fileInfo.isDir())
            {
                QDir dir;
                dir.setPath(strPath);
                ret=dir.removeRecursively();
            }
            PDU *repdu=mkPDU(0);
            repdu->uiMsgType=ENUM_MSG_TYPE_DEL_DIR_RESPOND;
            if(ret)
            {
                strcpy(repdu->data,DEL_DIR_OK);
            }
            else
            {
                strcpy(repdu->data,DEL_DIR_FAILURED);
            }
            write((char*)repdu,repdu->uiPDULen);
            free(repdu);
            repdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:
        {
            char oldName[32]={'\0'};
            char newName[32]={'\0'};
            char strCurPath[pdu->uiMsgLen]={'\0'};
            strncpy(oldName,pdu->data,32);
            strncpy(newName,pdu->data+32,32);
            strncpy(strCurPath,(char*)pdu->caMsg,pdu->uiMsgLen);
            QString strOldPath=QString("%1/%2").arg(strCurPath).arg(oldName);
            QString strNewPath=QString("%1/%2").arg(strCurPath).arg(newName);
            QDir dir;
            bool ret=dir.rename(strOldPath,strNewPath);
            PDU *repdu=mkPDU(0);
            repdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
            if(ret)
            {
                strcpy(repdu->data,RENAME_OK);
            }
            else
            {
                strcpy(repdu->data,RENAME_FAILURED);
            }
            write((char*)repdu,repdu->uiPDULen);
            free(repdu);
            repdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:
        {
            char strNewName[32]={'\0'};
            char strCurPath[pdu->uiMsgLen]={'\0'};
            strncpy(strNewName,pdu->data,32);
            strncpy(strCurPath,(char*)pdu->caMsg,pdu->uiMsgLen);
            QString strNewPath=QString("%1/%2").arg(strCurPath).arg(strNewName);
            qDebug() << strNewPath;
            QFileInfo fileInfo(strNewPath);
            if(fileInfo.isDir())
            {
                QDir dir(strNewPath);
                QFileInfoList FileInfoList=dir.entryInfoList();
                int fileCount=FileInfoList.size();
                PDU *repdu=mkPDU(sizeof(FileInfo)*fileCount);
                repdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
                FileInfo *pFileInfo=NULL;
                for(int i=0;i<fileCount;i++)
                {
                    pFileInfo=(FileInfo*)repdu->caMsg+i;
                    QString strFileName=FileInfoList.at(i).fileName();
                    qDebug() << strFileName ;
                    memcpy(pFileInfo->fileName,strFileName.toStdString().c_str(),strFileName.size());
                    if(FileInfoList.at(i).isDir())
                    {
                        pFileInfo->fileType=0;
                    }
                    else if(FileInfoList.at(i).isFile())
                    {
                        pFileInfo->fileType=1;
                    }
                }
                write((char*)repdu,repdu->uiPDULen);
                free(repdu);
                repdu=NULL;
                break;
            }
            else
            {
                PDU *repdu=mkPDU(0);
                repdu->uiMsgType=ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
                strcpy(repdu->data,ENTER_DIR_FAILURED);
                write((char*)repdu,repdu->uiPDULen);
                free(repdu);
                repdu=NULL;
            }
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
        {
            char fileName[32]={'\0'};
            char filePath[pdu->uiMsgLen]={'\0'};
            qint64 fileSize=0;
            sscanf(pdu->data,"%s %lld",fileName,&fileSize);
            qDebug() << fileSize;
            strncpy(filePath,(char*)pdu->caMsg,pdu->uiMsgLen);
            qDebug() << filePath;
            qDebug() << fileName;
            QString fileNewPath=QString("%1/%2").arg(filePath).arg(fileName);
            qDebug() << fileNewPath;
            m_file.setFileName(fileNewPath);
            if(m_file.open(QIODevice::WriteOnly))
            {
                m_iRecved=0;
                m_iTotal=fileSize;
                m_bUpload=true;
                qDebug() << "run here";
            }
            else
            {
                PDU *repdu=mkPDU(0);
                repdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
                strcpy(repdu->data,UPLOAD_FILE_ERR);
                write((char*)repdu,repdu->uiPDULen);
                free(repdu);
                repdu=NULL;
            }
            break;
        }
        case ENUM_MSG_TYPE_DEL_FILE_REQUEST:
        {
            char name[32]={'\0'};
            char strCurPath[pdu->uiMsgLen]={'\0'};
            strncpy(name,pdu->data,32);
            strncpy(strCurPath,(char*)pdu->caMsg,pdu->uiMsgLen);
            QString strPath=QString("%1/%2").arg(strCurPath).arg(name);
            QFileInfo fileInfo(strPath);
            bool ret=false;
            if(fileInfo.isFile())
            {
                QDir dir;
                ret=dir.remove(strPath);
            }
            PDU *repdu=mkPDU(0);
            repdu->uiMsgType=ENUM_MSG_TYPE_DEL_FILE_RESPOND;
            if(ret)
            {
                strcpy(repdu->data,DEL_FILE_OK);
            }
            else
            {
                strcpy(repdu->data,DEL_FILE_FAILURED);
            }
            write((char*)repdu,repdu->uiPDULen);
            free(repdu);
            repdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_DOWMLOAD_FILE_REQUEST:
        {
            char fileName[32]={'\0'};
            char strCurPath[pdu->uiMsgLen]={'\0'};
            strncpy(fileName,pdu->data,32);
            strncpy(strCurPath,(char*)pdu->caMsg,pdu->uiMsgLen);
            QString newFileName=QString("%1/%2").arg(strCurPath).arg(fileName);
            qDebug() << newFileName;
            QFileInfo fileInfo(newFileName);
            m_file.setFileName(newFileName);
            PDU *repdu=mkPDU(0);
            repdu->uiMsgType=ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
            if(m_file.open(QIODevice::ReadOnly))
            {
                qint64 fileSize=fileInfo.size();
                qDebug() << "fileSize:" << fileSize;
                sprintf(repdu->data,"%s %lld",fileName,fileSize);
                m_timer->start(1000);
            }
            else
            {
                strcpy(repdu->data,DOWNLOAD_FILE_ERR);
            }
            write((char*)repdu,repdu->uiPDULen);
            free(repdu);
            repdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_ONLINE_FRIEND_REQUEST:
        {
            char name[32]={'\0'};
            strncpy(name,pdu->data,32);
            QStringList ret=OpenDb::getInstance().requestOnlineFriend(name);
            uint uiMsgLen = ret.size()*32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_ONLINE_FRIEND_RESPOND;
            for (int i=0; i<ret.size(); i++)
            {
                memcpy((char*)(respdu->caMsg)+i*32
                       , ret.at(i).toStdString().c_str()
                       , ret.at(i).size());
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
        {
            char caSendName[32] = {'\0'};
            int num = 0;
            sscanf(pdu->data, "%s%d", caSendName, &num);
            int size = num*32;
            PDU *respdu = mkPDU(pdu->uiMsgLen-size);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_REQUEST;
            strcpy(respdu->data, caSendName);
            memcpy(respdu->caMsg, (char*)(pdu->caMsg)+size, pdu->uiMsgLen-size);

            char caRecvName[32] = {'\0'};
            for (int i=0; i<num; i++)
            {
                memcpy(caRecvName, (char*)(pdu->caMsg)+i*32, 32);
                MyTcpServer::getInstance().resend(caRecvName, respdu);
            }
            free(respdu);
            respdu = NULL;

            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            strcpy(respdu->data, "share file ok");
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND:
        {
            QString strRecvPath = QString("./%1").arg(pdu->data);
            QString strShareFilePath = QString("%1").arg((char*)(pdu->caMsg));
            int index = strShareFilePath.lastIndexOf('/');
            QString strFileName = strShareFilePath.right(strShareFilePath.size()-index-1);
            strRecvPath = strRecvPath+'/'+strFileName;

            QFileInfo fileInfo(strShareFilePath);
            if (fileInfo.isFile())
            {
                QFile::copy(strShareFilePath, strRecvPath);
            }
            else if (fileInfo.isDir())
            {
                copyDir(strShareFilePath, strRecvPath);
            }
            break;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:
        {
            char caFileName[32] = {'\0'};
            int srcLen = 0;
            int destLen = 0;
            sscanf(pdu->data, "%d %d %s", &srcLen, &destLen, caFileName);

            char pSrcPath[srcLen+1] ={'\0'};
            char pDestPath[destLen+1+32] ={'\0'};
            memcpy(pSrcPath, (char*)pdu->caMsg,srcLen);
            memcpy(pDestPath, (char*)(pdu->caMsg)+(srcLen+1), destLen);
            qDebug() << (char*)pdu->caMsg;
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
            QFileInfo fileInfo(pDestPath);
            if (fileInfo.isDir())
            {
                strcat(pDestPath, "/");
                strcat(pDestPath, caFileName);
                qDebug() << pSrcPath;
                qDebug() << pDestPath;
                bool ret = QFile::rename(pSrcPath, pDestPath);
                if (ret)
                {
                    strcpy(respdu->data, MOVE_FILE_OK);
                }
                else
                {
                    strcpy(respdu->data, COMMON_ERR);
                }
            }
            else if (fileInfo.isFile())
            {
                strcpy(respdu->data, MOVE_FILE_FAILURED);
            }

            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        default:
            break;
        }
        free(pdu);
        pdu=NULL;
        qDebug() << "------4";
    }
    else
    {
        qDebug() <<"-----1";
        PDU *respdu = NULL;
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;

        QByteArray buff = readAll();
        m_file.write(buff);
        m_iRecved += buff.size();
        qDebug() << m_iTotal;
        qDebug() << m_iRecved;
        if (m_iTotal == m_iRecved)
        {
            m_file.close();
            m_bUpload = false;

            strcpy(respdu->data, UPLOAD_FILE_OK);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
        else if (m_iTotal < m_iRecved)
        {
            m_file.close();
            m_bUpload = false;
            strcpy(respdu->data, UPLOAD_FILE_ERR);

            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
    }

//    });
    // 进入事件循环
   // eventLoop.exec();
}
void MyTcpSocket::clientOffline()
{
    bool ret=OpenDb::getInstance().handleOffline(m_strName.toStdString().c_str());
    if(!ret)
    {
        QMessageBox::warning(NULL,"警告","登入状态修改失败，请检查服务器");
    }
    emit offline(this);
}

void MyTcpSocket::sendFileToClient()
{
    m_timer->stop();
    qint64 ret=0;
    char *pData=new char[4096];
    while(true)
    {
        ret=m_file.read(pData,4096);
        if(ret>0&&ret<=4096)
        {
            write(pData,ret);
        }
        else if(ret==0)
        {
            m_file.close();
            break;
        }
        else
        {
            qDebug() << "发送文件内容给客户端失败！";
            m_file.close();
            break;
        }
    }
    delete [] pData;
    pData=NULL;
}

void MyTcpSocket::recvTcp(MyTcpSocket *mysocket)
{
    tcp=mysocket;
}
