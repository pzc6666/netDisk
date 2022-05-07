#ifndef OPENDB_H
#define OPENDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QStringList>
//#include "log.h"

class OpenDb : public QObject
{
    Q_OBJECT
public:
    explicit OpenDb(QObject *parent = 0);
    static OpenDb& getInstance();
    void init();
    bool handleRegist(const char* name,const char* password);
    bool handleLogin(const char* name,const char* password);
    bool handleCancel(const char* name,const char* password);
    bool handleOffline(const char* name);
    QStringList handleAllOnline();
    int handleSearchUser(const char *name);
    int handleAddFriend(const char* selfName,const char* oppName);
    bool handleAddFriendAgree(const char* selfName,const char* oppName);
    bool handleAllOffline();
    QStringList handleFlushFriend(const char* name);
    bool handleDelFriend(const char* selfname,const char* oppname);
    QStringList requestOnlineFriend(const char* name);
    ~OpenDb();
signals:

public slots:

private:
    QSqlDatabase m_db;//连接数据库
};

#endif // OPENDB_H
