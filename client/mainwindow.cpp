#include "mainwindow.h"

QFile certfile(":/localhost.cert");
QFile keyfile(":/localhost.key");
QSslSocket socket;
bool loginStatus = false;//初始未登录状态
QCryptographicHash fileHashObject(QCryptographicHash::Sha256);//用来hash的对象,重复使用要记得reset
QString targetSendFilePath = "",targetDownloadFilePath="";//从btn_choose给到btn_start
QVector<QVector<QString> >targetFileResult;
QTimer timer;
bool transferFile = false;
QFile hisFile;
QString hisHash = "";
qint64 targetDownloadFileSize = 0,bytesReceived = 0;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    WindowUi();
    SignUp *widget_signUp = new SignUp;
    connect(widget_signUp,&SignUp::switchPage,centralWidget,&QStackedWidget::setCurrentIndex);
    LogIn *widget_logIn = new LogIn;
    connect(widget_logIn,&LogIn::switchPage,centralWidget,&QStackedWidget::setCurrentIndex);
    FileUpload *widget_fileUpload = new FileUpload;
    connect(widget_fileUpload,&FileDownload::switchPage,centralWidget,&QStackedWidget::setCurrentIndex);
    FileDownload *widget_fileDownload = new FileDownload;
    connect(widget_fileDownload,&FileDownload::switchPage,centralWidget,&QStackedWidget::setCurrentIndex);

    centralWidget->addWidget(widget_welcome);
    centralWidget->addWidget(widget_signUp);
    centralWidget->addWidget(widget_logIn);
    centralWidget->addWidget(widget_fileUpload);
    centralWidget->addWidget(widget_fileDownload);
    centralWidget->setCurrentIndex(ORIGINAL);
    setCentralWidget(centralWidget);
    connect(choice_signup,&QPushButton::clicked,[=](){
        centralWidget->setCurrentIndex(SIGNUP);
    });
    connect(choice_login,&QPushButton::clicked,[=](){
        centralWidget->setCurrentIndex(LOGIN);
    });
    connect(choice_upload,&QPushButton::clicked,[=](){
        if(loginStatus)
            centralWidget->setCurrentIndex(FILE_UPLOAD);
        else
            QMessageBox::warning(this,"注意!","请先登录再上传文件,\n如果您没有账户,请先注册!");
    });
    connect(choice_download,&QPushButton::clicked,[=](){
        if(loginStatus)
            centralWidget->setCurrentIndex(FILE_DOWNLOAD);
        else
            QMessageBox::warning(this,"注意!","请先登录再下载文件,\n如果您没有账户,请先注册!");
    });
}

