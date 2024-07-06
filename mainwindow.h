#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>
#include <QSystemSemaphore>
#include <iostream>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void closeEvent(QCloseEvent *ce);
public slots:
    void locktimer_timeout();

    void Showui(std::string def,int beginsecs,int endsecs);
    void Showui2(std::string def,int beginsecs,int endsecs);
    void Hideui(std::string def,int beginsecs,int endsecs);
    void Hideui2(std::string def,int beginsecs,int endsecs);

signals:
    void showui(std::string def,int beginsecs,int endsecs);
    void showui2(std::string def,int beginsecs,int endsecs);
    void hideui(std::string def,int beginsecs,int endsecs);
    void hideui2(std::string def,int beginsecs,int endsecs);
private:
    QTimer locktimer;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};



extern QSystemSemaphore semap;
class MyThread : public QThread
{
    Q_OBJECT
protected:
    void run() override {
        std::cout << "进程获得锁，运行 acquire。" << std::endl;
        if (semap.acquire()) {
            std::cout << "进程获得锁，继续执行。" << std::endl;
        } else {
            std::cout << "无法获得锁，进程可能正在运行。" << std::endl;
        }
    }
};




#endif // MAINWINDOW_H
