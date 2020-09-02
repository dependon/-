#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QThread>
#include <QLabel>
#include <QPainter>
#include <QLineEdit>
#include <QFile>
#include <QPushButton>
#include <QCoreApplication>
#include <QEventLoop>
#include <QAction>
#include <QTextEdit>
#include <QDebug>
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
struct ImageFrameHead {
    int funCode;                        //!功能码
    unsigned int uTransFrameHdrSize;    //!sizeof(WIFI_FRAME_HEADER)
    unsigned int uTransFrameSize;       //!sizeof(WIFI_FRAME_HEADER) + Data Size

    //数据帧变量
    unsigned int uDataFrameSize;        //数据帧的总大小
    unsigned int uDataFrameTotal;       //一帧数据被分成传输帧的个数
    unsigned int uDataFrameCurr;        //数据帧当前的帧号
    unsigned int uDataInFrameOffset;    //数据帧在整帧的偏移
};

class UdpThread : public QThread
{
    Q_OBJECT
public:
    explicit UdpThread(QObject *parent = 0);
    ~UdpThread();
    int duankou1=65522;
   // int flag=1;
    void lianjie();


protected:
    virtual void run();



signals:
    void sigRecvOk(char *buf, int len);

public slots:
    void slotRecv();
private:
    QUdpSocket *m_udpSocket;
    char *m_buf;

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int duankou=65522;


private slots:
    void slotRecv(char *buf, int len);
    void duankou_file()
    {
        //m_udpThread->flag=1;
       QString qtext = ed->text();
        duankou=qtext.toInt();
        qDebug()<<duankou;
        m_udpThread->duankou1=duankou;
        qDebug()<<m_udpThread->duankou1;
        m_udpThread->lianjie();
    }
    void tingzhi_file()
    {
       /*
        QString qtext = ed->text();
         duankou=qtext.toInt();
         qDebug()<<duankou;
         m_udpThread->duankou1=duankou;
         qDebug()<<m_udpThread->duankou1;
         m_udpThread->lianjie();*/
        m_udpThread->duankou1=-1;
        m_udpThread->lianjie();


    }

private:
    QLabel *lb;
    UdpThread *m_udpThread;
    //QPixmap *pixmap;

    QLineEdit *ed;
    QAction *bt3;
    QAction *bt1;


};

#endif // MAINWINDOW_H
