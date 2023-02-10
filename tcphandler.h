#ifndef TCPHANDLER_H
#define TCPHANDLER_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QThread>
#include <QSocketNotifier>
#include <QFile>
#include <QRegularExpression>


class TcpHandler : public QObject
{
    Q_OBJECT
public:
    explicit TcpHandler(QObject *parent = nullptr);
    ~TcpHandler();
signals:


public:

public slots:
    void startLoop();
    void asyncWrite(const QString &message);

signals:
    void confirmConnected();
    void updateData(const QString &data);

private:

    const QString ADDRESS = "chat.freenode.net";
    const int PORT = 6667;

    bool shouldWrite = false;

    bool connected = false;

    QTcpSocket *socket = nullptr;

    void write(const QString &message);
    QString read();

    void parse(QString &data);

    void loop();

    bool connectToIRC();

};

#endif // TCPHANDLER_H
