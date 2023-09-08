#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ssl_server.h"
#include <QMainWindow>
#include <QtNetwork>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QSslKey>
#include <QSslCertificate>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSslSocket>
#include <QPushButton>
#include <QtSql/QSqlDatabase>
#include <QMessageBox>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QCryptographicHash>
#include <QDateTime>
#include <QTextBrowser>
#include <QThread>
#include <QThreadPool>

enum msgType{
    DOWNLOADRESULT,//0
    SIGNUP,//1
    LOGIN,//2
    FILEUPLOAD,//3
    FILEDOWNLOAD,//4
    SEARCH,//5
    LOGOUT,//6
};
const QString arrType[10] = {"Connected","SignUp","LogIn","FileInfo","FileData","Search","LogOut"};

namespace Ui {class MainWindow;}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void signUp();
    void logIn();
    bool fileWrite(QByteArray data);//写入文件数据,返回上传成功与否的结果
    int uploadNecessity();
    bool hashCheck(QByteArray data);//校验文件完整性
    int searchFile(QString targetFile);
    void fileInfoRead();
    void fileDataRead();
    void prepareNextData();
private:
    Ui::MainWindow *ui;
    SSL_server server;
    QSslSocket *socket;
    QSqlQuery query_getUserID;
    qint64 fileSize,bytesReceived;
    QString hashString = "",fileSavedPath = "",fileName = "",onlineUser="";
    int fileID = 0;//未获取到文件ID,默认0
    QFile targetSavedFile;
};

#endif // MAINWINDOW_H
