#include "mythread.h"
#include "ftplistitem.h"

MyThread::MyThread(int ID,QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
    DownloadStrted =0;
    basicFilePath="D:/dk work/Motarola/BinduStudent/Student/";
    filepath ="D:/dk work/Motarola/BinduStudent/Administration/Admin.xml";

    studentIndex =toGetStudentUserName();
}

void MyThread::run()
{
    //thered start hear
    qDebug()<<socketDescriptor<<"starting therad in Server";
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()),Qt::DirectConnection);
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()),Qt::DirectConnection);
    connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)),Qt::DirectConnection);
    qDebug()<<socketDescriptor<<"cliant connected to Server";


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
    qDebug()<<socketDescriptor<<"Client Disconnected";
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

    qDebug() << "Client Replay: " << Command;


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

    //QString init="INIT damitha\n";
    QString init="INIT ";
    init.append(studentIndex);
    init.append("\n");
    socket->write(init.toLatin1());
    socket->waitForBytesWritten(5000);
    qDebug()<< "Server Reply :"<<init;

}

void MyThread::DoNewDownload(QByteArray Arg)
{



        QByteArray fileDiscription = "";
        QByteArray File = "";

        if(Arg.contains("FNEnd"))
        {
            //Contains arquments
            int pos = Arg.indexOf("FNEnd",0);
            fileDiscription = Arg.mid(0,pos).trimmed();
            File = Arg.mid(pos + 5);

            QString fileInfo =QString::fromUtf8(fileDiscription.data(), fileDiscription.size());
            QStringList discriptionlist=fileInfo.split("/");
            subject=discriptionlist.at(0);
            type=discriptionlist.at(1);
            fileName=discriptionlist.at(2);
            toEditAdminXml(subject,type,fileName);
            //qDebug()<<subject<<type<<fileName<<"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSS";
        }


        //creating directry
       QString createFileCrationPath=basicFilePath;
       createFileCrationPath.append(subject);
        if(QDir(createFileCrationPath).exists())
        {
            createFileCrationPath.append("/");
            createFileCrationPath.append(type);

            if(QDir(createFileCrationPath).exists())
            {



            }
            else
            {
                QDir().mkdir(createFileCrationPath);

            }

        }
        else
        {
            QDir().mkdir(createFileCrationPath);
            createFileCrationPath.append("/");
            createFileCrationPath.append(type);
            if(QDir(createFileCrationPath).exists())
            {

            }
            else
            {
                QDir().mkdir(createFileCrationPath);
            }


        }





        createFileCrationPath.append("/");
        createFileCrationPath.append(fileName);

        newfile= new QFile(createFileCrationPath);
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
                 //to give aknowlagement
                 //QString sendResponeDone="DownloadDone damitha\n";
                 QString sendResponeDone="DownloadDone ";
                 sendResponeDone.append(studentIndex);
                 sendResponeDone.append("\n");
                 socket->write(sendResponeDone.toLatin1());
                 socket->waitForBytesWritten(5000);
                 qDebug()<< "Server Reply :"<<sendResponeDone;
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

        // to give acknoladement
       //QString sendResponeDone="DownloadDone damitha\n";
        QString sendResponeDone="DownloadDone ";
        sendResponeDone.append(studentIndex);
        sendResponeDone.append("\n");
        socket->write(sendResponeDone.toLatin1());
        socket->waitForBytesWritten(5000);
        qDebug()<< "Server Reply :"<<sendResponeDone;


    }
    else
    {
        newfile->write(Arg);


    }

}

void MyThread::DoUpload(QByteArray Arg)
{
   QString creatingfilepath= basicFilePath;
   creatingfilepath.append(QString::fromUtf8(Arg.trimmed().data(), Arg.trimmed().size()));
   //creatingfilepath.append("-damitha.xml");
   creatingfilepath.append("-");
   creatingfilepath.append(studentIndex);
   creatingfilepath.append(".xml");


   qDebug()<<Arg.append(creatingfilepath)<<"craing filepath";

    QFile newfile(creatingfilepath);
    if(newfile.exists())
    {

       // qDebug()<<Arg.append(studentIndex);
        QString startUpload="DOWNSTART ";
        //startUpload.append(Arg.trimmed());
        startUpload.append(creatingfilepath.mid(creatingfilepath.lastIndexOf("/")+1));
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

void MyThread::toEditAdminXml(QString subject,QString type,QString fileName)
{
    //qDebug()<<subject<<type<<fileName<<"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSS";
    if(!subject.isEmpty())
    {
        QFile newConfigFile(filepath);
        if(!newConfigFile.open(QFile::ReadWrite| QIODevice::Text))
        {

        }
        else
        {
            QDomDocument document;
            QDomElement root;


            if(newConfigFile.size()==0)
            {
                root = document.createElement("Administration");
                QDomElement newSubject= document.createElement("Subject");
                newSubject.setAttribute("SubjectName",subject);

                QDomElement newHomeWorkOrNote=document.createElement(type);
                newHomeWorkOrNote.setAttribute(type+"Name",fileName.mid(0,fileName.lastIndexOf(".")));
                newSubject.appendChild(newHomeWorkOrNote);

                root.appendChild(newSubject);


            }
            else
            {
                document.setContent(&newConfigFile);
                root= document.firstChildElement();


                QDomNodeList subjectList = root.elementsByTagName("Subject");


                int x=0;
                for(int i=0;i<subjectList.count();i++)
                {

                    QDomNode itemNode = subjectList.at(i);

                    if(itemNode.isElement())
                    {
                        if(itemNode.toElement().attribute("SubjectName")==subject)
                        {
                            x=1;
                            QDomElement newSubjectNode=itemNode.toElement();

                            QDomElement newHomeWorkOrNote=document.createElement(type);
                            newHomeWorkOrNote.setAttribute(type+"Name",fileName.mid(0,fileName.lastIndexOf(".")));
                            newSubjectNode.appendChild(newHomeWorkOrNote);
                            itemNode.toElement().appendChild(newHomeWorkOrNote);
                            root.replaceChild(newSubjectNode,itemNode);

                        }

                    }
                }

                if(x==0)
                {
                    QDomElement newSubject= document.createElement("Subject");
                    newSubject.setAttribute("SubjectName",subject);

                    QDomElement newHomeWorkOrNote=document.createElement(type);
                    newHomeWorkOrNote.setAttribute(type+"Name",fileName.mid(0,fileName.lastIndexOf(".")));
                    newSubject.appendChild(newHomeWorkOrNote);
                    root.appendChild(newSubject);

                }








            }

            document.appendChild(root);
            newConfigFile.close();

            if(!newConfigFile.open(QFile::ReadWrite|QIODevice::Truncate | QIODevice::Text))
            {

            }
            else
            {
                QTextStream stream(&newConfigFile);
                stream <<document.toString();
                newConfigFile.close();

           }



     }
  }

}

QString MyThread::toGetStudentUserName()
{
    QFile newConfigFile(filepath);
         if(!newConfigFile.open(QFile::ReadWrite| QIODevice::Text))
         {

         }
         else
         {
             QDomDocument document;
             QDomElement root;



                 document.setContent(&newConfigFile);
                 root= document.firstChildElement();


                 QDomNodeList student= root.elementsByTagName("Student");
                 return student.at(0).toElement().attribute("StudenttName");








         }

}






















