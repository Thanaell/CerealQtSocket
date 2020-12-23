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

    statusLabel = new QLabel;
    statusLabel->setWordWrap(true);
    statusLabel->setText(tr("The server is running.\n"
                            "Run the Local Fortune Client example now."));

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

    createClient();

}

void Server::createClient(){
    QProcess *myClientProcess = new QProcess();
    myClientProcess->start("client.exe",QStringList());
    myClientProcess->waitForStarted(-1);
    qDebug()<<"Started client"<<myClientProcess->processId();
    QObject::connect(server, &QLocalServer::newConnection,this, &Server::initClientSocket);

}

void Server::initClientSocket(){
    clientSocket = server->nextPendingConnection();
    connect(clientSocket, &QLocalSocket::disconnected,
            clientSocket, &QLocalSocket::deleteLater);
    sendMessage();
    QObject::connect(clientSocket, &QLocalSocket::readyRead, this, &Server::readMessage);
}

void Server::readMessage(){
    std::stringstream ss2;
    QByteArray inBytes = clientSocket->readAll();
    std::string outBinaryString;
    outBinaryString=std::string(inBytes.constData(), inBytes.length());
    ss2 << outBinaryString;
    cereal::BinaryInputArchive iarchive(ss2); // Create an input archive
    ComplexStruct myStruct2;
    iarchive(myStruct2.qName,myStruct2.mat,myStruct2.vec,myStruct2.name,myStruct2.intMap,myStruct2.intValue,myStruct2.myBasicStruct); // Read the data from the archivetom struct
    std::ostringstream oss;
    statusLabel->setText("read message from client "+ QString::number(myStruct2.intValue));
    clientSocket->disconnectFromServer(); //disconnects client after it answered
}

void Server::sendMessage()
{
    QByteArray block;
    std::stringstream ss;
    std::string binary;
    cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
    ComplexStruct myStruct;
    myStruct.mat=cv::Mat::ones(2,5,CV_8UC1);
    myStruct.vec.push_back(std::pair<int,std::optional<float>>{1,2.3});
    myStruct.vec.push_back(std::pair<int,std::optional<float>>{42,55.5});
    myStruct.name="hello2";
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

    clientSocket->write(block);
    clientSocket->flush();

}
