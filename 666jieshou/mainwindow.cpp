#include "mainwindow.h"
#include <QToolBar>
#include <QStatusBar>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    this->setAutoFillBackground(true);
      QPalette palette = this->palette();
      palette.setBrush(QPalette::Window,
                 QBrush(QPixmap(":/timg.jpg").scaled(// 缩放背景图.
                     this->size(),
                     Qt::IgnoreAspectRatio,
                     Qt::SmoothTransformation)));             // 使用平滑的缩放方式
         this->setPalette(palette);
      setMinimumSize(300, 120);
      setMaximumSize(300, 120);

    this->setWindowTitle("接收端后台控制台");
    this->setWindowIcon(QIcon(":/receiver.png"));
    lb=new QLabel;
    ed=new QLineEdit("65522");
    this->setCentralWidget(ed);
    ed->resize(100,25);

    bt3=new QAction("更改接收端口并开始接收");
    bt3->setIcon(QIcon(":/receiver.png"));
    bt3->setShortcut(QKeySequence("F1"));

    bt1=new QAction("直接停止并无法再连接");
    bt1->setIcon(QIcon(":/stop.png"));
    bt1->setShortcut(QKeySequence("F2"));

    QToolBar *fileToolBar = addToolBar(tr("功能区域"));

    fileToolBar->addAction(bt3);
    fileToolBar->addAction(bt1);

    QStatusBar *stb =statusBar();
    stb->addWidget(ed);



    m_udpThread = new UdpThread;
    connect(bt1,SIGNAL(triggered(bool)),this,SLOT(tingzhi_file()));
    connect(bt3,SIGNAL(triggered(bool)),this,SLOT(duankou_file()));
    connect(m_udpThread, SIGNAL(sigRecvOk(char*,int)), this, SLOT(slotRecv(char*,int)));
    m_udpThread->start();
    qDebug("OK");

}

MainWindow::~MainWindow()
{

}
void MainWindow::slotRecv(char * buf, int len)
{
    if(len!=0)
    {
    QPixmap pixmap;
    pixmap.loadFromData((uchar*)buf, len, "PNG");
    //p.drawPixmap(QPoint(0, 0), QPixmap (pixmap));
    lb->setScaledContents(true);
    //pixmap.scaled(0.5,0.5,Qt::KeepAspectRatio);
    lb->setPixmap(pixmap);
    lb->show();
    }

}

UdpThread::UdpThread(QObject *parent) :
    QThread(parent)
{

    m_buf = new char[1024*1024];
    memset(m_buf, 0, 1024);
    /*setMinimumSize(300, 150);
    this->setWindowTitle("接收端前台，彻底关闭请关闭前台");
    this->setWindowIcon(QIcon(":/receiver.png"));*/

}

UdpThread::~UdpThread()
{
    m_udpSocket->close();
    delete m_buf;
    exit();
}

void UdpThread::run()
{
  /* m_udpSocket = new QUdpSocket;
    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(slotRecv()),Qt::DirectConnection);
    qDebug()<< m_udpSocket->bind(QHostAddress::Any, duankou1);
    qDebug()<<duankou1;
    exec();*/
}
void UdpThread::lianjie()
{

    m_udpSocket = new QUdpSocket;
    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(slotRecv()),Qt::DirectConnection);
    qDebug()<< m_udpSocket->bind(QHostAddress::Any, duankou1);
    qDebug()<<duankou1;
    exec();

}

void UdpThread::slotRecv()
{
    char *recvBuf = new char[1052];
    memset(recvBuf, 0, 1052);
    //qDebug("start");
    while(m_udpSocket->hasPendingDatagrams()) {
        memset(recvBuf, 0, 1052);
        int size = m_udpSocket->pendingDatagramSize();

        //qDebug()<<"size"<<size;
        m_udpSocket->readDatagram(recvBuf, size);
        ImageFrameHead *mes = (ImageFrameHead *)recvBuf;
        //qDebug()<<mes->uDataFrameCurr<<mes->uDataFrameTotal;
        if (mes->funCode == 24) {
            memcpy(m_buf+mes->uDataInFrameOffset, (recvBuf+ sizeof(ImageFrameHead)), mes->uTransFrameSize);
            if (mes->uDataFrameCurr == mes->uDataFrameTotal) {
                qDebug("write");
                emit sigRecvOk(m_buf, mes->uDataFrameSize);
            }
        }

    }
    delete recvBuf;
}
