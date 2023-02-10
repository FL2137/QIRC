#include "mainwindow.h"
#include "./ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ircTcp = new TcpHandler();
    ircTcp->moveToThread(&tcpThread);


    connect(this,&MainWindow::tcpStart,ircTcp,&TcpHandler::startLoop);
    connect(ircTcp,&TcpHandler::confirmConnected,this,&MainWindow::confirmedConnected);
    connect(this,&MainWindow::sendMessage,ircTcp,&TcpHandler::asyncWrite,Qt::DirectConnection);
    connect(ircTcp,&TcpHandler::updateData,this,&MainWindow::receivedData);

    tcpThread.start();

    emit tcpStart();

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{

    tcpThread.requestInterruption();
    tcpThread.quit();
    tcpThread.wait();

    delete ircTcp;
    delete ui;
}



void MainWindow::receivedData(const QString &data){
    qDebug()<<data;
    QString code=data.left(2);

    if(code == "01"){ //write proposed channels to textArea
        QString tmpTxt = ui->textBrowser->toPlainText();
        tmpTxt+=data.mid(2,data.size()-2);
        ui->textBrowser->setPlainText(tmpTxt);
    }
    else if(code == "02"){ //handle nicknames
        const QStringList nicknames = data.split(' ');
        for(const auto &nick : nicknames){
            ui->listWidget->addItem(nick);
        }
    }



}
void MainWindow::confirmedConnected(){
    ui->serverLabel->setText("chat.freenode.net");
}

void MainWindow::on_pushButton_clicked()
{
    QString txt = ui->typeInput->toPlainText();
    emit sendMessage(txt);

}
