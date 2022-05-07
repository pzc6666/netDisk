#include "book.h"
#include "tcpclient.h"

Book::Book(QWidget *parent) : QWidget(parent)
{
    m_downloadStatus=false;
    m_pBookListW = new QListWidget;
    m_pReturnPB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelDirPB = new QPushButton("删除文件夹");
    m_pRenamePB = new QPushButton("重命名文件");
    m_pFlushFilePB = new QPushButton("刷新文件");

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushFilePB);


    m_pUploadPB = new QPushButton("上传文件");
    m_pDownLoadPB = new QPushButton("下载文件");
    m_pDelFilePB = new QPushButton("删除文件");
    m_pShareFilePB = new QPushButton("共享文件");
    m_pMoveFilePB = new QPushButton("移动文件");
    m_pSelectDirPB = new QPushButton("目标目录");

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);
    pFileVBL->addWidget(m_pMoveFilePB);
    pFileVBL->addWidget(m_pSelectDirPB);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB,SIGNAL(clicked(bool)),this,SLOT(createDir()));
    connect(m_pFlushFilePB,SIGNAL(clicked(bool)),this,SLOT(flushFile()));
    connect(m_pDelDirPB,SIGNAL(clicked(bool)),this,SLOT(delDir()));
    connect(m_pRenamePB,SIGNAL(clicked(bool)),this,SLOT(fileRename()));
    connect(m_pBookListW,SIGNAL(doubleClicked(QModelIndex)),this ,SLOT(enterDir(QModelIndex)));
    connect(m_pReturnPB,SIGNAL(clicked(bool)),this,SLOT(returnPre()));
    connect(m_pUploadPB,SIGNAL(clicked(bool)),this,SLOT(uploadFile()));
    connect(m_pTimer,SIGNAL(timeout()),this, SLOT(uploadFileData()));
    connect(m_pDelFilePB,SIGNAL(clicked(bool)),this,SLOT(delFile()));
    connect(m_pDownLoadPB,SIGNAL(clicked(bool)),this,SLOT(downloadFile()));
    connect(m_pShareFilePB,SIGNAL(clicked(bool)),this,SLOT(onlineShow()));
    connect(m_pMoveFilePB,SIGNAL(clicked(bool)),this,SLOT(moveFile()));
    connect(m_pSelectDirPB,SIGNAL(clicked(bool)),this,SLOT(selectAimDir()));

    m_pSelectDirPB->setEnabled(false);
}

Book::~Book()
{
    delete m_pTimer;
}

void Book::updateFileList(PDU *pdu)
{
    if(pdu==NULL) return ;

    QListWidgetItem *pItemTmp = NULL;
    int row = m_pBookListW->count();
    while (m_pBookListW->count()>0)
    {
        pItemTmp = m_pBookListW->item(row-1);
        m_pBookListW->removeItemWidget(pItemTmp);
        delete pItemTmp;
        row = row-1;
    }

    int fileCount=pdu->uiMsgLen/sizeof(FileInfo);
    FileInfo *pFileInfo=NULL;

    for(int i=0;i<fileCount;i++)
    {
        pFileInfo=(FileInfo*)pdu->caMsg+i;
        qDebug() << pFileInfo->fileName << " " << pFileInfo->fileType;
        QListWidgetItem *pItem = new QListWidgetItem;
        if (0 == pFileInfo->fileType)
        {
            pItem->setIcon(QIcon(QPixmap(":/map/dir.jpg")));
        }
        else if (1 == pFileInfo->fileType)
        {
            pItem->setIcon(QIcon(QPixmap(":/map/reg.jpg")));
        }
        pItem->setText(pFileInfo->fileName);
        m_pBookListW->addItem(pItem);
    }
}

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::enterDir()
{
    return m_strEnterDir;
}

QString Book::getSaveFilePath()
{
    return m_strSaveFilePath;
}

void Book::createDir()
{
    QString strNewDir=QInputDialog::getText(this,"新建文件夹","请输入新建文件夹名字：");
    if(!strNewDir.isEmpty())
    {
        if(strNewDir.size()>32)
        {
            QMessageBox::warning(this,"新建文件夹","文件夹名字过长,请重新输入");
        }
        else
        {
            QString strLoginName=TcpClient::getInstance().getLoginName();
            QString strCurPath=TcpClient::getInstance().curPath();
            PDU *pdu=mkPDU(strCurPath.size()+1);
            pdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->data,strLoginName.toStdString().c_str(),strLoginName.size());
            strncpy(pdu->data+32,strNewDir.toStdString().c_str(),strNewDir.size());
            strcpy((char*)pdu->caMsg,strCurPath.toStdString().c_str());
            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
        }
    }
    else
    {
        QMessageBox::warning(this,"新建文件夹","新建文件夹名字为空！");
    }
}

