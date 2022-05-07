#ifndef OPERATEWIDGET_H
#define OPERATEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "friend.h"
#include "book.h"
#include <QStackedWidget>

class OperateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OperateWidget(QWidget *parent = 0);

    static OperateWidget &getInstance();

    Friend *getFriend();

    Book *getBook();

signals:

public slots:

private:
    QListWidget *m_listWidget;
    Friend *m_pFrided;
    Book *m_pBook;

    QStackedWidget *m_pSW;
};

#endif // OPERATEWIDGET_H
