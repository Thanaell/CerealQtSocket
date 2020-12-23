#include <QtWidgets>
#include <QtNetwork>

#include "client.h"
#include <ostream>

//--------------------------------------------------------------
Client::Client(QWidget *parent)
    : QDialog(parent),
      m_statusLabel(new QLabel(tr("This examples requires that you run the "
                                "Local Server example as well."))),
      m_socket(new QLocalSocket(this))
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    m_statusLabel->setWordWrap(true);

    QPushButton *quitButton = new QPushButton(tr("Quit"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(this, &Client::mStructRead, this, &Client::mHandleReceivedStruct);
    connect(quitButton, &QPushButton::clicked, this, &Client::close);
    connect(m_socket, &QLocalSocket::readyRead, this, &Client::mReadMessage);
    connect(m_socket, &QLocalSocket::errorOccurred, this, &Client::mDisplayError);

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(m_statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    mRequestNewMessage();
}

//does something on struct, then sends struct back to server through socket
//--------------------------------------------------------------
void Client::mHandleReceivedStruct(){
    mOperateOnStruct();
    mSendMessage();
}

//connects to server
//--------------------------------------------------------------
void Client::mRequestNewMessage()
{
    m_socket->abort();
    m_socket->connectToServer("mySocket");
}

//read struct sent by server
//--------------------------------------------------------------
void Client::mReadMessage()
{
    std::stringstream ss2;
    QByteArray inBytes = m_socket->readAll();
    std::string outBinaryString;
    outBinaryString=std::string(inBytes.constData(), inBytes.length());
    ss2 << outBinaryString;
    cereal::BinaryInputArchive iarchive(ss2); // Create an input archive
    iarchive(m_struct.qName,m_struct.mat,m_struct.vec,m_struct.name,m_struct.intMap,m_struct.intValue,m_struct.myBasicStruct); // Read the data from the archivetom struct
    std::ostringstream oss;

    QString nameString="string name: "+QString::fromStdString(m_struct.name)+"\n";
    QString intString = "int : "+QString::number(m_struct.intValue)+"\n";
    QString vecString="vec: \n";
        for (auto element : m_struct.vec){
            vecString+=QString::number(element.first)+" : "+QString::number(element.second.value())+"\n";
        }
    QString mapString="map : \n";
        for (auto element : m_struct.intMap){
            mapString+=QString::number(element.first)+" : "+element.second+"\n";
        }
    QString basicStructIntString="Nested struct int : " + QString::number(m_struct.myBasicStruct.myInt)+"\n";
    QString basicStructMapString ="Nested struct map : \n";
        for (auto element : m_struct.myBasicStruct.myMap){
            basicStructMapString+=element.first+" : "+QString::number(element.second)+"\n";
        }
    oss << m_struct.mat;
        QString matString="Mat : "+QString(oss.str().c_str())+"\n";
    m_statusLabel->setText(nameString+intString+vecString+mapString+basicStructIntString+basicStructMapString+matString);

    emit mStructRead();

}

//does something on struct (for example modifying the cv::Mat)
//--------------------------------------------------------------
void Client::mOperateOnStruct(){
    m_struct.intValue++;
}

//send message to server
//--------------------------------------------------------------
void Client::mSendMessage(){
    QByteArray block;
    std::stringstream ss;
    std::string binary;
    cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
    oarchive(m_struct.qName,m_struct.mat,m_struct.vec,m_struct.name,m_struct.intMap,m_struct.intValue,m_struct.myBasicStruct);// Write the data to the archive
    ss >> binary;
    ss.clear();
    block=QByteArray(binary.c_str(), binary.length());

    m_socket->write(block);
    m_socket->flush();
}

//--------------------------------------------------------------
void Client::mDisplayError(QLocalSocket::LocalSocketError socketError)
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
                                    "Make sure the server is running, "
                                    "and check that the server name "
                                    "is correct."));
        break;
    case QLocalSocket::PeerClosedError:
        break;
    default:
        QMessageBox::information(this, tr("Local Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(m_socket->errorString()));
    }

}

