#include "mainwindow.h"

MainWindow::~MainWindow()
{
    delete title;
    delete choice_login;
    delete choice_signup;
    delete choice_upload;
    delete choice_download;
    delete hlayout_title;
    delete hlayout_choices;
    delete mainlayout;
    delete centralWidget;//堆栈窗口子部件如welcome无需delete
    //if(widget_welcome != nullptr)delete widget_welcome;
}
WidgetBase::~WidgetBase()
{
    delete btn_back;
    delete line_ip;
    delete spinbox_port;
    delete label_ip;
    delete label_port;
    delete btn_connection;
    delete hlayout_base;
}
SignUp::~SignUp()
{
    delete label_username;
    delete label_password;
    delete label_confirmPassword;
    delete label_confirmResult;
    delete line_username;
    delete line_password;
    delete line_confirmPassword;
    delete btn_signup;
    delete hlayout_username;
    delete hlayout_password;
    delete hlayout_confirmPassword;
    delete vlayout_signup;
}
LogIn::~LogIn()
{
    delete label_username;
    delete label_password;
    delete line_username;
    delete line_password;
    delete btn_login;

    delete hlayout_username;
    delete hlayout_password;
    delete vlayout_login;
}
FileUpload::~FileUpload()
{
    delete btn_choose_file;
    delete btn_start;
    delete toolButton_logout;
    delete filedialog;
    delete progressBar_upload;
}

FileDownload::~FileDownload()
{
    delete line_search;
    delete btn_search;
    delete toolButton_logout;
    delete table_searchResult;

    delete hlayout_search;
    delete vlayout_file;
}

