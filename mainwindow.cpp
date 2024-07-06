#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <deque>
#include <string>
#include <QFile>
#include <iostream>
#include <QSystemSemaphore>
#include <QDateTime>
#include <QInputDialog>
#include <QCloseEvent>


struct TimeDef {
    std::string def;
    std::string lock; //lock or unlock;
    int	begin_hour;
    int	begin_minute;
    int	end_hour;
    int	end_minute;
    int    beginsecs;
    int    endsecs;
};
std::deque<TimeDef*> allDef;
std::wstring xxminute=L"";
int alwaysshow = 0;
QString password;
int fullscreen=0;
int maxscreen=1;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);





    QFile ff("LockScreenRest.conf");
    QString ffctt;
    if(ff.exists()==false){
        ff.open(QFile::OpenModeFlag::ReadWrite);
        auto buf = "#help\n#maxscreen=1\n#fullscreen=1\n#password=\n#hotkey=Ctrl+Shift+Alt+X\n#keyboardallow=1\n#hotkeyallow=1\n#alpha=200\n#lock|unlock hour:minute-hour:minute\nlock 00:00-07:00\nunlock 07:00-07:50\nlock 07:50-08:00\nunlock 08:00-08:50\nlock 08:50-09:00\nunlock 09:00-09:50\nlock 09:50-10:00\nunlock 10:00-10:50\nlock 10:50-11:00\nunlock 11:00-11:50\nlock 11:50-13:00\nunlock 13:00-13:50\nlock 13:50-14:00\nunlock 14:00-14:50\nlock 14:50-15:00\nunlock 15:00-15:50\nlock 15:50-16:00\nunlock 16:00-16:50\nlock 16:50-17:00\nunlock 17:00-17:50\nlock 17:50-18:00\nunlock 18:00-18:50\nlock 18:50-19:00\nunlock 19:00-19:50\nlock 19:50-21:00\nunlock 21:00-21:50\nlock 21:50-22:00\nunlock 22:00-22:50\nlock 22:50-23:00\nunlock 23:00-23:50\nlock 23:50-24:00\n\n\n\n\n";
        ff.write(buf);
        ffctt=buf;
        ff.close();
    }else{
        ff.open(QFile::OpenModeFlag::ReadOnly);
        auto bs=ff.readAll();
        ffctt=bs;
        ff.close();

    }
    std::string ss=ffctt.toStdString();
    auto buf=ss.data();

    int bufi = 0;
    char *bufn = strchr(buf, '\n');
    std::string hotkey="X";
    int keyboardallow = 1;
    int alpha = 200;
    int hotkeyallow = 1;
    for (; bufn != nullptr;) {
        std::string lin(buf, bufn - buf);
        if (lin.size() > 0) {
            if (lin[lin.size() - 1] == '\r') {
                lin = lin.substr(0, lin.size() - 1);
            }
            if (lin.size() > 0 && lin[0] != '#' && lin.find("lock ")!=std::string::npos && lin.find('-')!=std::string::npos && lin.find(':')!=std::string::npos && (lin.size()==16 || lin.size()==18)) {
                //lin format:lock hour:minute-hour:minute
                TimeDef *td = new TimeDef;
                td->def=lin.substr(lin.find(' ')+1);
                td->lock = lin.substr(0, lin.find(' '));
                auto sp = lin.find(' ');
                auto slashp = lin.find('-');
                auto slashppre = lin.substr(sp+1, slashp-(sp+1));
                auto slashpaft = lin.substr(slashp + 1);
                auto slashpprep = slashppre.find(':');
                auto slashpaftp = slashpaft.find(':');
                td->begin_hour = strtol(slashppre.substr(0, slashpprep).data(), nullptr, 10);
                td->begin_minute = strtol(slashppre.substr(slashpprep + 1).data(), nullptr, 10);
                td->beginsecs = td->begin_hour * 3600 + td->begin_minute * 60;
                td->end_hour = strtol(slashpaft.substr(0, slashpaftp).data(), nullptr, 10);
                td->end_minute = strtol(slashpaft.substr(slashpaftp + 1).data(), nullptr, 10);
                td->endsecs = td->end_hour * 3600 + td->end_minute * 60;
                allDef.push_back(td);
            }
            else if (lin.find("=") != std::string::npos &&  lin.substr(0, lin.find("=")+1) == "#hotkey=") {
                hotkey = lin.substr(lin.find("=")+1);
            }else if (lin.find("=")!=std::string::npos && lin.substr(0, lin.find("=") + 1) == "#keyboardallow=") {
                keyboardallow = strtol(lin.substr(lin.find("=") + 1).data(),nullptr,10);
            }
            else if (lin.find("=")!=std::string::npos && lin.substr(0, lin.find("=")+1) == "#alpha=") {
                alpha = strtol(lin.substr(lin.find("=")+ 1).data(),nullptr,10);
            }else if (lin.find("=")!=std::string::npos && lin.substr(0, lin.find("=")+1) == "#hotkeyallow=") {
                hotkeyallow = strtol(lin.substr(lin.find("=")+ 1).data(),nullptr,10);
            } else if (lin.find("=") != std::string::npos &&  lin.substr(0, lin.find("=")+1) == "#password=") {
                password = lin.substr(lin.find("=")+1).data();
            }else if (lin.find("=") != std::string::npos &&  lin.substr(0, lin.find("=")+1) == "#fullscreen=") {
                fullscreen = strtol(lin.substr(lin.find("=")+ 1).data(),nullptr,10);
            }else if (lin.find("=") != std::string::npos &&  lin.substr(0, lin.find("=")+1) == "#maxscreen=") {
                maxscreen = strtol(lin.substr(lin.find("=")+ 1).data(),nullptr,10);
            }
        }
        buf = bufn + 1;
        bufn = strchr(buf, '\n');
    }



    connect(this,SIGNAL(showui(std::string,int,int)),this,SLOT(Showui(std::string,int,int)));
    connect(this,SIGNAL(showui2(std::string,int,int)),this,SLOT(Showui2(std::string,int,int)));
    connect(this,SIGNAL(hideui(std::string,int,int)),this,SLOT(Hideui(std::string,int,int)));
    connect(this,SIGNAL(hideui2(std::string,int,int)),this,SLOT(Hideui2(std::string,int,int)));

    connect(&locktimer,SIGNAL(timeout()),this,SLOT(locktimer_timeout()));
    locktimer.start(5000);
}

