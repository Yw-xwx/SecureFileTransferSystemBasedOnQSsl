#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "define.h"
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QLineEdit>
#include <QTcpSocket>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslKey>
#include <QSslError>
#include <QSslSocket>
#include <QSslCipher>
#include <QStackedWidget>
#include <QLabel>
#include <QMessageBox>
#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QFileDialog>
#include <QProgressBar>
#include <QTimer>
#include <QToolButton>//端口号加减滚动按钮
#include <QStyle>
#include <QSpinBox>
#include <QList>
#include <QListWidget>
#include <QStringList>
#include <QDataStream>
#include <QPixmap>
#include <QThread>
#include <QTableWidget>
#include <QHeaderView>
#include <QVector>

QT_REQUIRE_CONFIG(ssl);
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void WindowUi();
private:
    QWidget *widget_welcome;
    QLabel *title;
    QToolButton *choice_signup;
    QToolButton *choice_login;
    QToolButton *choice_download;
    QToolButton *choice_upload;
    QHBoxLayout *hlayout_title;//实现居中
    QHBoxLayout *hlayout_choices;
    QVBoxLayout *mainlayout;
    QStackedWidget *centralWidget;
    QLabel *backgroundPicture;
};

class WidgetBase: public QWidget
{
    Q_OBJECT
public:
    WidgetBase();
    ~WidgetBase();
    void BaseUi();
    void fileDataRead();

    QPushButton *btn_back;//返回主界面按钮
    QLineEdit *line_ip;//服务器IP地址输入框
    QSpinBox *spinbox_port;//服务器程序的开放端口号
    QLabel *label_ip;
    QLabel *label_port;
    QPushButton *btn_connection;//发起连接按钮
    QHBoxLayout *hlayout_base;
    QString welcomeUserName;

signals:
    void switchPage(int pageIndex);
};
class LogIn: public WidgetBase
{
    Q_OBJECT
public:
    LogIn();
    ~LogIn();
    void LoginUi();
    QLabel *label_username;
    QLineEdit *line_username;
    QLabel *label_password;
    QLineEdit *line_password;
    QPushButton *btn_login;

    QHBoxLayout *hlayout_username;
    QHBoxLayout *hlayout_password;
    QVBoxLayout *vlayout_login;
};
class SignUp:public WidgetBase
{
    Q_OBJECT
public:
    SignUp();
    ~SignUp();
    void SignupUi();

    QLabel *label_username;//用户名,即登录的账户
    QLabel *label_password;
    QLabel *label_confirmPassword;//二次确认密码
    QLabel *label_confirmResult;//确认密码和原密码是否相同的比较结果
    QLineEdit *line_username;
    QLineEdit *line_password;
    QLineEdit *line_confirmPassword;//用户名、密码、确认密码的输入框
    QPushButton *btn_signup;//点击注册按钮

    QHBoxLayout *hlayout_username;
    QHBoxLayout *hlayout_password;
    QHBoxLayout *hlayout_confirmPassword;
    QVBoxLayout *vlayout_signup;
public slots:
    void confirmPassword(QPixmap ok,QPixmap nok,QPixmap blank);
};
class FileUpload:public WidgetBase
{
    Q_OBJECT
public:
    FileUpload();
    ~FileUpload();
    void UploadUi();
    QPushButton *btn_choose_file;
    QPushButton *btn_start;
    QFileDialog *filedialog;//文件对话框
    QToolButton *toolButton_logout;
    QProgressBar *progressBar_upload;//文件上传进度条
private:
    QString filePath = "", fileName = "", fileHash = "";
    qint64 fileSize = 0;
    QByteArray dataTotal;
};

class FileDownload: public WidgetBase
{
    Q_OBJECT
public:
    FileDownload();
    ~FileDownload();
    void DownloadSetupUi();

    QProgressBar *progressBar_download;//进度条
    QPushButton *btn_search;//搜索文件按钮
    QLineEdit *line_search;//搜索文本框
    QToolButton *toolButton_logout;
    QTableWidget *table_searchResult;

    QHBoxLayout *hlayout_search;//管理搜索控件
    QVBoxLayout *vlayout_file;
};

#endif // MAINWINDOW_H
