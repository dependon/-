#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMainWindow>
#include <QUdpSocket>
#include <QFile>
#include <QPushButton>
#include <QCoreApplication>
#include <QEventLoop>
#include <QAction>
#include <QTextEdit>
#include <QDebug>
#include <QLineEdit>


#define TRAN_SIZE 1024;
struct ImageFrameHead
{
    int funCode;                        //!功能码
    unsigned int uTransFrameHdrSize;    //!sizeof(WIFI_FRAME_HEADER)
    unsigned int uTransFrameSize;       //!sizeof(WIFI_FRAME_HEADER) + Data Size

    //数据帧变量
    unsigned int uDataFrameSize;        //数据帧的总大小
    unsigned int uDataFrameTotal;       //一帧数据被分成传输帧的个数
    unsigned int uDataFrameCurr;        //数据帧当前的帧号
    unsigned int uDataInFrameOffset;    //数据帧在整帧的偏移
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event)
   {
    flag=0;
   }

private slots:
    void chuanshu();
    void zanting()
    {
        if(flag==1)
            flag=0;
        else
            flag=1;
    }
    void duankou_file()
    {
        flag=0;
       QString qtext = ed->text();
        duankou=qtext.toInt();
        qDebug()<<duankou;
    }

private:
    QAction *bt;
    QUdpSocket *m_udpSocket;
    int flag=1;
    QAction *bt2;
    QAction *bt3;
    int duankou=65522;
    QLineEdit *ed;
    QTabWidget *tabw;

};



#endif // MAINWINDOW_H