void Book::flushFile()
{
    QString strCurPath=TcpClient::getInstance().curPath();
    PDU *pdu=mkPDU(strCurPath.size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strcpy((char*)pdu->caMsg,strCurPath.toStdString().c_str());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Book::delDir()
{
    QString strCurPath=TcpClient::getInstance().curPath();
    QListWidgetItem *pItem=m_pBookListW->currentItem();
    if(pItem==NULL)
    {
        QMessageBox::warning(this,"删除文件夹","未选择要删除文件夹的名字！！");
    }
    else
    {
        QString strDirName=pItem->text();
        PDU *pdu=mkPDU(strCurPath.size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_DEL_DIR_REQUEST;
        strncpy(pdu->data,strDirName.toStdString().c_str(),strDirName.size());
        strcpy((char*)pdu->caMsg,strCurPath.toStdString().c_str());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Book::fileRename()
{
    QString strCurPath=TcpClient::getInstance().curPath();
    QListWidgetItem *pItem=m_pBookListW->currentItem();
    if(pItem==NULL)
    {
        QMessageBox::warning(this,"重命名文件","未选择要重命名文件的名字！！");
    }
    else
    {
        QString strOldName=pItem->text();
        QString strNewName=QInputDialog::getText(this,"文件重命名","请输入你要重命名的名字：");
        if(strNewName.isEmpty())
        {
            QMessageBox::warning(this,"文件重命名","你没有输入要重命名的名字！！！！");
        }
        else
        {
            PDU *pdu=mkPDU(strCurPath.size()+1);
            pdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
            strncpy(pdu->data,strOldName.toStdString().c_str(),strOldName.size());
            strncpy(pdu->data+32,strNewName.toStdString().c_str(),strNewName.size());
            strcpy((char*)pdu->caMsg,strCurPath.toStdString().c_str());
            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
        }
    }
}

void Book::enterDir(const QModelIndex &index)
{
    QString strNewName=index.data().toString();
    m_strEnterDir=strNewName;
    QString strCurPath=TcpClient::getInstance().curPath();
    PDU *pdu=mkPDU(strCurPath.size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    strncpy(pdu->data,strNewName.toStdString().c_str(),strNewName.size());
    strncpy((char*)pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Book::returnPre()
{
    QString strCurPath=TcpClient::getInstance().curPath();
    QString strRootPath="./"+TcpClient::getInstance().getLoginName();
    if(strCurPath==strRootPath)
    {
        QMessageBox::warning(this,"返回上一级","已经是根目录，不能再返回");
    }
    else
    {
        int index=strCurPath.lastIndexOf('/');
        strCurPath.remove(index,strCurPath.size());
        TcpClient::getInstance().setCurPath(strCurPath);
        clearEnterDir();
        flushFile();
    }
}

void Book::uploadFile()
{
    m_strUploadFilePath=QFileDialog::getOpenFileName();
    qDebug() << m_strUploadFilePath;
    if(!m_strUploadFilePath.isEmpty())
    {
        int index=m_strUploadFilePath.lastIndexOf('/');
        QString fileName=m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);
        qDebug() << fileName;
        QFile file(fileName);
        qint64 fileSize=file.size();
        qDebug() << fileSize;
        QString strCurPath=TcpClient::getInstance().curPath();
        PDU *pdu=mkPDU(strCurPath.size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        sprintf(pdu->data,"%s %lld",fileName.toStdString().c_str(),fileSize);
        strncpy((char*)pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        qDebug() << "already send!";
        free(pdu);
        pdu=NULL;

        m_pTimer->start(1000);
    }
    else
    {
        QMessageBox::warning(this,"上传文件","请先选择要上传的文件！");
    }
}

void Book::uploadFileData()
{
    m_pTimer->stop();
    QFile file(m_strUploadFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "上传文件", "打开文件失败");
        return;
    }

    char *pBuffer = new char[4096];
    qint64 ret = 0;
    while (true)
    {
        ret = file.read(pBuffer, 4096);
        if (ret > 0 && ret <= 4096)
        {
            TcpClient::getInstance().getTcpSocket().write(pBuffer, ret);
        }
        else if (0 == ret)
        {
            break;
        }
        else
        {
            QMessageBox::warning(this, "上传文件", "上传文件失败:读文件失败");
            break;
        }
    }
    file.close();
    delete []pBuffer;
    pBuffer = NULL;
}

void Book::delFile()
{
    QString strCurPath=TcpClient::getInstance().curPath();
    QListWidgetItem *pItem=m_pBookListW->currentItem();
    if(pItem==NULL)
    {
        QMessageBox::warning(this,"删除文件","未选择要删除文件的名字！！");
    }
    else
    {
        QString strFileName=pItem->text();
        PDU *pdu=mkPDU(strCurPath.size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_DEL_FILE_REQUEST;
        strncpy(pdu->data,strFileName.toStdString().c_str(),strFileName.size());
        strcpy((char*)pdu->caMsg,strCurPath.toStdString().c_str());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Book::downloadFile()
{
    QListWidgetItem *pItem=m_pBookListW->currentItem();
    if(pItem==NULL)
    {
        QMessageBox::warning(this,"下载文件","未选择要下载文件的名字！！");
    }
    else
    {
        QString strSaveFilePath=QFileDialog::getSaveFileName();
        qDebug() << strSaveFilePath;
        if(strSaveFilePath.isEmpty())
        {
            QMessageBox::warning(this,"下载文件","下载文件的保存路径不能为空！");
            m_strSaveFilePath.clear();
        }
        else
        {
            m_strSaveFilePath=strSaveFilePath;
            QString strCurPath=TcpClient::getInstance().curPath();
            QString strFileName=pItem->text();
            PDU *pdu=mkPDU(strCurPath.size()+1);
            pdu->uiMsgType=ENUM_MSG_TYPE_DOWMLOAD_FILE_REQUEST;
            strncpy(pdu->data,strFileName.toStdString().c_str(),strFileName.size());
            strcpy((char*)pdu->caMsg,strCurPath.toStdString().c_str());
            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
        }
    }
}

void Book::onlineShow()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if (NULL == pItem)
    {
        QMessageBox::warning(this, "分享文件", "请选择要分享的文件");
        return;
    }
    else
    {
        requestOnlineFriend();
        m_strShareFileName = pItem->text();
    }
}

void Book::moveFile()
{
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if (NULL != pCurItem)
    {
        m_strMoveFileName = pCurItem->text();
        QString strCutPath = TcpClient::getInstance().curPath();
        m_strMoveFilePath = strCutPath+'/'+m_strMoveFileName;

        m_pSelectDirPB->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this, "移动文件", "请选择要移动的文件");
    }
}

void Book::selectAimDir()
{
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if (NULL != pCurItem)
    {
        QString strDestDir = pCurItem->text();
        QString strCutPath = TcpClient::getInstance().curPath();
        m_strDestDir = strCutPath+'/'+strDestDir;

        int srcLen = m_strMoveFilePath.size();
        int destLen = m_strDestDir.size();
        PDU *pdu = mkPDU(srcLen+destLen+2);
        pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
        sprintf(pdu->data, "%d %d %s", srcLen, destLen, m_strMoveFileName.toStdString().c_str());
        qDebug() << "move file" << m_strMoveFilePath;
        memcpy((char*)pdu->caMsg, m_strMoveFilePath.toStdString().c_str(), srcLen);
        memcpy((char*)(pdu->caMsg)+(srcLen+1), m_strDestDir.toStdString().c_str(), destLen);

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this, "移动文件", "请选择要移动的文件");
    }
    m_pSelectDirPB->setEnabled(false);
}

void Book::setDownloadStatus(bool downloadStatus)
{
    m_downloadStatus=downloadStatus;
}

bool Book::getDownloadStatus()
{
    return m_downloadStatus;
}

void Book::requestOnlineFriend()
{
    QString login_name=TcpClient::getInstance().getLoginName();
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_ONLINE_FRIEND_REQUEST;
    strncpy(pdu->data,login_name.toStdString().c_str(),login_name.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Book::ShareFile(QStringList friendList)
{
    ShareFile::getInstance().updateFriend(friendList);
    if (ShareFile::getInstance().isHidden())
    {
        ShareFile::getInstance().show();
    }
}

QString Book::getShareFileName()
{
    return m_strShareFileName;
}
