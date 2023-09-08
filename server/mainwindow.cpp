#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "var.h"

int nextUser = 0;
int flag_login = -1;//0 fail,1 ok
bool flag_login_error = 0;//0 means nameError,1 means passwordError----which means the account exists;
int flag_signup = -1;
QCryptographicHash hashObject(QCryptographicHash::Sha256);
QFile clientLogFile("D:/Desktop/clientLog.txt");
QTextStream logWrite(&clientLogFile);
bool transferFile = false;//标识是否在传输文件
QString timeNow,targetNameBackup;//用户要下载的文件名
QByteArray arraySearchResult = "";
QDataStream streamSearch(&arraySearchResult,QIODevice::ReadWrite);
int lastIndex = 0;//记录上次读取时,数据流对象所在的下标

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(8888)//到ssl_server.cpp内的构造函数去创建对象
{    
    ui->setupUi(this);
    ui->title->setText("等待客户端连接");
    if(!clientLogFile.open(QIODevice::ReadWrite|QIODevice::Append))//append以追加写入
        QMessageBox::warning(this,"失败","日志文件打不开!");
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");//使用QODBC connector
    db.setHostName("127.0.0.1");
    db.setPort(MYSQL_PORT);
    db.setDatabaseName(DATABASE_NAME);//ODBC中设置为连接到file_library数据库
    db.setUserName(MYSQL_NAME);
    db.setPassword(MYSQL_PSWD);
    if(db.open())
        qDebug() << "MySQL service is successfully opened!";
    else
        qDebug() << "Failed to open MySQL,because:\n" << db.lastError().text();
    socket = server.getSSLSocket();
    connect(&server,&SSL_server::SSLOver,[=](){
        if(socket->state() == QAbstractSocket::ConnectedState){
            ui->title->setText(QString("与[%1:%2]建立SSL连接\n采用的加密算法套件是:%3").
                               arg(socket->peerAddress().toString()).arg(socket->peerPort()).arg(socket->sessionCipher().name()));
            logWrite << "--------------------------------" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << ": Connected with \"" << socket->peerAddress().toString() << "\"------------------------------\n";
        }
    });
    connect(&server,&SSL_server::disconnection,[=](){
        ui->title->setText("客户端断开连接");
        socket->deleteLater();
        clientLogFile.close();//客户端断开连接即可读取该日志文件
        ui->line_typeReceived->setText("Disconnected");
        ui->line_accountReceived->clear();
        ui->line_passwordReceived->clear();
        ui->textBrowser->clear();
    });
    connect(socket,&QSslSocket::readyRead,[=](){
        timeNow = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        if(transferFile) {
            fileDataRead();
            return;
        }
        logWrite << timeNow << ':' << ' ';//日志文件首先记录时间
        QDataStream in(socket);//先把套接字的数据读到in数据流
        int type;
        in >> type;
        ui->line_typeReceived->setText(arrType[type]);
        if(type == msgType::FILEUPLOAD) {
            fileInfoRead();//起初下载是false，读取文件信息，读取完更改下载true
            transferFile = true;
        }
        else if(type == msgType::SIGNUP){
            QString name,password;
            in >> name >> password;
            ui->line_accountReceived->setText(name);
            ui->line_passwordReceived->setText(password);//password-receive是用户设置的密码(未加盐)
            signUp();
            QByteArray arrSend;
            QDataStream streamSend(&arrSend,QIODevice::WriteOnly);
            if(flag_signup == 1){
                streamSend << int(SIGNUP) << 1;
                logWrite << "UserName: \"" << name << "\" sign up successfully with the password: \"" << password << "\" and log in automatically.\n";
                onlineUser = name;
            }
            else{
                streamSend << int(SIGNUP) << 0;
                logWrite << "Try to sign up the userName: \"" << name << "\" but fail.\n";
            }
            socket->write(arrSend);
        }
        else if(type == msgType::LOGIN) {
            QString name,password;
            in >> name >> password;
            ui->line_accountReceived->setText(name);
            ui->line_passwordReceived->setText(password);//password-receive是用户设置的密码(未加盐)
            logIn();
            QByteArray arrSend;
            QDataStream streamSend(&arrSend,QIODevice::WriteOnly);
            if(flag_login == 1){
                streamSend << int(LOGIN) << 1;
                logWrite << "\"" << name << "\" log in with right password.\n";
                onlineUser = name;
            }
            else{
                if(flag_login_error == 1){
                    logWrite << "\"" << name << "\" log in with wrong password,which is \"" << password << "\".\n";
                    streamSend << int(LOGIN) << 0;
                }
                else{
                    logWrite << "Non-exist account: \"" << name << "\" fail to log in.\n";
                    streamSend << int(LOGIN) << -1;
                }
            }
            socket->write(arrSend);
        }
        else if(type == msgType::LOGOUT){
            QByteArray arrSend;
            QDataStream streamSend(&arrSend,QIODevice::WriteOnly);
            streamSend << int(LOGOUT);
            logWrite << "\"" << onlineUser << "\" log out successfully.\n";
            ui->line_accountReceived->setText(onlineUser);
            socket->write(arrSend);
            onlineUser = "";
        }
        else if(type == msgType::SEARCH){
            QString searchTarget;
            in >> searchTarget;
            ui->line_accountReceived->setText(searchTarget);
            int countsearchResult = searchFile(searchTarget);
            QByteArray arrSend;
            QDataStream streamSend(&arrSend,QIODevice::WriteOnly);
            streamSend << int(SEARCH) << countsearchResult;
            if(countsearchResult > 0){
                ui->line_passwordReceived->setText("搜索目标文件成功!");
                logWrite << "\"" << onlineUser << "\"search for \"" << searchTarget << "\" successfully,the results are: \n";
                ui->textBrowser->append(QString("-----本次查询有%1个搜索结果,如下:").arg(countsearchResult));
                QString targetName,targetSize;
                streamSearch.device()->seek(lastIndex);
                for(int i = 0;i<countsearchResult;i++){
                    streamSearch >> targetName >> targetSize;
                    ui->textBrowser->append(targetName+' '+targetSize);
                    logWrite << "    " << i+1 << ".\"" << targetName << "\" \"" << targetSize << "\".\n";
                    streamSend << targetName << targetSize;
                }
                lastIndex = streamSearch.device()->pos();
            }
            else{
                ui->line_passwordReceived->setText("什么也没搜到!");
                ui->textBrowser->append("-----本次没有搜到任何文件!");
                logWrite << "\"" << onlineUser << "\" fail to search for \"" << searchTarget << "\".\n";
            }
            socket->write(arrSend);
        }
        else if(type == msgType::FILEDOWNLOAD)
        {
            QString targetName,targetPath;
            in >> targetName;
            targetNameBackup = targetName;
            QSqlQuery query_searchFile;
            query_searchFile.prepare("select filePath from files where fileName =?");
            query_searchFile.addBindValue(targetName);
            query_searchFile.exec();
            while(query_searchFile.next())
                targetPath = query_searchFile.value(0).toString();
            ui->textBrowser->append("用户本次要下载的文件存储在:"+targetPath);
            QFile targetFile(targetPath);
            if(!targetFile.open(QIODevice::ReadOnly))
                ui->textBrowser->append("用户要下载的文件打不开");
            QByteArray targetData = targetFile.readAll();
            qint64 targetFileSize = targetFile.size();
            hashObject.reset();
            hashObject.addData(targetData);//找到文件、准备好计算哈希值、准备发送[下载标识、name、哈希、size]
            QByteArray arraySend;//待发送文件的属性的字节数组
            QDataStream streamSend(&arraySend, QIODevice::WriteOnly);//绑定该字节数组和数据流
            streamSend << int(FILEDOWNLOAD) << QString(targetName) << QString(hashObject.result().toHex()) << qint64(targetFileSize);
            socket->write(arraySend);//发送该字节数组[文件名、文件哈希值、文件大小，三个部分]
            if(!socket->waitForBytesWritten(10*1000)) {//等待发送完成，才能继续下次发送，否则发送过快，对方无法接收
                qDebug() << (QString("网络请求超时,原因:%1").arg(socket->errorString()));
                return;
            }
            qint64 bufferSize = 4096; //发送文件的缓冲=块大小
            qint64 bytesWritten=0;//已经发送的字节数
            double progressByte= 0;//发送进度
            targetFile.seek(0);
            while(bytesWritten != targetFileSize)
            {
                int progress = static_cast<int>(bytesWritten/1.0/targetFileSize*100);//进度百分数分子
                if(bytesWritten<targetFileSize)
                {
                    QByteArray DataInfoBlock = targetFile.read(qMin(targetFileSize,bufferSize));
                    qint64 WriteBolockSize = socket->write(DataInfoBlock, DataInfoBlock.size());
                    QThread::usleep(3); //添加延时，防止服务端发送文件帧过快，否则发送过快，客户端接收不过来，导致丢包
                    if(!socket->waitForBytesWritten(3000))
                    {
                        qDebug() << "请求超时3000";
                        return;
                    }
                    bytesWritten += WriteBolockSize;//更新已发送文件字节数
                }
                if(bytesWritten==targetFileSize)
                {
                    qDebug() << "文件已经全部传输完成!";
                    targetFile.close();
                    return;
                }
                if(bytesWritten > targetFileSize) {qDebug() << "发送的字节数大于文件本身大小";return;}
                if(bytesWritten/1.0/targetFileSize > progressByte) {
                    qDebug() << QString("当前给客户端的文件传输进度：%1/%2 -> %3%").arg(bytesWritten).arg(targetFileSize).arg(progress);
                    progressByte+=0.1;
                }
            }
        }
        else if(type == msgType::DOWNLOADRESULT)
        {
            int result;
            in >> result;
            if(result)
                logWrite << "\"" << onlineUser << "\" download the file: \"" << targetNameBackup << "\" successfully!\n";
            else
                logWrite << "\"" << onlineUser << "\" download the file: \"" << targetNameBackup << "\", but fail.\n";
        }
        else
            qDebug() << "客户端发的什么类型都不是";
    });
}
void MainWindow::fileInfoRead()
{
    QDataStream in(socket);
    qDebug()<<"文件信息读取......";
    // 接收文件大小，数据总大小信息和文件名大小,文件名信息
    in >> fileName >> hashString >> fileSize;
    qDebug() << hashString;
    // 获取文件名，建立文件
    ui->textBrowser->append(QString("下载文件 %1, 文件大小: %2").arg(fileName).arg(fileSize));
    QString filePath = "D:/Desktop/Files/Received/"+fileName;
    targetSavedFile.setFileName(filePath);
    if(!targetSavedFile.open(QIODevice::ReadWrite))
        qDebug()<<"文件打开失败!";
    ui->textBrowser->append("文件属性获取完成————"+fileName + ' '+hashString+ ' '+fileSize);
    bytesReceived = 0;
}
void MainWindow::fileDataRead()
{
    qint64 readBytes = socket->bytesAvailable();
    if(readBytes <0) return;
    // 如果接收的数据大小小于要接收的文件大小，那么继续写入文件
    if(bytesReceived < fileSize) {
        QByteArray data = socket->read(readBytes);
        bytesReceived+=readBytes;
        ui->textBrowser->append(QString("接收进度:%1/%2(字节)").arg(bytesReceived).arg(fileSize));
        targetSavedFile.write(data);
    }
    if (bytesReceived==fileSize){    // 接收数据完成时
        ui->textBrowser->append(QString("已成功接收文件[%1]全部内容,正在校验文件完整性...").arg(fileName));
        targetSavedFile.seek(0);
        QByteArray arrSend;
        QDataStream streamSend(&arrSend,QIODevice::WriteOnly);
        if(hashCheck(targetSavedFile.readAll())){
            if(uploadNecessity() > 0)
                ui->textBrowser->append(QString("文件在数据库内的fileID是%1\n").arg(fileID));
            logWrite << "\"" << onlineUser << "\" transfer the " << fileSize << "B file named " << fileName << " successfully!\n";
            streamSend << int(FILEUPLOAD) << 1;
        }
        else{
            streamSend << int(FILEUPLOAD) << 0;
            logWrite << "\"" << onlineUser << "\" want to transfer the " << fileSize << "B file named " << fileName << " but fail due to the incorrect hash test.\n";
        }
        targetSavedFile.close();
        transferFile = false;
        socket->write(arrSend);
        qDebug() << arrSend.toHex();
        prepareNextData();
    }
    if(bytesReceived > fileSize)
        logWrite << "\"" << onlineUser << "\" transfer the " << fileSize << "B file named " << fileName << " met with \"bytesReceived > fileSize\".\n";
}
void MainWindow::prepareNextData()
{
    fileSize = 0;
    bytesReceived = 0;
    hashString = "";
    fileSavedPath = "";
    fileName = "";
}
void MainWindow::logIn()
{//以"用户密码@用户名!用户ID",连接之后做SHA256摘要
    QSqlQuery query_select;
    query_select.prepare("select userID,userName,userPassword from users where userName = ?");//ODBC用问号占位
    query_select.addBindValue(ui->line_accountReceived->text());//addBindValue必须严格按照问号顺序,而bindValue需要多指出一个占位问号的索引
    flag_login = 0;
    if(query_select.exec() && query_select.next()){
        QString IDfind,nameFind,passwordSaved;//查表得到ID、name、password,加盐给hashString去计算
        IDfind = QString::number(query_select.value(0).toInt());
        nameFind = query_select.value(1).toString();
        passwordSaved = query_select.value(2).toString();
        hashString = ui->line_passwordReceived->text() + '@' + nameFind +'!' + IDfind;
        hashObject.reset();
        hashObject.addData(hashString.toUtf8());
        if(passwordSaved != hashObject.result().toHex()){
            qDebug() << QString("用户%1登录输入的密码错误,实际密码为%2").arg(ui->line_accountReceived->text()).arg(passwordSaved);
            flag_login_error = 1;
        }
        else{
            flag_login = 1;
            qDebug() << QString("%1登录成功").arg(ui->line_accountReceived->text());
        }
    }
    else{
        qDebug() << QString("不存在用户%1").arg(ui->line_accountReceived->text());
        flag_login_error = 0;
    }
}
void MainWindow::signUp()
{
    //当前注册用户被分配的ID
    QSqlQuery query_getUserID;
    query_getUserID.prepare("select MAX(userID) from users");
    if(query_getUserID.exec() && query_getUserID.next())
        nextUser = query_getUserID.value(0).toInt()+1;

    QSqlQuery query_insert;
    query_insert.prepare("insert into users(userID,userName,userPassword) values(?,?,?)");
    query_insert.addBindValue(nextUser);
    query_insert.addBindValue(ui->line_accountReceived->text());//绑定name

    hashString = ui->line_passwordReceived->text() + '@' + ui->line_accountReceived->text() +'!' + QString::number(nextUser);
    hashObject.reset();
    hashObject.addData(hashString.toUtf8());

    query_insert.addBindValue(hashObject.result().toHex());//绑定password
    if(query_insert.exec() == true){
        flag_signup = 1;
        qDebug() << QString("%1注册成功,密码是").arg(ui->line_accountReceived->text()) + hashObject.result().toHex();
        nextUser++;
    }
    else{
        flag_signup = 0;
        qDebug() << query_insert.lastError().text();//输出注册时的错误
    }
}
int MainWindow::uploadNecessity()
{
    QSqlQuery file_select_maxID;
    file_select_maxID.prepare("select MAX(fileID) from files");
    int availableFile = 0;
    if(file_select_maxID.exec() && file_select_maxID.next())
        availableFile = file_select_maxID.value(0).toInt()+1;
    qDebug() << "availableFileID:" << availableFile;//现存最大ID数+1,是空闲的ID
    QSqlQuery file_select_exist;//查找当前文件是否存在于服务器上
    file_select_exist.prepare("select fileID from files where fileHash = ?");
    file_select_exist.addBindValue(hashString);
    if(file_select_exist.exec() && file_select_exist.next())
        return -1;
    fileSavedPath = "D:/Desktop/Files/Received/"+fileName;
    fileID = availableFile;
    QSqlQuery insert_new;
    insert_new.prepare("insert into files(fileID,fileName,filePath,fileBytes,fileHash) values(?,?,?,?,?)");
    insert_new.addBindValue(availableFile++);//ID
    insert_new.addBindValue(fileName);//name
    insert_new.addBindValue(fileSavedPath);//path
    insert_new.addBindValue(fileSize);//bytes
    insert_new.addBindValue(hashString);//Hash
    if(insert_new.exec())
        qDebug() << "用户"+onlineUser+"上传的文件信息录入成功,文件将保存在此路径:"<<fileSavedPath;
    else
        return 0;
    return fileID;//返回刚才录入的文件的ID
}
bool MainWindow::hashCheck(QByteArray data)
{
    hashObject.reset();
    hashObject.addData(data);
    QString result = hashObject.result().toHex();
    if(result == hashString)
        return 1;
    return 0;
}
int MainWindow::searchFile(QString targetFile)
{
    QSqlQuery query_searchFile;
    query_searchFile.prepare("select fileName,fileSize from files where fileName like ?");
    query_searchFile.addBindValue(('%'+targetFile+'%'));//模糊查询
    qDebug() << "用户要查找的目标是:" << targetFile << "查询到的结果如下:";
    int countResult = 0;//记录查询到的结果数量
    if(query_searchFile.exec() && query_searchFile.next())//select查询结果为空时,exec可以执行返回真,只有next返回假
    {
        do
        {
            countResult++;
            qDebug() << query_searchFile.value(0).toString() << query_searchFile.value(1).toString();
            streamSearch << query_searchFile.value(0).toString() << query_searchFile.value(1).toString();
        }while(query_searchFile.next());
    }
    return countResult;
}

MainWindow::~MainWindow()
{
    delete ui;
}
