#include "opendb.h"

OpenDb::OpenDb(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    //init();
    //Log::getInstance();
}

OpenDb &OpenDb::getInstance()
{
    static OpenDb instance;
    return instance;
}

void OpenDb::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D:\\network_disk\\TcpServer\\cloud.db");
    if(m_db.open())
    {
        QSqlQuery query;
        query.exec("select * from usrInfo");
        while(query.next())
        {
            QString data=QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }
    }
    else
    {
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败！");
    }
}

bool OpenDb::handleRegist(const char *name, const char *password)
{
    if(NULL==name||NULL==password) return false;
    QString data=QString("insert into usrInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(password);
    QSqlQuery query;
    return query.exec(data);
}

bool OpenDb::handleLogin(const char *name, const char *password)
{
    if(NULL==name||NULL==password) return false;
    QString data=QString("select * from usrInfo where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(password);
    QSqlQuery query;
    query.exec(data);
    if(query.next())
    {
        data=QString("update usrInfo set online=1 where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(password);
        QSqlQuery query;
        query.exec(data);
        return true;
    }
    else
    {
        return false;
    }
}

bool OpenDb::handleCancel(const char *name, const char *password)
{
    if(NULL==name||NULL==password) return false;
    QString data=QString("delete from usrInfo where name=\'%1\' and pwd=\'%2\'").arg(name).arg(password);
    QSqlQuery query;
    return query.exec(data);
}

bool OpenDb::handleOffline(const char *name)
{
    if(NULL==name) return false;
    QString data=QString("update usrInfo set online=0 where name=\'%1\'").arg(name);
    QSqlQuery query;
    return query.exec(data);
}

QStringList OpenDb::handleAllOnline()
{
    QString data=QString("select name from usrInfo where online=1");
    QSqlQuery query;
    query.exec(data);
    QStringList result;
    result.clear();
    while(query.next())
    {
        qDebug() << query.value(0).toString();
        result.append(query.value(0).toString());
    }
    return result;
}

int OpenDb::handleSearchUser(const char *name)
{
    if(NULL==name) return -1;
    QString data=QString("select online from usrInfo where name=\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    while(query.next())
    {
        int ret=query.value(0).toInt();
        if(1==ret)
        {
            return 1;
        }
        else if(0==ret)
        {
            return 0;
        }
    }
    return -1;
}

int OpenDb::handleAddFriend(const char *selfName, const char *oppName)
{
    if(NULL==selfName||NULL==oppName)
    {
        return -1;
    }
    QString data=QString("select * from friend where (id=(select id from usrInfo where name=\'%1\') and friendid=(select id from usrInfo where name=\'%2\'))"
                         "or (id=(select id from usrInfo where name=\'%3\') and friendid=(select id from usrInfo where name=\'%4\'))").arg(selfName).arg(oppName).arg(oppName).arg(selfName);
    QSqlQuery query;
    query.exec(data);
    if(query.next())
    {
        return 0;
    }
    return 1;
}

bool OpenDb::handleAddFriendAgree(const char *selfName, const char *oppName)
{
    if(NULL==selfName||NULL==oppName)
    {
        return false;
    }
    qDebug() << "好友名字：" << oppName;
    QString data = QString("insert into friend(id, friendId) values((select id from usrInfo where name=\'%1\'), (select id from usrInfo where name=\'%2\'))").arg(selfName).arg(oppName);
    QSqlQuery query;
    return query.exec(data);
}

bool OpenDb::handleAllOffline()
{
    QString data=QString("update usrInfo set online=0 where online=1");
    QSqlQuery query;
    return query.exec(data);
}

QStringList OpenDb::handleFlushFriend(const char *name)
{
    QStringList friendList;
    friendList.clear();
    if(NULL==name)
    {
        return friendList;
    }
    QString data=QString("select name,online from usrInfo where id in (select friendId from friend where id=(select id from usrInfo where name=\'%1\'))").arg(name);
    QSqlQuery query;
    query.exec(data);
    while(query.next())
    {
        QString strname=query.value(0).toString();
        QString status;
        if(query.value(1)==1)
        {
            status=" (online)";
        }
        else
        {
            status=" (offline)";
        }
        qDebug() << strname+status;
        friendList.append(strname+status);
    }
    data=QString("select name,online from usrInfo where id in (select id from friend where friendId=(select id from usrInfo where name=\'%1\'))").arg(name);
    query.exec(data);
    while(query.next())
    {
        QString strname=query.value(0).toString();
        QString status;
        if(query.value(1)==1)
        {
            status=" (online)";
        }
        else
        {
            status=" (offline)";
        }
        qDebug() << strname+status;
        friendList.append(strname+status);
    }
    return friendList;
}

bool OpenDb::handleDelFriend(const char* selfname,const char* oppname)
{
    if(selfname==NULL||oppname==NULL) return false;
    QString data=QString("delete from friend where (id=(select id from usrInfo where name=\'%1\') "
                         "and friendId=(select id from usrInfo where name=\'%2\')) or "
                         "(id=(select id from usrInfo where name=\'%3\') and "
                         "friendId=(select id from usrInfo where name=\'%4\'))").arg(selfname).arg(oppname).arg(oppname).arg(selfname);
    QSqlQuery query;
    return query.exec(data);
}

QStringList OpenDb::requestOnlineFriend(const char *name)
{
    QStringList friendList;
    friendList.clear();
    if(NULL==name)
    {
        return friendList;
    }
    QString data=QString("select name from usrInfo where online=1 and id in (select friendId from friend where id=(select id from usrInfo where name=\'%1\'))").arg(name);
    QSqlQuery query;
    query.exec(data);
    while(query.next())
    {
        QString strname=query.value(0).toString();
        friendList.append(strname);
    }
    data=QString("select name from usrInfo where online=1 and id in (select id from friend where friendId=(select id from usrInfo where name=\'%1\'))").arg(name);
    query.exec(data);
    while(query.next())
    {
        QString strname=query.value(0).toString();
        friendList.append(strname);
    }
    return friendList;
}

OpenDb::~OpenDb()
{
    OpenDb::handleAllOffline();
    m_db.close();
}
