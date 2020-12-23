#include "server.h"

#include <QtWidgets>
#include <QtNetwork>
#include <fstream>


Server::Server(QWidget *parent)
    : QDialog(parent), nbClients(0)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    server = new QLocalServer(this);
    if (!server->listen("mySocket")) {
        QMessageBox::critical(this, tr("Local Server"),
                              tr("Unable to start the server: %1.")
                              .arg(server->errorString()));
        close();
        return;
    }

    statusLabel = new QLabel;
    statusLabel->setWordWrap(true);
    statusLabel->setText(tr("The server is running.\n"
                            "Run the Local Client example now."));

    QPushButton *quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    connect(quitButton, &QPushButton::clicked, this, &Server::close);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(statusLabel);
    mainLayout->addLayout(buttonLayout);

    setWindowTitle(QGuiApplication::applicationDisplayName());

    for (int i=0; i<maxClients; i++){
        fillStruct(i+1);
        createClient();
    }

}
void Server::fillStruct(int i){
    clientStructs[i].mat=cv::Mat::ones(2,5,CV_8UC1);
    clientStructs[i].vec.push_back(std::pair<int,std::optional<float>>{1,2.3});
    clientStructs[i].vec.push_back(std::pair<int,std::optional<float>>{42,55.5});
    clientStructs[i].name="hello2";
    clientStructs[i].qName="qHello";
    clientStructs[i].intMap[1]="helloQ";
    clientStructs[i].intValue=i;
    clientStructs[i].myBasicStruct.myInt=5;
    clientStructs[i].myBasicStruct.myMap["hello"]=42;
    clientStructs[i].myBasicStruct.myMap["world"]=12;
}

void Server::createClient(){
    nbClients++;
    clientProcesses[nbClients]=new QProcess();
    clientProcesses[nbClients]->start("client.exe",QStringList());
    clientProcesses[nbClients]->waitForStarted(-1);
    qDebug()<<"Started client"<<nbClients;
    //QObject::connect(server, &QLocalServer::newConnection,this, &Server::initClientSocket); //non blocking but loses info of which process has which socket
    server->waitForNewConnection(-1);
    initClientSocket(nbClients);

}

void Server::initClientSocket(int numClient){
    clientSockets[numClient] = server->nextPendingConnection();
    connect( clientSockets[numClient], &QLocalSocket::disconnected,
             clientSockets[numClient], &QLocalSocket::deleteLater);
    sendMessage(numClient);
    QObject::connect( clientSockets[numClient], &QLocalSocket::readyRead, this, &Server::readMessage);
}

void Server::readMessage(){
    int i;
    for (auto elem: clientSockets){
        if (elem.second==sender()){
            i=elem.first;
            break;
        }
    }
    std::stringstream ss2;
    QByteArray inBytes = static_cast<QLocalSocket*>(sender())->readAll();
    std::string outBinaryString;
    outBinaryString=std::string(inBytes.constData(), inBytes.length());
    ss2 << outBinaryString;
    cereal::BinaryInputArchive iarchive(ss2); // Create an input archive
    ComplexStruct myStruct2;
    iarchive(myStruct2.qName,myStruct2.mat,myStruct2.vec,myStruct2.name,myStruct2.intMap,myStruct2.intValue,myStruct2.myBasicStruct); // Read the data from the archivetom struct
    std::ostringstream oss;
    statusLabel->setText(statusLabel->text()+"\n"+"Answer from client "+ QString::number(i)+" "+ QString::number(myStruct2.intValue));
    static_cast<QLocalSocket*>(sender())->disconnectFromServer(); //disconnects client after it answered
}

void Server::sendMessage(int numClient)
{
    QByteArray block;
    std::stringstream ss;
    std::string binary;
    cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
    ComplexStruct myStruct=clientStructs[numClient];
    oarchive(myStruct.qName,myStruct.mat,myStruct.vec,myStruct.name,myStruct.intMap,myStruct.intValue,myStruct.myBasicStruct);// Write the data to the archive
    ss >> binary;
    ss.clear();
    block=QByteArray(binary.c_str(), binary.length());

    clientSockets[numClient]->write(block);
    clientSockets[numClient]->flush();

}
