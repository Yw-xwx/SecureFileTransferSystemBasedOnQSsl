#ifndef SSL_SERVER_H
#define SSL_SERVER_H
#include <QtNetwork>

class SSL_server: public QTcpServer
{
    Q_OBJECT
public:
    SSL_server(quint16 port);
    QSslSocket* getSSLSocket(){return serverSocket;};
protected:
    void incomingConnection(qintptr socketDescriptor) override;
signals:
    void SSLOver();
    void disconnection();
private slots:
    void ready();
private:
    void load_certificate();
    QSslSocket *serverSocket = new QSslSocket(this);
};
#endif // SSL_SERVER_H