extern QSystemSemaphore semap;
MainWindow::~MainWindow()
{
    // 完成任务后释放信号量
    semap.release(1);
    std::cout << "进程释放锁。" << std::endl;
    delete ui;
}

void MainWindow::Showui(std::string def,int beginsecs,int endsecs){
    if(this->isHidden()){
        if(maxscreen==1){
            if(this->isHidden() || this->isMaximized()==false){
                this->showMaximized();
            }
        }else if(fullscreen==1){
            this->showFullScreen();
        }else{
            this->showNormal();
        }
        this->activateWindow();
        //this->setWindowFlag(Qt::WindowStaysOnTopHint);
    }else{
        this->activateWindow();
    }
    QString minutenum=QString("%1").arg((endsecs - beginsecs) / 60);
    ui->msgl->setText("Take A Rest "+minutenum +" Minute");
    QDateTime dt=QDateTime::currentDateTime();
    auto curts=dt.toString("yyyy-MM-ddThh:mm:ss");
    ui->resttime->setText(QString("Rest time:")+QString::fromStdString(def)+"    Now:"+curts);


}




void MainWindow::Showui2(std::string def,int beginsecs,int endsecs){
    if(this->isHidden()){
        if(maxscreen==1){
            if(this->isHidden() || this->isMaximized()==false){
                this->showMaximized();
            }
        }else if(fullscreen==1){
            this->showFullScreen();
        }else{
            this->showNormal();
        }
        this->activateWindow();
        //this->setWindowFlag(Qt::WindowStaysOnTopHint);
    }else{
        this->activateWindow();
    }
    QString minutenum=QString("%1").arg((endsecs - beginsecs) / 60);
    ui->msgl->setText("Alt+F4 Close Window For Working "+minutenum +" Minutes");
    QDateTime dt=QDateTime::currentDateTime();
    auto curts=dt.toString("yyyy-MM-ddThh:mm:ss");
    ui->resttime->setText(QString("Now work time:")+QString::fromStdString(def)+"    Now:"+curts);
}




void MainWindow::Hideui(std::string def,int beginsecs,int endsecs){
    this->hide();
    xxminute = L"";
    //if (this->isHidden()==false) {
    ui->msgl->setText("Alt+F4 Close Window For Working");
    QDateTime dt=QDateTime::currentDateTime();
    auto curts=dt.toString("yyyy-MM-ddThh:mm:ss");
    ui->resttime->setText(QString("Now work time:")+QString::fromStdString(def)+"    Now:"+curts);
    //}
}



void MainWindow::Hideui2(std::string def,int beginsecs,int endsecs){
    this->hide();
    xxminute = L"";
    //if (this->isHidden()==false) {
    QString minutenum=QString("%1").arg((endsecs - beginsecs) / 60);
    ui->msgl->setText("Take A Rest "+minutenum +" Minute");
    QDateTime dt=QDateTime::currentDateTime();
    auto curts=dt.toString("yyyy-MM-ddThh:mm:ss");
    ui->resttime->setText(QString("Rest time:")+QString::fromStdString(def)+"    Now:"+curts);
    //}
}




bool unlockuntilnextunlock=false;
bool lockuntiluserclose=false;

