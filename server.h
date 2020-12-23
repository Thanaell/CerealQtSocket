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

private slots:
    void sendMessage(int numClient);
    void readMessage();
    void initClientSocket(int numClient);
private:
    const int m_maxClients=3;
    int m_nbClients;
    std::map<int, QProcess*> m_clientProcesses;
    std::map<int, QLocalSocket*> m_clientSockets;
    std::map<int, ComplexStruct> m_clientStructs;
    QLabel *m_statusLabel;
    QLocalServer *m_server;
    void mFillStruct(int i);
    bool mCreateClient();
};

#endif
