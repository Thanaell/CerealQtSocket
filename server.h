#ifndef SERVER_H
#define SERVER_H

#include <QDialog>
#include "dataStructs.h"

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
    void sendMessage();
private:
    QLocalServer *server;
    int number;
};

#endif
