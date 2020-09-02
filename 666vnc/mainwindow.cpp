#include "mainwindow.h"

#include <QTime>
#include <QDesktopWidget>
#include <QToolBar>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
//#include <windows.h>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QThread>
#include <QBuffer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

{
    /* bool isedited;
    isedited=false;*/
    this->setAutoFillBackground(true);
      QPalette palette = this->palette();
      palette.setBrush(QPalette::Window,
                 QBrush(QPixmap(":/l.jpg").scaled(// 缩放背景图.
                     this->size(),
                     Qt::IgnoreAspectRatio,
                     Qt::SmoothTransformation)));             // 使用平滑的缩放方式
         this->setPalette(palette);

    setMinimumSize(300,120);
    setMaximumSize(300,120);
    this->setWindowTitle("广播端控制台");
    this->setWindowIcon(QIcon(":/start.png"));

    m_udpSocket = new QUdpSocket(this);
    m_bt= new QAction("发送");
    m_bt->setIcon(QIcon(":/start.png"));
    m_bt->setShortcut(QKeySequence("F1"));

    m_bt2= new QAction("暂停/继续");
    m_bt2->setIcon(QIcon(":/pause.png"));
    m_bt2->setShortcut(QKeySequence("F2"));

    m_bt3= new QAction("发送端口更改");
    m_bt3->setIcon(QIcon(":/port_parrellel.png"));
    m_bt3->setShortcut(QKeySequence("F3"));

    QToolBar *fileToolBar = addToolBar(tr("功能区域"));
    fileToolBar->addAction(m_bt);
    fileToolBar->addAction(m_bt2);
    fileToolBar->addAction(m_bt3);
    ed=new QLineEdit("65522");
    //QVBoxLayout *vbox = new QVBoxLayout(this);
    this->setCentralWidget(ed);
    ed->resize(100,25);

   /* tabw = new QTabWidget;
    tabw->setTabsClosable(true);
    this->setCentralWidget(tabw);
    tabw->setMinimumSize(270, 50);*/
    QStatusBar *stb =statusBar();
    stb->addWidget(ed);


    qDebug()<<"send";
    connect(m_bt,SIGNAL(triggered(bool)),this,SLOT(chuanshu()));
    connect(m_bt2,SIGNAL(triggered(bool)),this,SLOT(zanting()));
    connect(m_bt3,SIGNAL(triggered(bool)),this,SLOT(m_host_file()));
}
MainWindow::~MainWindow()
{

}

void MainWindow::chuanshu()
{

     m_flag=1;
    while(m_flag==1)
    {
        QDesktopWidget *desk = QApplication::desktop();
        QScreen  *screen = QGuiApplication::primaryScreen();
        QPixmap p = screen->grabWindow(desk->winId());
# if 0
        p.save("1.png");

        QFile file("1.png");
        if (!file.open(QIODevice::ReadOnly))
            return;
//         qDebug()<<bytes;
        char *m_sendBuf = new char[1024];
        int size = file.size();

        int num = 0;
        int count = 0;
        int endSize = size%996;
        if (endSize == 0) {
            num = size/996;
        }
        else {
            num = size/996+1;
        }
        while (count < num) {
            memset(m_sendBuf, 0, 1024);
            ImageFrameHead mes;
            mes.funCode = 24;
            mes.uTransFrameHdrSize = sizeof(ImageFrameHead);
            if ((count+1) != num) {
                mes.uTransFrameSize = 996;
            }
            else {
                mes.uTransFrameSize = endSize;
            }
            qDebug()<<size;
            qDebug()<<mes.uTransFrameHdrSize;
            mes.uDataFrameSize = size;
            mes.uDataFrameTotal = num;
            mes.uDataFrameCurr = count+1;
            mes.uDataInFrameOffset = count*(1024 - sizeof(ImageFrameHead));
            file.read(m_sendBuf+sizeof(ImageFrameHead), 1024-sizeof(ImageFrameHead));
            memcpy(m_sendBuf, (char *)&mes, sizeof(ImageFrameHead));
            m_udpSocket->writeDatagram(m_sendBuf, mes.uTransFrameSize+mes.uTransFrameHdrSize, QHostAddress("255.255.255.255"), m_host);
            QTime dieTime = QTime::currentTime().addMSecs(1);
            while( QTime::currentTime() < dieTime )
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            count++;
        }
        file.close();
        QTime dieTime = QTime::currentTime().addMSecs(200);
        while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        qDebug("ok");
        QThread::msleep(500);
#else
                QByteArray bytes;
                QBuffer buff(&bytes);
                buff.open(QIODevice::ReadWrite);
                p.save(&buff,"PNG");
                QByteArray sendBuff;

                sendBuff.resize(DATA_FRAME_SIZE);
                char *m_sendBuf = new char[1024];
                int size = buff.size();
                int num = 0;
                int count = 0;
                int endSize = size%996;
                if (endSize == 0) {
                    num = size/996;
                }
                else {
                    num = size/996+1;
                }
                while (count < num) {
                    memset(m_sendBuf, 0, 1024);
                    ImageFrameHead mes;
                    mes.funCode = 24;
                    mes.uTransFrameHdrSize = sizeof(ImageFrameHead);
                    if ((count+1) != num) {
                        mes.uTransFrameSize = 996;
                    }
                    else {
                        mes.uTransFrameSize = endSize;
                    }
                    qDebug()<<size;
                    qDebug()<<mes.uTransFrameHdrSize;
                    mes.uDataFrameSize = size;
                    mes.uDataFrameTotal = num;
                    mes.uDataFrameCurr = count+1;
                    mes.uDataInFrameOffset = count*(1024 - sizeof(ImageFrameHead));
                    memcpy(sendBuff.data()+sizeof(ImageFrameHead), (bytes.data()+mes.uDataInFrameOffset), DATA_FRAME_SIZE - sizeof(ImageFrameHead));
                    memcpy(sendBuff.data(), (char *)&mes, sizeof(ImageFrameHead));
                    m_udpSocket->writeDatagram(sendBuff, mes.uTransFrameSize+mes.uTransFrameHdrSize, QHostAddress("255.255.255.255"), m_host);
                    QTime dieTime = QTime::currentTime().addMSecs(1);
                    while( QTime::currentTime() < dieTime )
                        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                    count++;
                }
                buff.close();
                QTime dieTime = QTime::currentTime().addMSecs(200);
                while( QTime::currentTime() < dieTime )
                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                qDebug("ok");
                QThread::msleep(50);
#endif
    }

}

