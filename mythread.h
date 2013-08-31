#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include<QTcpSocket>
#include<QDebug>
#include <QMetaEnum>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QByteArray>
#include <QFile>
#include <QByteArray>


class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(int ID,QObject *parent = 0);
    void run();
    int DownloadStrted;
    enum RequestCommands
    {
        USER,
        RETR,//! Username
        LIST               //! List a directory

    };

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:

    void readyRead();
    void disconnected();
   // void Tofilelist();
    void bytesWritten(qint64 bytes);

private:
    QTcpSocket *socket;
    int socketDescriptor;
     QString downloadFilePath;
     QString uploadFilePath;
     QString studentIndex;
    //QString mCommandBuffer;
    QByteArray mCommandBuffer;
    //void DoLIST(QString Arg);
    void DoRETR(QString Arg);
   // void ProcessCommand(QString ClientCommand);
   // void ExecuteCommand(QString ClientCommand);
    void ProcessCommand(QByteArray ClientCommand);
    void ExecuteCommand(QByteArray ClientCommand);
    void ListFile(QString Path);
    void SendResponse(QString data);
   // void mywork(int info);
    //void mydownwork(int info);
    //void DoINIT(QString Arg);
    void DoINIT(QByteArray Arg);
   // void DoDownload(QString Arg);
    void DoNewDownload(QByteArray Arg);
    void DoDownload(QByteArray Arg);
     void DoUpload(QByteArray Arg);
    QFile *newfile;

};

#endif // MYTHREAD_H
