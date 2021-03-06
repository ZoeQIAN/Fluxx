#include"qclient.h"
#include <iostream>
#include <QMessageBox>
#include <sstream>

using namespace std;

QClient::QClient(QObject *parent):
    QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
}

QClient::~QClient()
{
    tcpSocket->close();
    delete tcpSocket;
}

bool QClient::connectServer(QString server_ip)
{
    tcpSocket->abort(); //
    tcpSocket->connectToHost(server_ip,PORT);
    if(tcpSocket->waitForConnected())
        return true;
    else
        return false;
}

bool QClient::sendMsg(string s)
{
    QTextStream out(tcpSocket);
    QString str(QString(QString::fromLocal8Bit(s.c_str())));
    out << str;
	return true;
}

string QClient::recvMsg()
{
    QTextStream in(tcpSocket);
    QString str;
    in >> str;
    std::cout << "rcvMsg:" << std::string((const char *)str.toLocal8Bit()) << std::endl;
    return std::string((const char *)str.toLocal8Bit());
}
