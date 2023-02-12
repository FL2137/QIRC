#include "tcphandler.h"

TcpHandler::TcpHandler(QObject *parent): QObject{parent} {


}

TcpHandler::~TcpHandler(){
    socket->close();
    delete socket;
}

void TcpHandler::startLoop()
{
    loop();
}

void fileLog(const QString &line){
    QFile logfile("log.txt");
    if(!logfile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
        qDebug()<<"error opening log file";
        return;
    }

    logfile.write(line.toStdString().c_str());

    logfile.close();
}


bool TcpHandler::connectToIRC(){
    socket = new QTcpSocket();
    socket->connectToHost(ADDRESS,PORT);
    if(socket->waitForConnected(3500)){
        qDebug()<<"Connected to server";
        return true;
    }
    else {
        qDebug()<<"Connecting to server FAILED";
        return false;

    }

}


void TcpHandler::write(const QString &message){
    socket->write(message.toStdString().c_str());
}



QString TcpHandler::read(){
    socket->waitForReadyRead(3000);
    return socket->readAll();
}


QString getPingData(QString line){
    QString pingData = "";
    int pingStart = line.indexOf("PING")+5;
    for(int i=pingStart; ;i++){
        if(line[i] != '\r'){
            pingData+=line[i];
        }
        else break;
    }
    return pingData;
}
#include <vector>
using namespace std;
vector<std::string> msg_split(std::string str) {
    vector<std::string> result = {};
    int j = 0;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '\r' && str[i + 1] == '\n') {
            result.push_back(str.substr(j, i-2));
            j = i+2;
        }
    }
    return result;
}

[[nodiscard]]
QStringList lineMessage(const QString &message){

    QStringList list = {};
    QString line = "";
    for(int i=0;i<message.size();i++){
        if(message[i] == '\n'){
            line+=message[i];
            list.push_back(line);
            line = "";

        }
        else{
            line+=message[i];

        }
    }
    list.push_back(line);
    return list;
}

QString findPong(const QString &line){

    if(line.contains("PING") == false)
        return "";

    QString pongData = "";
    pongData = getPingData(line);

    return QString("PONG "+pongData+"\r\n");
}


QString substr(const QString &str,QChar start, QChar end, int nOfStart=1, int nOfEnd=1){
    auto istart = str.indexOf(start);
    auto iend = str.indexOf(end);
    QString result = "";
    for(int i=istart;i<=iend;i++){
        result+=str[i];
    }
    qDebug()<<"RESULT; "<<result;
    return result;
}

void TcpHandler::parse(QString &data){


    auto list = lineMessage(data);

    for(auto &obj : list){


        //handling ping pong
        QString pongData = findPong(obj);
        if(pongData != ""){
            if(!connected){
                pongData+="CAP END\r\n";
            }
            qDebug()<<"P: "<<pongData;
            write(pongData);
        }
        //end ping pong


        if(obj.contains("001")){ //handle successfull login
            connected=true;
            emit confirmConnected();
        }   
        //regex.setPattern()
        else if(obj.contains(QRegularExpression("(332|335|372|001|002)"))){ //handle welcome messages
            int indx=-1;
            for(int i=0;i<obj.size();i++){
                if(obj[i]==':' && indx == -1)
                    indx = 0;

                else if(obj[i]==':' && indx == 0){
                    indx=i;
                    break;
                }
            }
            emit updateData("01"+obj.mid(indx+1,obj.size()-indx));
        }
        else if(obj.contains("353")){
            QStringList nicknames = {};
            auto str = obj.section(':',2,2);
            emit updateData("02"+str);
        }
        else if(obj.contains("QUIT")){
            QString who = obj.section(':',0,0);
            qDebug()<<who;
        }

        //default print
        if(obj !="")
        qDebug()<<"L:"<<obj;
    }




}


void TcpHandler::loop(){
    connectToIRC();


    write("CAP LS 302\r\nNICK kuli_fuli\r\nUSER kuli_fuli 0 * https://kiwiirc.com/\r\n");
    for(;;){

        QString readData = read();
        parse(readData);

        if(shouldWrite){
            write(readData);
            shouldWrite = false;
        }
        if(QThread::currentThread()->isInterruptionRequested()){
            break;
        }
    }
}


void TcpHandler::asyncWrite(const QString &message){
    qDebug()<<"SENDING: "<<message;
    write(message+"\r\n");
}







