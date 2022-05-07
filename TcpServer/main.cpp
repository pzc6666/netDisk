#include "tcpserver.h"
//#include "opendb.h"
#include "log.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenDb::getInstance().init();
    Log::getInstance();
    TcpServer w;
    //w.show();

    return a.exec();
}
