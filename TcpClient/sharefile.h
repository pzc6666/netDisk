#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QCheckBox>
#include <QListWidget>
#include <QDebug>

class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = 0);
    static ShareFile& getInstance();
    void updateFriend(QStringList onlineFriend);
    void test(int count);

signals:

public slots:
    void cancelSelect();
    void selectAll();
    void okSahre();
    void cancelShare();
private:
    QPushButton *m_pSelectAllPB;
    QPushButton *m_pCancelSelectPB;

    QPushButton *m_pOKPB;
    QPushButton *m_pCancelPB;

    QScrollArea *m_pSA;
    QWidget *m_pFriendW;
    QVBoxLayout *m_pFriendWVBL;
    QButtonGroup *m_pButtonGroup;
};

#endif // SHAREFILE_H
