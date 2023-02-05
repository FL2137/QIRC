#include "mainwindow.h"
#include "./ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    socket = std::make_shared<QTcpSocket>();

    tcpReader = new QTcpHandler(socket);
    tcpSender = new QTcpHandler(socket);

    tcpReader->moveToThread(&readerThread);
    tcpSender->moveToThread(&senderThread);
    connect(&senderThread, &QThread::finished, tcpSender, &QObject::deleteLater);
    connect(&readerThread, &QThread::finished, tcpReader, &QObject::deleteLater);
    connect(this,&MainWindow::operateRead, tcpReader, &QTcpHandler::read);
    connect(this,&MainWindow::operateSend, tcpSender, &QTcpHandler::send);
    connect(tcpReader,&QTcpHandler::dataReady, this, &MainWindow::receivedData);
    connect(tcpReader,&QTcpHandler::connectedToServer,this,&MainWindow::confirmedConnected);

    senderThread.start();
    readerThread.start();
    emit operateRead();

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{

    senderThread.quit();
    senderThread.wait();


    readerThread.requestInterruption();
    readerThread.quit();
    readerThread.wait();


    delete ui;
}


void MainWindow::receivedData(const QString &data){
    if(data != "")
        ui->textBrowser->setText(data);
}
void MainWindow::confirmedConnected(){
    qDebug()<<"CONNECTION CONFIRMED";
}

void MainWindow::on_pushButton_clicked()
{
    QString txt = ui->typeInput->toPlainText();
    emit operateSend(txt);
}

