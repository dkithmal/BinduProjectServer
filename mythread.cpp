#include "mythread.h"
#include "ftplistitem.h"

MyThread::MyThread(int ID,QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
    DownloadStrted =0;
    uploadFilePath="D:/dk work/Motarola/project/upload/";
    downloadFilePath ="D:/dk work/Motarola/project/download/";
    studentIndex ="114071H";
}

void MyThread::run()
{
    //thered start hear
    qDebug()<<socketDescriptor<<"starting therad";
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()),Qt::DirectConnection);
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()),Qt::DirectConnection);
    connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)),Qt::DirectConnection);
   qDebug()<<socketDescriptor<<"cliant connected";


   exec();
}


void MyThread::readyRead()
{
    //Append to the command buffer
    if(DownloadStrted==0)
    {
        mCommandBuffer.append(socket->readAll());
        if(mCommandBuffer.endsWith('\n'))
        {
            ProcessCommand(mCommandBuffer);
            mCommandBuffer.clear();
        }
        //qDebug()<<mCommandBuffer;

    }
    else
    {

        DoDownload(socket->readAll());


    }



}


void MyThread::disconnected()
{
     qDebug()<<socketDescriptor<<"Disconnected";
    socket->deleteLater();
    exit(0);

}




void MyThread::ProcessCommand(QByteArray ClientCommand)
{

    ExecuteCommand(ClientCommand);

}


void MyThread::bytesWritten(qint64 bytes)
{
    qDebug()<<bytes;
}



void MyThread::ExecuteCommand(QByteArray ClientCommand)
{

       QByteArray Command = "";
        QByteArray Arg = "";

    if(ClientCommand.contains(" "))
    {
        //Contains arquments
        int pos = ClientCommand.indexOf(" ");
        Command = ClientCommand.mid(0,pos).trimmed();
        Arg = ClientCommand.mid(pos + 1);
    }
    else
    {
        //No arquments
        Command = ClientCommand.trimmed();
    }

    qDebug() << "Client Replay: " << Command << " " << Arg;


    if(Command=="INIT")
    {

        DoINIT(Arg);

    }
    if(Command=="START")
    {
        DoNewDownload(Arg);

    }
    if(Command=="DOWN")
    {
        DoUpload(Arg);

    }


}




void MyThread::DoINIT(QByteArray Arg)
{

    QString init="INIT damitha\n";
    qDebug()<<"Command: " <<init.toLatin1();
    socket->write(init.toLatin1());
    socket->waitForBytesWritten(5000);

}

void MyThread::DoNewDownload(QByteArray Arg)
{



        QByteArray fileName = "";
        QByteArray File = "";

        if(Arg.contains("FNEnd"))
        {
            //Contains arquments
            int pos = Arg.indexOf("FNEnd",0);
            fileName = Arg.mid(0,pos).trimmed();
            File = Arg.mid(pos + 5);          
        }


        QString filepath=downloadFilePath;
        filepath.append(fileName);
        //qDebug()<<filepath;

        newfile= new QFile(filepath);
        if(newfile->exists())
        {
            newfile->remove();

        }
        if(!newfile->open(QIODevice::Append))
        {

            qDebug()<<"error Cant open the file";
        }
        else
        {
            if(File.endsWith("End\n"))
            {

                 qDebug()<<"Download Started";

                 newfile->write(File.mid(0,(File.length()-4)));
                 newfile->close();
                 DownloadStrted=0;

                 qDebug()<<"Download Done";
            }
            else {
               qDebug()<<"Download Started";

               newfile->write(File);
               DownloadStrted=1;
            }




        }


}

void MyThread::DoDownload(QByteArray Arg)
{
    if(Arg.endsWith("End\n"))
    {
        newfile->write(Arg.mid(0,(Arg.length()-4)));
        newfile->close();
        DownloadStrted =0;

        qDebug()<<"Download Done";


    }
    else
    {
        newfile->write(Arg);


    }

}

void MyThread::DoUpload(QByteArray Arg)
{
    QFile newfile(uploadFilePath.append(Arg).trimmed());
    if(newfile.exists())
    {

       // qDebug()<<Arg.append(studentIndex);
        QString startUpload="DOWNSTART ";
        startUpload.append(Arg.trimmed());
        startUpload.append("FNEnd");

        socket->write(startUpload.toLatin1());
        qDebug()<<"Server Command "<<startUpload;

        newfile.open(QIODevice::ReadOnly);
        while (!newfile.atEnd())
        {
            QByteArray buffer = newfile.read(1024 * 8);
            socket->write(buffer);
            socket->waitForBytesWritten(5000);
        }
        newfile.close();
        socket->write("End\n");
        socket->waitForBytesWritten(5000);

         qDebug()<<"Upload Done";

    }
    else
    {
        qDebug()<<"its in do upload file dose not exists";

    }


}




















