#include "mainwindow.h"
const QString qss_line = "border-radius: 20px; font-size: 16pt; font-family: KaiTi ; color: black; background-color: white;";
const QString qss_choices = "QToolButton { border: 2px solid black; background-color: black; font-size: 20pt; font-family: KaiTi; border-radius: 20px; color: black; }"
                            "QToolButton:hover { border: 2px solid black; background-color: black; font-size: 24pt; font-family: KaiTi; border-radius: 20px; color: red; }"
                            "QPushButton { border: 2px solid black; background-color: white; font-size: 20pt; font-family: KaiTi; border-radius: 20px; color: black; }"
                            "QPushButton:hover { border: 2px solid black; background-color: black; font-size: 24pt; font-family: KaiTi; border-radius: 20px; color: red; }"
                            "QLabel {border: 2px solid black; background-color: white; font-size: 20pt; font-family: KaiTi; border-radius: 20px; color: black; }"
                            "QPushButton[enable=\"false\"] { color:#909090; font-color:#909090;}";

void MainWindow::WindowUi()
{
    //窗口设置
    this->setWindowTitle("基于QSsl的安全文件传输系统");
    this->setMinimumSize(1140,660);
    this->setMaximumSize(1140,660);
    this->setStyleSheet("border: 2px none black; background-image:url(D:/Pictures/background.jpg);");
    this->setWindowIcon(QIcon(":/client.ico"));
    //内存分配
    widget_welcome = new QWidget(this);
    title = new QLabel("",widget_welcome);
    choice_signup = new QToolButton(widget_welcome);
    choice_login = new QToolButton(widget_welcome);
    choice_upload = new QToolButton(widget_welcome);
    choice_download = new QToolButton(widget_welcome);
    hlayout_title = new QHBoxLayout;
    hlayout_choices = new QHBoxLayout;
    mainlayout = new QVBoxLayout(widget_welcome);//必须得把wideget塞进布局
    centralWidget = new QStackedWidget(this);//必须得new,不然程序异常结束
    //欢迎界面的小部件及其布局设置
    QPixmap pixTitle(":/title.jpg");
    title->setGeometry(50,50,721,395);
    title->setPixmap(pixTitle);
    title->setScaledContents(true);
    hlayout_title->addStretch(1);
    hlayout_title->addWidget(title);
    hlayout_title->addStretch(1);

    choice_login->setText("登录");
    choice_login->setIcon(QIcon(":/login.jpg"));
    choice_login->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    choice_login->setIconSize(QSize(100,100));
    choice_signup->setText("注册");
    choice_signup->setIcon(QIcon(":/sign.jpg"));
    choice_signup->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    choice_signup->setIconSize(QSize(100,110));
    choice_upload->setText("上传文件");
    choice_upload->setIcon(QIcon(":/upload.jpg"));
    choice_upload->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    choice_upload->setIconSize(QSize(120,100));
    choice_download->setText("下载文件");
    choice_download->setIcon(QIcon(":/download.jpg"));
    choice_download->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    choice_download->setIconSize(QSize(120,100));

    choice_upload->setStyleSheet(qss_choices);
    choice_download->setStyleSheet(qss_choices);
    choice_login->setStyleSheet(qss_choices);
    choice_signup->setStyleSheet(qss_choices);
    choice_signup->setFixedSize(200,150);
    choice_login->setFixedSize(200,150);
    choice_upload->setFixedSize(200,150);
    choice_download->setFixedSize(200,150);

    hlayout_choices->addWidget(choice_signup);
    hlayout_choices->addStretch(1);
    hlayout_choices->addWidget(choice_login);
    hlayout_choices->addStretch(1);
    hlayout_choices->addWidget(choice_upload);
    hlayout_choices->addStretch(1);
    hlayout_choices->addWidget(choice_download);
    hlayout_choices->setGeometry(QRect(50,200,1140,400));

    mainlayout->addLayout(hlayout_title);
    mainlayout->addStretch(3);
    mainlayout->addLayout(hlayout_choices);
    mainlayout->addStretch(3);
}
void WidgetBase::BaseUi()
{
    btn_back = new QPushButton("返回主界面",this);
    line_ip = new QLineEdit(this);
    spinbox_port = new QSpinBox(this);
    label_ip = new QLabel("目标IP",this);
    label_port = new QLabel(" 端口号",this);
    btn_connection = new QPushButton("发起连接",this);
    hlayout_base = new QHBoxLayout;

    btn_back->setStyleSheet(qss_choices);
    btn_back->setFixedSize(220,100);
    btn_connection->setStyleSheet(qss_choices);
    btn_connection->setFixedSize(220,100);
    label_ip->setFixedSize(120,53);
    label_port->setFixedSize(140,53);
    line_ip->setFixedSize(200,33);
    spinbox_port->setFixedSize(100,33);
    label_ip->setStyleSheet(qss_choices);
    label_port->setStyleSheet(qss_choices);
    line_ip->setStyleSheet(qss_line);
    spinbox_port->setRange(1024,65535);
    spinbox_port->setStyleSheet(qss_line);
    line_ip->setText("localhost");
    spinbox_port->setValue(8888);

    hlayout_base->addWidget(btn_back);
    hlayout_base->addStretch(1);
    hlayout_base->addWidget(label_ip);
    hlayout_base->addStretch(1);
    hlayout_base->addWidget(line_ip);
    hlayout_base->addStretch(1);
    hlayout_base->addWidget(label_port);
    hlayout_base->addStretch(1);
    hlayout_base->addWidget(spinbox_port);
    hlayout_base->addStretch(1);
    hlayout_base->addWidget(btn_connection);
    hlayout_base->setGeometry(QRect(10,20,1100,100));
}
void SignUp::SignupUi()
{
    label_username = new QLabel("  用户名:   ",this);
    line_username = new QLineEdit(this);
    label_password = new QLabel("   密码:    ",this);
    line_password = new QLineEdit(this);
    label_confirmPassword = new QLabel(" 确认密码:  ",this);
    line_confirmPassword = new QLineEdit(this);
    label_confirmResult = new QLabel("",this);

    btn_signup = new QPushButton("立即注册",this);
    hlayout_username = new QHBoxLayout;
    hlayout_password = new QHBoxLayout;
    hlayout_confirmPassword = new QHBoxLayout;
    vlayout_signup = new QVBoxLayout;

    line_password->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    line_confirmPassword->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    hlayout_username->addWidget(label_username);
    hlayout_username->addStretch(2);
    hlayout_username->addWidget(line_username);
    hlayout_password->addWidget(label_password);
    hlayout_password->addStretch(2);
    hlayout_password->addWidget(line_password);
    hlayout_confirmPassword->addWidget(label_confirmPassword);
    hlayout_confirmPassword->addStretch(2);
    hlayout_confirmPassword->addWidget(line_confirmPassword);
    hlayout_confirmPassword->addStretch(2);

    label_username->setFixedSize(200,53);
    label_password->setFixedSize(200,53);
    label_confirmPassword->setFixedSize(200,53);
    label_confirmResult->setFont(QFont("KaiTi",18));
    line_username->setFixedSize(800,53);
    line_password->setFixedSize(800,53);
    line_confirmPassword->setFixedSize(800,53);

    label_username->setStyleSheet(qss_choices);
    label_password->setStyleSheet(qss_choices);
    label_confirmPassword->setStyleSheet(qss_choices);
    label_confirmResult->setStyleSheet("background-color:transparent");
    line_username->setStyleSheet(qss_line);
    line_password->setStyleSheet(qss_line);
    line_confirmPassword->setStyleSheet(qss_line);
    hlayout_username->setGeometry(QRect(80,140,500,100));
    hlayout_password->setGeometry(QRect(80,250,500,100));
    hlayout_confirmPassword->setGeometry(QRect(80,360,1010,100));
    label_confirmResult->setGeometry(1088,394,30,30);
    //整体纵向布局管理
    vlayout_signup->addLayout(hlayout_username);
    vlayout_signup->addStretch();
    vlayout_signup->addLayout(hlayout_password);
    vlayout_signup->addStretch();
    vlayout_signup->addLayout(hlayout_confirmPassword);
    vlayout_signup->addStretch();
    vlayout_signup->addWidget(btn_signup);
    btn_signup->setGeometry(500,480,200,100);
    btn_signup->setStyleSheet(qss_choices);
    label_confirmResult->hide();
}
void LogIn::LoginUi()
{
    label_username = new QLabel("账号:",this);
    line_username = new QLineEdit(this);
    label_password = new QLabel("密码:",this);
    line_password = new QLineEdit(this);
    btn_login = new QPushButton("立即登录",this);
    hlayout_username = new QHBoxLayout;
    hlayout_password = new QHBoxLayout;
    vlayout_login = new QVBoxLayout;
    //中部账号密码设置
    hlayout_username->addWidget(label_username);
    hlayout_username->addStretch();
    hlayout_username->addWidget(line_username);
    hlayout_password->addWidget(label_password);
    hlayout_password->addStretch();
    hlayout_password->addWidget(line_password);
    line_password->setEchoMode(QLineEdit::Password);
    label_username->setFixedSize(100,53);
    label_password->setFixedSize(100,53);
    line_username->setFixedSize(800,53);
    line_password->setFixedSize(800,53);
    label_username->setStyleSheet(qss_choices);
    label_password->setStyleSheet(qss_choices);
    line_username->setStyleSheet(qss_line);
    line_password->setStyleSheet(qss_line);
    hlayout_username->setGeometry(QRect(80,240,500,100));
    hlayout_password->setGeometry(QRect(80,350,500,100));

    //整体纵向布局管理
    vlayout_login->addLayout(hlayout_username);
    vlayout_login->addStretch();
    vlayout_login->addLayout(hlayout_password);
    vlayout_login->addStretch();
    vlayout_login->addWidget(btn_login);
    btn_login->setGeometry(400,500,200,100);
    btn_login->setStyleSheet(qss_choices);
}
void FileUpload::UploadUi()
{
    btn_connection->hide();
    label_ip->hide();
    label_port->hide();
    line_ip->hide();
    spinbox_port->hide();
    toolButton_logout = new QToolButton(this);
    btn_choose_file = new QPushButton("选择本地文件",this);
    btn_start = new QPushButton("立即上传⬆",this);
    filedialog = new QFileDialog(this);
    progressBar_upload = new QProgressBar(this);
    toolButton_logout->setText("注销登录");
    toolButton_logout->setIcon(QIcon(":/logout.jpg"));
    toolButton_logout->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolButton_logout->setIconSize(QSize(120,100));
    hlayout_base->addWidget(toolButton_logout);

    btn_choose_file->setGeometry(140,200,400,160);
    btn_start->setGeometry(800,240,300,100);
    btn_choose_file->setStyleSheet(qss_choices);
    btn_start->setStyleSheet(qss_choices);
    toolButton_logout->setStyleSheet(qss_choices);
    toolButton_logout->setGeometry(940,10,200,150);

    progressBar_upload->setGeometry(100,400,800,50);
    progressBar_upload->setStyleSheet("border-radius:10px");

}
void FileDownload::DownloadSetupUi()
{
    btn_connection->hide();
    label_ip->hide();
    label_port->hide();
    line_ip->hide();
    spinbox_port->hide();
    //内存分配
    btn_search = new QPushButton("立即搜索",this);
    line_search = new QLineEdit(this);
    toolButton_logout = new QToolButton(this);
    hlayout_search = new QHBoxLayout;
    vlayout_file = new QVBoxLayout;
    table_searchResult = new QTableWidget(this);
    //搜索框、搜索按钮
    line_search->setGeometry(50,140,680,50);
    btn_search->setGeometry(750,140,160,50);
    btn_search->setStyleSheet(qss_choices);
    line_search->setStyleSheet(qss_line);
    toolButton_logout->setText("注销登录");
    toolButton_logout->setIcon(QIcon(":/logout.jpg"));
    toolButton_logout->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolButton_logout->setIconSize(QSize(120,100));
    toolButton_logout->setGeometry(940,10,200,150);
    toolButton_logout->setStyleSheet(qss_choices);
    hlayout_base->addWidget(toolButton_logout);

    hlayout_search->addStretch(2);
    hlayout_search->addWidget(line_search);
    hlayout_search->addStretch(1);
    hlayout_search->addWidget(btn_search);
    hlayout_search->addStretch(2);

    table_searchResult->setColumnCount(2);
    table_searchResult->setGeometry(100,240,600,380);
    table_searchResult->setHorizontalHeaderLabels(QStringList() << "文件名" << "文件大小");
    table_searchResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//水平头设置列
    table_searchResult->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_searchResult->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_searchResult->setColumnWidth(0,240);
    table_searchResult->setColumnWidth(0,120);
    //整体纵向布局管理
    vlayout_file->addLayout(hlayout_search);
    vlayout_file->addStretch(1);
    vlayout_file->addWidget(table_searchResult);
    vlayout_file->addStretch(1);
}
