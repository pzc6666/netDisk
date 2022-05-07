#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include "protocol.h"
#include "sharefile.h"

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = 0);
    ~Book();
    void updateFileList(PDU *pdu);
    void clearEnterDir();
    QString enterDir();
    QString getSaveFilePath();
    void setDownloadStatus(bool downloadStatus);
    bool getDownloadStatus();
    void requestOnlineFriend();
    void ShareFile(QStringList friendList);
    QString getShareFileName();


    qint64 m_iTotal=0;
    qint64 m_iRecved=0;
signals:

public slots:
    void createDir();
    void flushFile();
    void delDir();
    void fileRename();
    void enterDir(const QModelIndex &index);
    void returnPre();
    void uploadFile();
    void uploadFileData();
    void delFile();
    void downloadFile();
    void onlineShow();
    void moveFile();
    void selectAimDir();

private:
    QListWidget *m_pBookListW;
    QPushButton *m_pReturnPB;
    QPushButton *m_pCreateDirPB;
    QPushButton *m_pDelDirPB;
    QPushButton *m_pRenamePB;
    QPushButton *m_pFlushFilePB;
    QPushButton *m_pUploadPB;
    QPushButton *m_pDownLoadPB;
    QPushButton *m_pDelFilePB;
    QPushButton *m_pShareFilePB;
    QPushButton *m_pMoveFilePB;
    QPushButton *m_pSelectDirPB;

    QString m_strEnterDir;
    QString m_strUploadFilePath;
    QString m_strSaveFilePath;

    QTimer *m_pTimer=new QTimer(this);

    bool m_downloadStatus;

    QString m_strShareFileName;

    QString m_strMoveFileName;
    QString m_strMoveFilePath;
    QString m_strDestDir;
};

#endif // BOOK_H
