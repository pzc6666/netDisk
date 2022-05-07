#ifndef LOGWORKER_H
#define LOGWORKER_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QMutex>

class LogWorker : public QObject
{
    Q_OBJECT
public:
    explicit LogWorker(QObject *parent = 0);

    void writeLog();
    static LogWorker& getInstance();

    QStringList getList();

    void setList(QStringList list);

    void setFilePath(QString filepath);

    QString getFilePath();
signals:
    void sWriteLog();

public slots:

private:
    QThread *t1;
    QStringList List;
    QString filepath;
    QMutex mutex;
};

#endif // LOGWORKER_H