WidgetBase::WidgetBase()
{
    BaseUi();
    connect(btn_connection,&QPushButton::clicked,[=](){
        //加载证书和密钥
        certfile.open(QIODevice::ReadOnly);
        keyfile.open(QIODevice::ReadOnly);
        QSslCertificate certificate(&certfile,QSsl::Pem);
        QSslKey key(&keyfile,QSsl::Rsa,QSsl::Pem);
        certfile.close();
        keyfile.close();
        //设置SSL配置
        QSslConfiguration sslConfiguration;
        sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
        sslConfiguration.setLocalCertificate(certificate);
        sslConfiguration.setPrivateKey(key);
        sslConfiguration.setProtocol(QSsl::TlsV1_0OrLater);
        socket.setSslConfiguration(sslConfiguration);
        socket.connectToHostEncrypted(SERVER_ADDRESS,SERVER_PORT);
    });
    connect(&socket,&QSslSocket::encrypted,this,[=](){
        line_ip->setEnabled(false);
        line_ip->setStyleSheet("QLineEdit[enabled=\"false\"] { font-family:Consolas; font-size: 20pt; color: #909090; }");
        spinbox_port->setEnabled(false);
        spinbox_port->setStyleSheet("QLineEdit[enabled=\"false\"] { color: #909090; }");
        btn_connection->setEnabled(false);
    });
    connect(&socket,&QSslSocket::readyRead,this,[=](){
        if(transferFile)
        {
            if(!hisFile.isOpen())
            {
                hisFile.setFileName(targetDownloadFilePath);
                hisFile.open(QIODevice::ReadWrite|QIODevice::Append);
            }
            qint64 readBytes = socket.bytesAvailable();
            if(readBytes <0) return;
            if(bytesReceived < targetDownloadFileSize) {
                QByteArray data = socket.read(readBytes);
                bytesReceived+=readBytes;
                qDebug() << QString("接收进度:%1/%2(字节)").arg(bytesReceived).arg(targetDownloadFileSize);
                hisFile.write(data);
            }
            if (bytesReceived==targetDownloadFileSize)
            {
                qDebug() << "已成功接收文件全部内容,正在校验文件完整性...";
                hisFile.seek(0);
                fileHashObject.reset();
                fileHashObject.addData(hisFile.readAll());
                QString result = fileHashObject.result().toHex();
                QByteArray arrSend;
                QDataStream streamSend(&arrSend,QIODevice::WriteOnly);
                if(result == hisHash)
                {
                    QMessageBox::information(this,"成功!","文件已完成下载并通过完整性检验!");
                    streamSend << ORIGINAL << 1;
                }
                else{
                    QMessageBox::warning(this,"失败","文件下载完成但未通过完整性检验!请稍后重试下载");
                    streamSend << ORIGINAL << 0;
                }
                socket.write(arrSend);
                qDebug() << result << hisHash;
                hisFile.close();
                transferFile = false;
                targetDownloadFileSize = 0;
                bytesReceived = 0;
                hisHash = "";
                targetSendFilePath = "";
            }
            if(bytesReceived > targetDownloadFileSize)
            {
                qDebug() << "下载字节数 > 文件大小";
                transferFile = false;
            }
            return;
        }
        QDataStream streamFromSever(&socket);
        int type;//当前执行的操作和正误结果
        streamFromSever >> type;
        if(type == SIGNUP)
        {
            int result;
            streamFromSever >> result;
            if(result == 1)
            {
                QMessageBox::information(this,"注册成功!","您的账号已经注册成功,即将为您自动登录,请稍候...");
                loginStatus = true;
                emit switchPage(ORIGINAL);
            }
            else
            {
                QMessageBox::warning(this,"注册失败","账户名重复");
                loginStatus = false;
            }
        }
        else if(type == LOGIN)
        {
            int result;
            streamFromSever >> result;
            if(result==1)
            {
                QMessageBox::information(this,"登录成功!","账号密码验证通过,欢迎进入系统!");
                loginStatus = true;
                emit switchPage(ORIGINAL);
            }
            else if (result == 0)
            {
                QMessageBox::warning(this,"登录失败","密码错误,请重新输入!");
                loginStatus = false;
            }
            else
            {
                QMessageBox::warning(this,"登录失败","不存在此账户,请检查用户名");
                loginStatus = false;
            }
        }
        else if(type == FILE_UPLOAD)
        {
            int result;
            streamFromSever >> result;
            if(result)
                QMessageBox::information(this,"上传成功!","文件已成功传输并通过完整性校验!");
            else
                QMessageBox::warning(this,"上传失败","文件完整性校验失败,可能源于网络波动,请稍后重新尝试传输!");
        }
        else if(type == LOGOUT)
        {
            QMessageBox::information(this,"下线成功!","您已成功退出账户!");
            loginStatus = false;
        }
        else if(type == SEARCH)
        {
            int result;
            streamFromSever >> result;
            if(result <= 0)
                QMessageBox::warning(this,"搜索失败","抱歉...没有您搜索的文件哟,换个文件试试吧!");
            else
            {
                QMessageBox::information(this,"搜索成功!",QString("共找到%1个文件").arg(result));
                if(targetFileResult.size()!=0)
                    targetFileResult.clear();
                QString fileName = "",size=0;
                int countSearchResult = 0;
                do
                {
                    streamFromSever >> fileName >> size;//搜索结果是俩字符串
                    if(fileName != ""){
                        qDebug() << fileName << ' ' << size;
                        QVector<QString>temp;
                        temp.append(fileName);
                        temp.append(size);
                        targetFileResult.append(temp);
                        countSearchResult++;
                    }
                    else
                        break;
                }while(1);
                timer.setInterval(200);
                timer.start();
            }
        }
        else if(type == FILE_DOWNLOAD)
        {
            if(!transferFile)
            {
                QDataStream in(&socket);
                qDebug()<<"文件信息读取......";
                QString downloadFileName,downloadFileHash;
                qint64 downloadFileSize;
                // 接收文件大小，数据总大小信息和文件名大小,文件名信息
                in >> downloadFileName >> downloadFileHash >> downloadFileSize;
                // 获取文件名，建立文件
                qDebug() << QString("下载文件: %1, 文件大小: %2字节").arg(downloadFileName).arg(downloadFileSize);
                QString downloadFilePath = "D:/Desktop/Files/Download/"+downloadFileName;
                hisFile.setFileName(downloadFilePath);
                if(!hisFile.open(QIODevice::ReadWrite))
                    qDebug()<<"文件打开失败!";
                bytesReceived = 0;
                hisHash = downloadFileHash;
                targetDownloadFileSize = downloadFileSize;
                transferFile = true;
                qDebug() << downloadFileHash << downloadFileName << downloadFilePath << downloadFileSize;
                qDebug() << hisHash << targetDownloadFileSize;
                targetDownloadFilePath = downloadFilePath;
            }
        }
        else if(type != ORIGINAL)
        {
            qDebug() << "其他数据类型" << type;
            QString temp="1";
            while(temp.size() != 0)
            {    streamFromSever >> temp;
                qDebug() << temp;
            }
        }
    });
}

