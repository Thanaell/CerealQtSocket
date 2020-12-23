#ifndef SERVER_H
#define SERVER_H

#include <QDialog>
#include "dataStructs.h"
#include <QProcess>
#include <QLocalSocket>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QLocalServer;
QT_END_NAMESPACE

class Server : public QDialog
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);
    void createClient();
private slots:
    void sendMessage(int numClient);
    void readMessage();
    void initClientSocket(int numClient);
private:
    const int maxClients=3;
    int nbClients;
    std::map<int, QProcess*> clientProcesses;
    std::map<int, QLocalSocket*> clientSockets;
    std::map<int, ComplexStruct> clientStructs;
    QLabel *statusLabel;
    QLocalServer *server;
    int number;
};

#endif
