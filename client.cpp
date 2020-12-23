#include <QtWidgets>
#include <QtNetwork>

#include "client.h"
#include <ostream>

Client::Client(QWidget *parent)
    : QDialog(parent),
      statusLabel(new QLabel(tr("This examples requires that you run the "
                                "Local Server example as well."))),
      socket(new QLocalSocket(this)),
      nbMessage(0)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    statusLabel->setWordWrap(true);

    QPushButton *quitButton = new QPushButton(tr("Quit"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(this, &Client::structRead, this, &Client::handleReceivedStruct);
    connect(quitButton, &QPushButton::clicked, this, &Client::close);
    connect(socket, &QLocalSocket::readyRead, this, &Client::readMessage);
    connect(socket, &QLocalSocket::errorOccurred, this, &Client::displayError);

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    requestNewMessage();
}

void Client::handleReceivedStruct(){
    operateOnStruct();
    sendMessage();
}

void Client::requestNewMessage()
{
    blockSize = 0;
    socket->abort();
    socket->connectToServer("mySocket");
}

void Client::readMessage()
{
    /*if (blockSize == 0) {
        // Relies on the fact that QDataStream serializes a quint32 into
        // sizeof(quint32) bytes
        if (socket->bytesAvailable() < (int)sizeof(quint32))
            return;
        in >> blockSize;
    }
    if (socket->bytesAvailable() < blockSize || in.atEnd())
        return;
        */
    std::stringstream ss2;
    QByteArray inBytes = socket->readAll();
    std::string outBinaryString;
    outBinaryString=std::string(inBytes.constData(), inBytes.length());
    ss2 << outBinaryString;
    cereal::BinaryInputArchive iarchive(ss2); // Create an input archive
    iarchive(myStruct.qName,myStruct.mat,myStruct.vec,myStruct.name,myStruct.intMap,myStruct.intValue,myStruct.myBasicStruct); // Read the data from the archivetom struct
    std::ostringstream oss;

    QString nameString="string name: "+QString::fromStdString(myStruct.name)+"\n";
    QString intString = "int : "+QString::number(myStruct.intValue)+"\n";
    QString vecString="vec: \n";
        for (auto element : myStruct.vec){
            vecString+=QString::number(element.first)+" : "+QString::number(element.second.value())+"\n";
        }
    QString mapString="map : \n";
        for (auto element : myStruct.intMap){
            mapString+=QString::number(element.first)+" : "+element.second+"\n";
        }
    QString basicStructIntString="Nested struct int : " + QString::number(myStruct.myBasicStruct.myInt)+"\n";
    QString basicStructMapString ="Nested struct map : \n";
        for (auto element : myStruct.myBasicStruct.myMap){
            basicStructMapString+=element.first+" : "+QString::number(element.second)+"\n";
        }
    oss << myStruct.mat;
        QString matString="Mat : "+QString(oss.str().c_str())+"\n";
    statusLabel->setText(nameString+intString+vecString+mapString+basicStructIntString+basicStructMapString+matString);

    emit structRead();

}

void Client::operateOnStruct(){
    myStruct.intValue++;
}
void Client::sendMessage(){
    QByteArray block;
    std::stringstream ss;
    std::string binary;
    cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
    oarchive(myStruct.qName,myStruct.mat,myStruct.vec,myStruct.name,myStruct.intMap,myStruct.intValue,myStruct.myBasicStruct);// Write the data to the archive
    ss >> binary;
    ss.clear();
    block=QByteArray(binary.c_str(), binary.length());

    socket->write(block);
    socket->flush();
}

void Client::displayError(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError) {
    case QLocalSocket::ServerNotFoundError:
        QMessageBox::information(this, tr("Local Client"),
                                 tr("The host was not found. Please make sure "
                                    "that the server is running and that the "
                                    "server name is correct."));
        break;
    case QLocalSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Local Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the server name "
                                    "is correct."));
        break;
    case QLocalSocket::PeerClosedError:
        break;
    default:
        QMessageBox::information(this, tr("Local Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(socket->errorString()));
    }

}