SignUp::SignUp()
{
    SignupUi();//界面设置
    QPixmap ok(":same.jpg"),nok(":different.jpg"),nothing;
    label_confirmResult->setPixmap(nothing);
    connect(btn_back,&QPushButton::clicked,[&](){
        emit switchPage(ORIGINAL);
        line_username->clear();
        line_password->clear();
        line_confirmPassword->clear();
        label_confirmResult->hide();
    });
    connect(line_password,&QLineEdit::textChanged,[=](){label_confirmResult->hide();});
    connect(line_confirmPassword,&QLineEdit::textChanged,[=](){confirmPassword(ok,nok,nothing);});
    connect(btn_signup,&QPushButton::clicked,[=](){
        if(line_password->text().size() == 0 || line_username->text().size() == 0)
            QMessageBox::warning(this,"注册失败","账户名、密码皆不可为空,\n请检查后重新输入!");
        else
        {
            QPixmap confirm_pixmap = *(label_confirmResult->pixmap());
            if(confirm_pixmap != ok)
                QMessageBox::warning(this,"注册失败","两次输入的密码不一致,请检查后重新输入!");
            else{
                QByteArray arraySend;//socket->write的字节数组
                QDataStream streamSend(&arraySend,QIODevice::WriteOnly);//绑定该socket的二进制数据流
                streamSend << SIGNUP << line_username->text() << line_password->text();
                if(socket.state() == QAbstractSocket::ConnectedState)
                    socket.write(arraySend);
                else
                    QMessageBox::warning(this,"失败","尚未连接到服务器!");
            }
        }
    });
}
LogIn::LogIn()
{
    LoginUi();
    connect(btn_back,&QPushButton::clicked,[=](){
        emit switchPage(ORIGINAL);
        line_username->clear();
        line_password->clear();
    });
    connect(btn_login,&QPushButton::clicked,[=](){
        QByteArray arraySend;//socket->write的字节数组
        QDataStream streamSend(&arraySend,QIODevice::WriteOnly);//绑定该socket的二进制数据流
        streamSend << LOGIN << line_username->text() << line_password->text();
        if(socket.state() == QAbstractSocket::ConnectedState)
            socket.write(arraySend);
        else
            QMessageBox::warning(this,"失败","尚未连接到服务器!");
    });
}
FileUpload::FileUpload()
{
    UploadUi();
    QDataStream in(&socket);
    connect(btn_choose_file,&QPushButton::clicked,[=](){
        filedialog->setFileMode(QFileDialog::ExistingFiles);
        filedialog->setDirectory("D:/Desktop/Files/");// 打开对话框并获取选择的文件路径,默认打开桌面的Files文件夹,运行时可改变
        this->filePath = filedialog->getOpenFileName();
        targetSendFilePath = this->filePath;
        QFile chosenFile(this->filePath);//打开选择的这个文件
        this->fileSize = chosenFile.size();//计算文件的大小、单位是byte
        if(!chosenFile.open(QIODevice::ReadOnly))
            qDebug() << "文件打开失败!";
        else{
            this->dataTotal = chosenFile.read(chosenFile.size());//读取文件全部数据
            chosenFile.close();
        }
        QList<QString> cut_filepath = this->filePath.split('/');
        this->fileName = cut_filepath[cut_filepath.size()-1];//获取文件名
        fileHashObject.reset();
        fileHashObject.addData(this->dataTotal);
        this->fileHash = fileHashObject.result().toHex();//计算数据的 SHA256 哈希值
        if(fileSize!=0)
        {
            qDebug() << "你选择的文件的路径是:" << this->filePath;
            qDebug() << "你选择的文件是:" << this->fileName;
            qDebug() << QString("你选择的文件的大小是:%1字节").arg(this->fileSize);
            qDebug() << "整个文件的哈希值:" << this->fileHash;
            btn_choose_file->setText("点击右侧立即上传⏩");
        }
        else
            qDebug() << "尚未选择任何文件,请重新选择!";
    });
    connect(btn_back,&QPushButton::clicked,[=](){
        emit switchPage(ORIGINAL);
        btn_choose_file->setText("选择本地文件");
    });
    connect(btn_start,&QPushButton::clicked,[=]()
    {
        if(fileSize == 0){
            QMessageBox::warning(this,"失败","尚未选择本地文件!");
            return;
        }//退出当前信号的响应,不执行后续代码
        QByteArray arraySend;//待发送文件的属性的字节数组
        QDataStream streamSend(&arraySend, QIODevice::WriteOnly);//绑定该字节数组和数据流
        streamSend << int(FILE_UPLOAD) << QString(this->fileName) << QString(this->fileHash) << qint64(this->fileSize);
        socket.write(arraySend);//发送该字节数组[文件头标识、文件名、文件哈希值、文件大小，四个部分]
        if(!socket.waitForBytesWritten(10*1000)) {//等待发送完成，才能继续下次发送，否则发送过快，对方无法接收
            qDebug() << (QString("网络请求超时,原因:%1").arg(socket.errorString()));
            return;
        }
        qint64 bufferSize = CHUNK_SIZE; //发送文件的缓冲=块大小
        double progressByte= 0;//发送进度
        qint64 bytesWritten=0;//已经发送的字节数
        QFile targetFile(targetSendFilePath);
        if(!targetFile.open(QIODevice::ReadOnly))
            qDebug() << "待发送的目标文件无法读取";
        while(bytesWritten != fileSize)
        {
            int progress = static_cast<int>(bytesWritten/1.0/fileSize*100);//进度百分数
            if(bytesWritten<fileSize)
            {
                QByteArray DataInfoBlock = targetFile.read(qMin(fileSize,bufferSize));
                qint64 WriteBolockSize = socket.write(DataInfoBlock, DataInfoBlock.size());
                QThread::usleep(3); //添加延时，防止服务端发送文件帧过快，否则发送过快，客户端接收不过来，导致丢包
                if(!socket.waitForBytesWritten(3*1000)) {//等会
                    qDebug() << "网络请求超时";
                    return;
                }
                bytesWritten += WriteBolockSize;//更新已发送文件字节数
                progressBar_upload->setValue(progress);
            }
            if(bytesWritten==fileSize)
            {
                qDebug() << QString("当前上传进度：%1/%2 -> %3%").arg(bytesWritten).arg(fileSize).arg(progress);
                qDebug() << "文件已经全部传输完成!";
                btn_choose_file->setText("选择本地文件");//发送完毕更改文本
                progressBar_upload->setValue(100);
                targetFile.close();
                return;
            }
            if(bytesWritten > fileSize) {qDebug() << "发送的字节数大于文件本身大小";return;}
            if(bytesWritten/1.0/fileSize > progressByte) {
                qDebug() << QString("当前上传进度：%1/%2 -> %3%").arg(bytesWritten).arg(fileSize).arg(progress);
                progressByte+=0.1;
            }
        }
    });
    connect(toolButton_logout,&QPushButton::clicked,[=](){
        QByteArray arraySend;//socket->write的字节数组
        QDataStream streamSend(&arraySend,QIODevice::WriteOnly);//绑定该socket的二进制数据流
        streamSend << int(LOGOUT);
        if(socket.state() == QAbstractSocket::ConnectedState)
            socket.write(arraySend);
        else
            QMessageBox::warning(this,"失败","尚未连接到服务器!");
        loginStatus = false;
        btn_choose_file->setText("选择本地文件");
        emit switchPage(ORIGINAL);
    });
}
FileDownload::FileDownload()
{
    DownloadSetupUi();
    connect(&timer,&QTimer::timeout,[=]()
    {
        table_searchResult->clear();
        table_searchResult->setColumnCount(2);
        table_searchResult->setHorizontalHeaderLabels(QStringList() << "文件名" << "文件大小");
        table_searchResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//水平头设置列
        table_searchResult->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table_searchResult->setSelectionBehavior(QAbstractItemView::SelectRows);
        table_searchResult->setColumnWidth(0,240);
        table_searchResult->setColumnWidth(0,120);
        int count = targetFileResult.size();
        table_searchResult->setRowCount(count);
        if(count != 0)
        {
            for(int i = 0;i<count;i++)
            {
                QString name = targetFileResult[i][0],size = targetFileResult[i][1];
                table_searchResult->setItem(i,0,new QTableWidgetItem(name));
                table_searchResult->setItem(i,1,new QTableWidgetItem(size));
            }
            timer.stop();
        }
    });
    connect(btn_search,&QPushButton::clicked,[=](){
        QByteArray arraySend;//socket->write的字节数组
        QDataStream streamSend(&arraySend,QIODevice::WriteOnly);//绑定该socket的二进制数据流
        streamSend << SEARCH << line_search->text();
        if(socket.state() == QAbstractSocket::ConnectedState)
            socket.write(arraySend);
        else
            QMessageBox::warning(this,"失败","尚未连接到服务器!");
    });
    connect(btn_back,&QPushButton::clicked,[=](){
        emit switchPage(ORIGINAL);
        line_search->clear();
        if(targetFileResult.size()!=0)
            targetFileResult.clear();
        for (int row = 0; row < table_searchResult->rowCount(); ++row){
            for (int col = 0; col < table_searchResult->columnCount(); ++col){
                QTableWidgetItem* item = table_searchResult->item(row, col);
                    if (item)
                        item->setText(""); // 清空单元格的文本内容
            }
        }
    });
    connect(toolButton_logout,&QPushButton::clicked,[=](){
        QByteArray arraySend;//socket->write的字节数组
        QDataStream streamSend(&arraySend,QIODevice::WriteOnly);//绑定该socket的二进制数据流
        streamSend << LOGOUT;
        if(socket.state() == QAbstractSocket::ConnectedState)
            socket.write(arraySend);
        else
            QMessageBox::warning(this,"失败","尚未连接到服务器!");
        loginStatus = false;
        line_search->clear();
        table_searchResult->clear();
        table_searchResult->setColumnCount(2);
        table_searchResult->setHorizontalHeaderLabels(QStringList() << "文件名" << "文件大小");
        table_searchResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//水平头设置列
        table_searchResult->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table_searchResult->setSelectionBehavior(QAbstractItemView::SelectRows);
        table_searchResult->setColumnWidth(0,240);
        table_searchResult->setColumnWidth(0,120);
        emit switchPage(ORIGINAL);
    });
    connect(table_searchResult,&QTableWidget::itemClicked,[=](QTableWidgetItem* item)
    {
       qDebug() << "目标下载文件:" << table_searchResult->item(item->row(),0)->text();
       QByteArray arraySend;//待发送文件的属性的字节数组
       QDataStream streamSend(&arraySend, QIODevice::WriteOnly);//绑定该字节数组和数据流
       streamSend << FILE_DOWNLOAD << table_searchResult->item(item->row(),0)->text();
       socket.write(arraySend);
    });
}
void SignUp::confirmPassword(QPixmap ok,QPixmap nok,QPixmap blank)
{
    if(line_password->text().size()+line_confirmPassword->text().size() == 0){
        label_confirmResult->setPixmap(blank);//确认密码没输入完,label标签显示空图片,即不显示相同与否的判别
        label_confirmResult->hide();
    }
    else if(line_confirmPassword->text().size() >= line_password->text().size()){
        if(line_password->text() != line_confirmPassword->text())
            label_confirmResult->setPixmap(nok);
        else
            label_confirmResult->setPixmap(ok);
        label_confirmResult->setScaledContents(true);//让图片大小适配label对象的尺寸
        label_confirmResult->show();
    }
    else{
        label_confirmResult->setPixmap(blank);//确认密码没输入完,label标签显示空图片,即不显示相同与否的判别
        label_confirmResult->hide();
    }
}
