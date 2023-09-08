#include "ssl_server.h"

//main函数建立ssl_server对象,调用此文件的SSL_server构造函数
//首先监听对象创建时设置的端口号
//调用load_certificate()加载私钥和证书,检测加密完成的信号
//有新连接进入,调用incomingConnection函数,
SSL_server::SSL_server(quint16 port): QTcpServer()
{
    if (!this->listen(QHostAddress::LocalHost, port))
    {
        qDebug() << "Failed to start listen port" << port;
        this->close();
        return;
    }
    else
        qDebug() << "Start listening on port" << port;
    load_certificate();
    connect(serverSocket, SIGNAL(encrypted()), this, SLOT(ready()));//加密完成调用ready函数,ready内部发射SSLover信号,到mainwindow.cpp内去做SSLover信号的响应
    connect(serverSocket, &QSslSocket::disconnected, this, [=]() {
        qDebug() << "Client disconnected!";
        serverSocket->deleteLater();
        emit disconnection();
    });
}

void SSL_server::load_certificate()
{
    QByteArray key;
    QByteArray cert;
    QFile file_key(":/localhost.key");
    if(file_key.open(QIODevice::ReadOnly))
    {
        key = file_key.readAll();
        file_key.close();
    }
    else
        qDebug() << file_key.errorString();
    QFile file_cert(":/localhost.cert");
    if(file_cert.open(QIODevice::ReadOnly))
    {
        cert = file_cert.readAll();
        file_cert.close();
    }
    else
        qDebug() << file_cert.errorString();
    QSslKey ssl_key(key, QSsl::Rsa,QSsl::Pem,QSsl::PrivateKey,"localhost");
    QSslCertificate ssl_cert(cert);
    serverSocket->setLocalCertificate(ssl_cert);
    serverSocket->setPrivateKey(ssl_key);
    //    serverSocket->setSocketOption(QAbstractSocket::KeepAliveOption, true );
}

void SSL_server::incomingConnection(qintptr socketDescriptor)
{
    if (serverSocket->setSocketDescriptor(socketDescriptor))
    {
        addPendingConnection(serverSocket);
        serverSocket->startServerEncryption();
    }
    else
        serverSocket->close();
}

void SSL_server::ready()
{
    qDebug() << "SSL encrypted!";
    emit SSLOver();
}
