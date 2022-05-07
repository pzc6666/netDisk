#include "operatewidget.h"

OperateWidget::OperateWidget(QWidget *parent) : QWidget(parent)
{
    this->m_listWidget=new QListWidget(this);
    this->m_listWidget->addItem("好友");
    this->m_listWidget->addItem("图书");
    this->setWindowTitle("柯瑞网盘");
    m_pFrided=new Friend;
    m_pBook=new Book;

    m_pSW=new QStackedWidget;
    m_pSW->addWidget(m_pFrided);
    m_pSW->addWidget(m_pBook);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_listWidget);
    pMain->addWidget(m_pSW);

    setLayout(pMain);

    connect(m_listWidget,SIGNAL(currentRowChanged(int)),m_pSW,SLOT(setCurrentIndex(int)));
}

OperateWidget &OperateWidget::getInstance()
{
    static OperateWidget instance;
    return instance;
}

Friend *OperateWidget::getFriend()
{
    return m_pFrided;
}

Book *OperateWidget::getBook()
{
    return m_pBook;
}
