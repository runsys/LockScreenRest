#include "mainwindow.h"

#include <QApplication>
#include <QApplication>
#include <QSystemSemaphore>
#include <iostream>
#include <QThread>
#include <QFile>

QSystemSemaphore semap("LockScreenRestAlarmClock", 1, QSystemSemaphore::Open);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyThread thrd;
    thrd.start();
    QThread::sleep(1);
    if(!thrd.isFinished()){
        std::cout<<"已经启动了一个，本进程退出"<<std::endl;
        QFile ff("ShowOnce");
        ff.open(QFile::WriteOnly);
        ff.close();
        return 0;
    }else{
        std::cout<<"没有在运行的程序，本进程启动"<<std::endl;
    }

    MainWindow w;
    w.setWindowTitle("Lock Screen Rest");
    w.setWindowIcon(QIcon(":/appicon.png"));
    w.setWindowFlag(Qt::WindowStaysOnTopHint);
    w.showMaximized();
    return a.exec();
}

