#include <QtWidgets>
#include <QtNetwork>

#include "client.h"
#include <ostream>

Client::Client(QWidget *parent)
    : QDialog(parent),
      getMessageButton(new QPushButton(tr("Get Message"))),
      statusLabel(new QLabel(tr("This examples requires that you run the "
                                "Local Server example as well."))),
      socket(new QLocalSocket(this))
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    statusLabel->setWordWrap(true);

    getMessageButton->setDefault(true);
    QPushButton *quitButton = new QPushButton(tr("Quit"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(getMessageButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(getMessageButton, &QPushButton::clicked,
            this, &Client::requestNewMessage);
    connect(quitButton, &QPushButton::clicked, this, &Client::close);
    connect(socket, &QLocalSocket::readyRead, this, &Client::readMessage);
    connect(socket, &QLocalSocket::errorOccurred, this, &Client::displayError);

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
}

void Client::requestNewMessage()
{
    getMessageButton->setEnabled(false);
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
    ComplexStruct myStruct2;
    iarchive(myStruct2.qName,myStruct2.mat,myStruct2.vec,myStruct2.name,myStruct2.intMap,myStruct2.intValue,myStruct2.myBasicStruct); // Read the data from the archivetom struct
    std::ostringstream oss;

    QString nameString="string name: "+QString::fromStdString(myStruct2.name)+"\n";
    QString intString = "int : "+QString::number(myStruct2.intValue)+"\n";
    QString vecString="vec: \n";
        for (auto element : myStruct2.vec){
            vecString+=QString::number(element.first)+" : "+QString::number(element.second.value())+"\n";
        }
    QString mapString="map : \n";
        for (auto element : myStruct2.intMap){
            mapString+=QString::number(element.first)+" : "+element.second+"\n";
        }
    QString basicStructIntString="Nested struct int : " + QString::number(myStruct2.myBasicStruct.myInt)+"\n";
    QString basicStructMapString ="Nested struct map : \n";
        for (auto element : myStruct2.myBasicStruct.myMap){
            basicStructMapString+=element.first+" : "+QString::number(element.second)+"\n";
        }
    oss << myStruct2.mat;
        QString matString="Mat : "+QString(oss.str().c_str())+"\n";
    statusLabel->setText(nameString+intString+vecString+mapString+basicStructIntString+basicStructMapString+matString);

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

    getMessageButton->setEnabled(true);
}

