#include "QTcpHandler.h"
#include <QTextStream>

QTcpHandler::QTcpHandler(std::shared_ptr<QTcpSocket> sock){
    if(!socket){
        qDebug()<<"initializing new qtcpsocket";
        socket = sock;
        QString addr = "chat.freenode.org";
        socket->connectToHost(addr,6667);
    }
}

bool QTcpHandler::connect(const QString &addr, const int &port){
    QTcpHandler::socket->connectToHost(addr,port);
    if(socket->waitForConnected(3000)){
        return true;
    }
    return false;
}
vector<std::string> QTcpHandler::msg_split(std::string str) {
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
bool QTcpHandler::contains(std::string str, std::string val){
    if (str.find(val) != string::npos)
         return true;
    else return false;
}


bool QTcpHandler::pong(string& msg) {
    static bool connected = false;




    if(msg.find("PING")==std::string::npos) return false;

    string pongsub = msg.substr(msg.find("PING"),msg.length()-1);
    pongsub = pongsub.substr(0,pongsub.find_first_of("\r\n"));
    pongsub = pongsub.substr(pongsub.find_first_of(":")+1,pongsub.length());
    if(!connected){
        msg = "PONG "+pongsub+"\r\nCAP END\r\n";
        connected = true;
    }
    else
        msg = "PONG "+pongsub+"\r\n";


    return true;
}

struct Replay {
    QString server;
    int code;
    QString receiver;
    QString message;
};

void make_replay(const QString &data, Replay &r){
    QTextStream out(stdout);

    QString header = data.section(':',2,2);



    qDebug()<<header;
}

Replay make_replay(const QString &data){
    Replay r;



    return r;
}

QString QTcpHandler::parser(const QString &data){
    Replay r;
    QString result;
    vector<std::string> lines = msg_split(data.toStdString());

    qDebug()<<QString::fromStdString(lines[0]);

    return "";
}




void QTcpHandler::fileLog(const QString &data){
    QFile file("log.txt");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text  | QIODevice::Append)){
        qDebug()<<"ERROR OPENING LOG.TXT";
    }
    else{
        QTextStream out(&file);
        vector<string> lines = msg_split(data.toStdString());
        QString size = QString::fromStdString(std::to_string(lines.size()));

        QString towrite =       "||"+ size +"--------------------------------------\n\r";
        for(auto &line : lines){
            QString val = QString::fromStdString(line);
            make_replay(val);
            towrite+=val;
        }
        towrite+="--------------------------------------\n\r";
        out << towrite << "\n";

        file.close();
    }
}

void QTcpHandler::read(){
    const char *tmp = "CAP LS 302\r\nNICK kuli_fuli\r\nUSER kuli_fuli 0 * https://kiwiirc.com/\r\n";
    socket->write(tmp);


    for(;;){

        socket->waitForReadyRead(3000);
        socket->waitForBytesWritten(30000);

        QString data = socket->read(512);


        std::string ss = data.toStdString();
        fileLog(data);

        if(pong(ss)){
            data = QString::fromStdString(ss);
            qDebug()<<"FROM PONGER: "+data;
            socket->write(data.toStdString().c_str());
        }

        //qDebug()<<data;
        //emit dataReady(data);
        if(QThread::currentThread()->isInterruptionRequested()){
            break;
        }
    }
}

void QTcpHandler::send(const QString &message){
    qDebug()<<"KURWA TO SIE WYSYLA: "+message;
    socket->write(message.toStdString().c_str());
}
