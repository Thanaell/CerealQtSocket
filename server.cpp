#include "server.h"
#include <QtWidgets>
#include <QtNetwork>
#include <fstream>

//-----------------------------------------------------
Server::Server(QWidget *parent)
    : QDialog(parent), m_nbClients(0)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_server = new QLocalServer(this);
    if (!m_server->listen("mySocket")) {
        QMessageBox::critical(this, tr("Local Server"),
                              tr("Unable to start the m_server: %1.")
                              .arg(m_server->errorString()));
        close();
        return;
    }

    m_statusLabel = new QLabel;
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setText(tr("The m_server is running.\n"
                            "Run the Local Client example now."));

    QPushButton *quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    connect(quitButton, &QPushButton::clicked, this, &Server::close);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_statusLabel);
    mainLayout->addLayout(buttonLayout);

    setWindowTitle(QGuiApplication::applicationDisplayName());

    for (int i=0; i<m_maxClients; i++){
        mFillStruct(i+1);
        if(mCreateClient()){
            qDebug()<<"started client "+QString::number(m_nbClients);
        }
    }

}

//-----------------------------------------------------
void Server::mFillStruct(int i){
    m_clientStructs[i].mat=cv::Mat(2,5,CV_8UC1,i);
    m_clientStructs[i].vec.push_back(std::pair<int,std::optional<float>>{1,2.3});
    m_clientStructs[i].vec.push_back(std::pair<int,std::optional<float>>{42,55.5});
    m_clientStructs[i].name="hello2";
    m_clientStructs[i].qName="qHello";
    m_clientStructs[i].intMap[1]="helloQ";
    m_clientStructs[i].intValue=i;
    m_clientStructs[i].myBasicStruct.myInt=5;
    m_clientStructs[i].myBasicStruct.myMap["hello"]=42;
    m_clientStructs[i].myBasicStruct.myMap["world"]=12;
}

//Creates a new QProcess using Client.exe and waits for it to connect
//-----------------------------------------------------
bool Server::mCreateClient(){
    QProcess * newClient=new QProcess();
    newClient->start("client.exe",QStringList());

    //non blocking option but loses info of which process has which socket

    //QObject::connect(m_server, &QLocalServer::newConnection,this, &Server::initClientSocket);
    if ( !m_server->waitForNewConnection(60)){ //experimentely, works if delay > 60ms
        newClient->kill();
        qDebug()<<"client could not connect";
        return false;
    }
    m_nbClients++;
    m_clientProcesses[m_nbClients]=newClient;
    initClientSocket(m_nbClients);
    return true;

}


//inits QLocalSocket between server and client numClient
//-----------------------------------------------------
void Server::initClientSocket(int numClient){
    m_clientSockets[numClient] = m_server->nextPendingConnection();
    connect( m_clientSockets[numClient], &QLocalSocket::disconnected,
             m_clientSockets[numClient], &QLocalSocket::deleteLater);
    sendMessage(numClient);
    QObject::connect( m_clientSockets[numClient], &QLocalSocket::readyRead, this, &Server::readMessage);
}

//reads Socket then disconnects client
//-----------------------------------------------------
void Server::readMessage(){
    int i;
    for (auto elem: m_clientSockets){
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
    m_statusLabel->setText(m_statusLabel->text()+"\n"+"Answer from client "+ QString::number(i)+" "+ QString::number(myStruct2.intValue));
    static_cast<QLocalSocket*>(sender())->disconnectFromServer(); //disconnects client after it answered
}

//sends message onto socket
//-----------------------------------------------------
void Server::sendMessage(int numClient)
{
    QByteArray block;
    std::stringstream ss;
    std::string binary;
    cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
    ComplexStruct myStruct=m_clientStructs[numClient];
    oarchive(myStruct.qName,myStruct.mat,myStruct.vec,myStruct.name,myStruct.intMap,myStruct.intValue,myStruct.myBasicStruct);// Write the data to the archive
    ss >> binary;
    ss.clear();
    block=QByteArray(binary.c_str(), binary.length());

    m_clientSockets[numClient]->write(block);
    m_clientSockets[numClient]->flush();

}
