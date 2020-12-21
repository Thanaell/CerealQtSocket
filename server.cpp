#include "server.h"

#include <QtWidgets>
#include <QtNetwork>
#include <fstream>


Server::Server(QWidget *parent)
    : QDialog(parent)
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

    QLabel *statusLabel = new QLabel;
    statusLabel->setWordWrap(true);
    statusLabel->setText(tr("The server is running.\n"
                            "Run the Local Fortune Client example now."));

    QPushButton *quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    connect(quitButton, &QPushButton::clicked, this, &Server::close);
    connect(server, &QLocalServer::newConnection, this, &Server::sendMessage);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(statusLabel);
    mainLayout->addLayout(buttonLayout);

    setWindowTitle(QGuiApplication::applicationDisplayName());

    createClient();

}

void Server::createClient(){
    QProcess *myClientProcess = new QProcess();
    myClientProcess->start("client.exe",QStringList());
    myClientProcess->waitForStarted(-1);
    qDebug()<<"Started client"<<myClientProcess->processId();
}

void Server::sendMessage()
{
    QByteArray block;
    std::stringstream ss;
    std::string binary;
    cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
    ComplexStruct myStruct;
    myStruct.mat=cv::Mat::ones(5,8,CV_8UC1);
    myStruct.vec.push_back(std::pair<int,std::optional<float>>{1,2.3});
    myStruct.vec.push_back(std::pair<int,std::optional<float>>{42,55.5});
    myStruct.name="hello";
    myStruct.qName="qHello";
    myStruct.intMap[1]="helloQ";
    myStruct.intValue=2;
    myStruct.myBasicStruct.myInt=5;
    myStruct.myBasicStruct.myMap["hello"]=42;
    myStruct.myBasicStruct.myMap["world"]=12;
    oarchive(myStruct.qName,myStruct.mat,myStruct.vec,myStruct.name,myStruct.intMap,myStruct.intValue,myStruct.myBasicStruct);// Write the data to the archive
    ss >> binary;
    ss.clear();
    block=QByteArray(binary.c_str(), binary.length());


    QLocalSocket *clientConnection = server->nextPendingConnection();
    connect(clientConnection, &QLocalSocket::disconnected,
            clientConnection, &QLocalSocket::deleteLater);
    clientConnection->write(block);
    clientConnection->flush();
    clientConnection->disconnectFromServer();
}
