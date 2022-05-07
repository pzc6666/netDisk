#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include "online.h"
#include "privatechat.h"
//#include "tcpclient.h"

class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = 0);
    void showAllOnlineUser(PDU *pdu);
    void updateFriendList(PDU *pdu);
    void showMyMsg(PDU *pdu);
    void showMassMsg(PDU *pdu);
    void acceptOnlineFriend(PDU *pdu);
    QStringList getOnlineFriend();
    QString m_strname;

signals:

public slots:
    void showOnline();
    void searchUser();
    void flushFriend();
    void deleteFriend();
    void privateChat();
    void massTexting();

private:
    QTextEdit *m_pShowMsgTE;
    QListWidget *m_pFriendListWidget;
    QLineEdit *m_pInputMsgLE;

    QPushButton *m_pDelFriendPB;
    QPushButton *m_pFlushFriendPB;
    QPushButton *m_pShowOnlineUsrPB;
    QPushButton *m_pSearchUsrPB;
    QPushButton *m_pMsgSendPB;
    QPushButton *m_pPrivateChatPB;

    Online *m_pOnline;
    QStringList m_onlineFriend;
};

#endif // FRIEND_H