void MainWindow::locktimer_timeout()
{
    if (allDef.size() == 0) {
        if (this->isMaximized()) {
            ui->msgl->setText("Empty Time List!");
            this->activateWindow();
        }
    }else {
        time_t now;
        time(&now);
        auto date = localtime(&now);
        auto cursecs = date->tm_hour * 3600 + date->tm_min * 60;
        for (int i = 0; i < allDef.size(); i += 1) {
            auto it = allDef[i];
            if (cursecs >= it->beginsecs && cursecs < it->endsecs) {
                if (it->lock == std::string("lock",4)) {
                    if(QFile::exists("ShowOnce")){
                        QFile::remove("ShowOnce");
                        lockuntiluserclose=true;
                    }
                    if(lockuntiluserclose){
                        //this->setFocus();
                        //this->showNormal();
                        //this->setFocus();
                        emit showui(allDef[i]->def,allDef[i]->beginsecs,allDef[i]->endsecs);
                        continue;
                        if(this->isHidden()){
                            if(maxscreen==1){
                                if(this->isHidden() || this->isMaximized()==false){
                                    this->showMaximized();
                                }
                            }else if(fullscreen==1){
                                this->showFullScreen();
                            }else{
                                this->showNormal();
                            }
                            this->activateWindow();
                            //this->setWindowFlag(Qt::WindowStaysOnTopHint);
                        }else{
                            this->activateWindow();
                        }
                        QString minutenum=QString("%1").arg((it->endsecs - it->beginsecs) / 60);
                        ui->msgl->setText("Take A Rest "+minutenum +" Minute");
                        QDateTime dt=QDateTime::currentDateTime();
                        auto curts=dt.toString("yyyy-MM-ddThh:mm:ss");
                        ui->resttime->setText(QString("Rest time:")+QString::fromStdString(allDef[i]->def)+"    Now:"+curts);
                        continue;
                    }
                    if(unlockuntilnextunlock){
                        emit hideui2(allDef[i]->def,allDef[i]->beginsecs,allDef[i]->endsecs);
                        continue;
                        //this->hide();
                    }else{
                        //this->setFocus();
                        //this->showNormal();
                        //this->setFocus();
                        emit showui(allDef[i]->def,allDef[i]->beginsecs,allDef[i]->endsecs);
                        continue;
                        if(this->isHidden()){
                            if(maxscreen==1){
                                if(this->isHidden() || this->isMaximized()==false){
                                    this->showMaximized();
                                }
                            }else if(fullscreen==1){
                                this->showFullScreen();
                            }else{
                                this->showNormal();
                            }
                            this->activateWindow();
                        }else{
                            this->activateWindow();
                        }

                        QString minutenum=QString("%1").arg((it->endsecs - it->beginsecs) / 60);
                        ui->msgl->setText("Take A Rest "+minutenum +" Minute");
                        QDateTime dt=QDateTime::currentDateTime();
                        auto curts=dt.toString("yyyy-MM-ddThh:mm:ss");
                        ui->resttime->setText(QString("Rest time:")+QString::fromStdString(allDef[i]->def)+"    Now:"+curts);
                        break;
                    }
                }
                else if (it->lock == std::string("unlock",6)) {
                    unlockuntilnextunlock=false;
                    if(QFile::exists("ShowOnce") || lockuntiluserclose){
                        QFile::remove("ShowOnce");
                        lockuntiluserclose=true;
                        emit showui2(allDef[i]->def,allDef[i]->beginsecs,allDef[i]->endsecs);
                        continue;
                        if(maxscreen==1){
                            if(this->isHidden() || this->isMaximized()==false){
                                this->showMaximized();
                            }
                        }else if(fullscreen==1){
                            this->showFullScreen();
                        }else{
                            this->showNormal();
                        }
                        this->activateWindow();

                        ui->msgl->setText("Alt+F4 Close Window For Working");
                        QDateTime dt=QDateTime::currentDateTime();
                        auto curts=dt.toString("yyyy-MM-ddThh:mm:ss");
                        ui->resttime->setText(QString("Now work time:")+QString::fromStdString(allDef[i]->def)+"    Now:"+curts);
                    }else{

                        emit hideui(allDef[i]->def,allDef[i]->beginsecs,allDef[i]->endsecs);
                        continue;
                        this->hide();
                        xxminute = L"";
                        //if (this->isHidden()==false) {
                        ui->msgl->setText("Alt+F4 Close Window For Working");
                        QDateTime dt=QDateTime::currentDateTime();
                        auto curts=dt.toString("yyyy-MM-ddThh:mm:ss");
                        ui->resttime->setText(QString("Now work time:")+QString::fromStdString(allDef[i]->def)+"    Now:"+curts);
                        //}
                    }
                    break;
                }
            }
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *ce)
{
    if(password!=""){
        QInputDialog idlg;
        idlg.setLabelText("Password");
        auto ipass=idlg.getText(this,"Input Close Password","Password:",QLineEdit::Password);
        if(ipass!=password){
            ce->ignore();
            return;
        }
    }
    lockuntiluserclose=false;
    unlockuntilnextunlock=true;
    this->hide();
    ce->ignore();
}
