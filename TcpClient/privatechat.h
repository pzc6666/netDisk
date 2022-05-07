#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include <QDateTime>
#include "protocol.h"

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChat(QWidget *parent = 0);
    static PrivateChat& getInstance();
    void setName(QString privateName);
    void updateMsg(PDU *pdu);
    void myMsgShow(PDU *pdu);
    ~PrivateChat();

private slots:
    void on_send_btn_clicked();

private:
    Ui::PrivateChat *ui;

    QString m_strLoginName;
    QString m_strPrivateName;
};

#endif // PRIVATECHAT_H
