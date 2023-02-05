#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//qt includes
#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <QThread>
#include <QFile>

//.h includes
#include "QTcpHandler.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow, std::enable_shared_from_this<MainWindow>
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString serverName;

private:
    QTcpHandler *tcpSender;
    QTcpHandler *tcpReader;

    Ui::MainWindow *ui;
    std::shared_ptr<QTcpSocket> socket;
    QThread readerThread;
    QThread senderThread;

public slots:
    void receivedData(const QString &result);
    void confirmedConnected();
private slots:
    void on_pushButton_clicked();

signals:
    void operateSend(const QString &message);
    void operateRead();



};
#endif // MAINWINDOW_H
