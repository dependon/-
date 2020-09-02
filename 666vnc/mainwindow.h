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

#define DATA_FRAME_SIZE 1024
#define TRAN_SIZE 1024
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
    m_flag=0;
   }

private slots:
    void chuanshu();
    void zanting()
    {
        if(m_flag==1)
            m_flag=0;
        else
            m_flag=1;
    }
    void m_host_file()
    {
        m_flag=0;
       QString qtext = ed->text();
        m_host=qtext.toInt();
        qDebug()<<m_host;
    }

private:
    QAction *m_bt;
    QUdpSocket *m_udpSocket;
    int m_flag=1;
    QAction *m_bt2;
    QAction *m_bt3;
    int m_host=65522;
    QLineEdit *ed;
    QTabWidget *tabw;

};



#endif // MAINWINDOW_H
