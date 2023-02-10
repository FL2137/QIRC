#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//qt includes
#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <QThread>
#include <QFile>

//.h includes
#include "TcpHandler.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString serverName;

private:

    TcpHandler *ircTcp;

    Ui::MainWindow *ui;


    QThread tcpThread;

public slots:
    void receivedData(const QString &result);
    void confirmedConnected();
private slots:
    void on_pushButton_clicked();

signals:
    void tcpStart();
    void sendMessage(const QString &message);



};
#endif // MAINWINDOW_H
