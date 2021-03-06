/*
 * Copyright (C) 2013 ~ 2014 National University of Defense Technology(NUDT) & cdos Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sessiondispatcher.h"
#include <QDebug>
#include <QVariant>
#include <QProcessEnvironment>
#include <QtDBus>
#include <QObject>
#include <QString>
#include "messagedialog.h"
#include "warningdialog.h"
#include <QDesktopWidget>
#include <QDeclarativeContext>
#include <QFontDialog>
#include <QFileDialog>
#include "kthread.h"
#include "wizarddialog.h"
#include "changecitydialog.h"
#include "util.h"
#include "kfontdialog.h"
#include "logindialog.h"
#include "math.h"

QString selectedFont;
QString selectedFcitxFont;

SessionDispatcher::SessionDispatcher(QObject *parent) :
    QObject(parent)
{
    sessioniface = new QDBusInterface("com.cdos.session",
                               "/",
                               "com.cdos.session",
                               QDBusConnection::sessionBus());
    page_num = 0;
    this->mainwindow_width = 850;
    this->mainwindow_height = 600;
    this->alert_width_bg = 329;
    this->alert_width = 329;
    this->alert_height = 195;

    httpauth = new HttpAuth();
    mSettings = new QSettings(YOUKER_COMPANY_SETTING, YOUKER_SETTING_FILE_NAME_SETTING);
    mSettings->setIniCodec("UTF-8");

    //初始化QSetting配置文件
    initConfigFile();

    //超时计时器
    timer=new QTimer(this);
    loginOK = false;

//    skin_widget = new SkinsWidget(mSettings);
//    skinCenter = new SkinCenter();
//    connect(skin_widget, SIGNAL(skinSignalToQML(QString)), this, SLOT(handler_change_skin(QString)));
    //handler_change_titlebar_position
    //QObject::connect(sessioniface, SIGNAL(change_titlebar_position(QString)), this, SLOT(handler_change_titlebar_position(QString)));
    //QObject::connect(sessioniface, SIGNAL(display_scan_process(QString)), this, SLOT(handler_scan_process(QString)));
    //QObject::connect(sessioniface, SIGNAL(scan_complete(QString)), this, SLOT(handler_scan_complete(QString)));
    //QObject::connect(sessioniface, SIGNAL(access_weather(QString, QString)), this, SLOT(accord_flag_access_weather(QString, QString)));
    //QObject::connect(sessioniface, SIGNAL(total_data_transmit(QString, QString)), this, SLOT(handler_total_data_transmit(QString,QString)));

    //Apt and Soft center cache
   // QObject::connect(sessioniface, SIGNAL(data_transmit_by_cache(QString, QString, QString, QString)), this, SLOT(handler_append_cache_data_to_model(QString,QString,QString,QString)));
    //QObject::connect(sessioniface, SIGNAL(cache_transmit_complete(QString)), this, SLOT(handler_cache_scan_over(QString)));
//    QObject::connect(sessioniface, SIGNAL(path_transmit_by_cache(QString, QString)), this, SLOT(handler_cache_path(QString, QString)));

    //Uninstall unneed package and old kernel package
    //QObject::connect(sessioniface, SIGNAL(data_transmit_by_package(QString, QString, QString, QString)), this, SLOT(handler_append_package_data_to_model(QString,QString,QString,QString)));
    //QObject::connect(sessioniface, SIGNAL(package_transmit_complete()), this, SLOT(handler_package_scan_over()));

    //Largest file
//    QObject::connect(sessioniface, SIGNAL(data_transmit_by_large(QString, QString)), this, SLOT(handler_append_largest_file_to_model(QString,QString)));
//    QObject::connect(sessioniface, SIGNAL(large_transmit_complete()), this, SLOT(handler_largest_scan_over()));

    //cookies
    //QObject::connect(sessioniface, SIGNAL(data_transmit_by_cookies(QString, QString, QString)), this, SLOT(handler_append_cookies_to_model(QString,QString,QString)));
    //QObject::connect(sessioniface, SIGNAL(cookies_transmit_complete(QString)), this, SLOT(handler_cookies_scan_over(QString)));

    //cloud conf
    //QObject::connect(sessioniface, SIGNAL(upload_cloud_conf_signal(QString)), this, SLOT(handler_upload_cloud_conf(QString)));
   // QObject::connect(sessioniface, SIGNAL(download_cloud_conf_signal(QString)), this, SLOT(handler_download_cloud_conf(QString)));

    //login
    QObject::connect(httpauth, SIGNAL(response(/*QString,*/QString,QString,QString)), this, SLOT(handle_data_after_login_success(/*QString,*/QString,QString,QString)));
    QObject::connect(httpauth, SIGNAL(refresh(/*QString,*/QString)), this, SLOT(handle_data_after_search_success(/*QString,*/QString)));
    QObject::connect(httpauth, SIGNAL(error(int)), this, SLOT(handle_data_when_login_failed(int)));
    QObject::connect(httpauth, SIGNAL(failedCommunicate()), this, SLOT(resetTimerStatus()));
    QObject::connect(httpauth, SIGNAL(successCommunicate()), this, SLOT(searchCurrentInfo()));
}

SessionDispatcher::~SessionDispatcher() {
    if(loginOK) {
        //退出
        mSettings->beginGroup("user");
        int id = mSettings->value("id").toInt();
        mSettings->endGroup();
        mSettings->sync();
        QString requestData = QString("http://www.cdos.com/boxbeta/find_get.php?pp[type]=logout&pp[table]=yk_member&pp[id]=%1").arg(id);
        QUrl url(requestData);
        httpauth->sendGetRequest(url);
    }
    waitTime = 0;
    disconnect(timer,SIGNAL(timeout()),this,SLOT(connectHttpServer()));
    if(timer->isActive()) {
        timer->stop();
    }

    mSettings->sync();
    if (mSettings != NULL) {
        delete mSettings;
    }

    this->exit_qt();
//    this->ready_exit_normally();
}

//dbus服务退出
void SessionDispatcher::exit_qt() {
    sessioniface->call("exit");
}

void SessionDispatcher::open_folder_qt(QString path) {
    sessioniface->call("open_folder", path);
}

void SessionDispatcher::download_kysoft_cloud_conf_qt() {
    sessioniface->call("download_kysoft_cloud_conf");
}

void SessionDispatcher::upload_kysoft_cloud_conf_qt() {
    sessioniface->call("upload_kysoft_cloud_conf");
}

//接收下载和使用云端配置的信号
void SessionDispatcher::handler_download_cloud_conf(QString download) {
    emit this->tellDownloadCloudConfToQML(download);
}

//接收上传配置到云端时的信号
void SessionDispatcher::handler_upload_cloud_conf(QString upload) {
    emit this->tellUploadCloudConfToQML(upload);
}

//准发发送信号告诉自己去改变自身的标题栏控制按钮位置
void SessionDispatcher::handler_change_titlebar_position(QString position) {
    emit this->startChangeControlBtnPosition(position);
}

//每30minutes连接服务器beat一次
void SessionDispatcher::connectHttpServer(){
    qDebug()<<"start to connect every 30 minutes...";
    mSettings->beginGroup("user");
    int id = mSettings->value("id").toInt();
    mSettings->endGroup();
    mSettings->sync();
    //心跳
    QString requestData = QString("http://www.cdos.com/boxbeta/find_get.php?pp[type]=beat&pp[table]=yk_member&pp[id]=%1").arg(id);
    QUrl url(requestData);
    httpauth->sendGetRequest(url);
}

//beat失败处理，beat不成功，界面的用户信息消失，改为登录界面，提示网络出错
void SessionDispatcher::resetTimerStatus() {
    //主动查询
    QString requestData = QString("http://www.cdos.com/boxbeta/find_get.php?pp[type]=network");
    QUrl url(requestData);
    httpauth->sendGetRequest(url);
}

//查询当前的积分、等级....
void SessionDispatcher::searchCurrentInfo() {
    waitTime = 0;
    mSettings->beginGroup("user");
    int id = mSettings->value("id").toInt();
    mSettings->endGroup();
    mSettings->sync();
    QString requestData = QString("http://www.cdos.com/boxbeta/find_get.php?pp[type]=getall&pp[table]=yk_member&pp[id]=%1").arg(id);
    QUrl url(requestData);
    httpauth->sendGetRequest(url);
}

//显示SliderShow
void SessionDispatcher::show_slider_qt() {
    sessioniface->call("display_slide_show");
}

//程序正常关闭之前，关闭定时器，获取id后发送退出信号给服务端
void SessionDispatcher::ready_exit_normally() {
    //关闭定时器
    waitTime = 0;
    disconnect(timer,SIGNAL(timeout()),this,SLOT(connectHttpServer()));
    if(timer->isActive()) {
        timer->stop();
    }
    //退出
    mSettings->beginGroup("user");
    int id = mSettings->value("id").toInt();
    mSettings->endGroup();
    mSettings->sync();
    QString requestData = QString("http://www.cdos.com/boxbeta/find_get.php?pp[type]=logout&pp[table]=yk_member&pp[id]=%1").arg(id);
    QUrl url(requestData);
    httpauth->sendGetRequest(url);
}

//void SessionDispatcher::handler_write_user_info_when_exit() {//更新数据库数据和本地配置文件
//    this->ready_exit_normally();
//    emit this->ready_to_exit();//通知菜单可以退出程序了
//}

//点击登录框的确定按钮后，开始发送数据给服务端进行登录验证
void SessionDispatcher::verify_user_and_password(QString user, QString pwd) {
    //显示登录动态图
    emit showLoginAnimatedImage();
//    qDebug() << user;
//    qDebug() << pwd;

    //发送数据给服务端进行登录验证
    QString requestData = QString("http://www.cdos.com/boxbeta/find_get.php?pp[type]=login&pp[table]=yk_member&name=%1&password=%2").arg(user).arg(pwd);
    QUrl url(requestData);
    httpauth->sendGetRequest(url);
}

//弹出登录框
void SessionDispatcher::popup_login_dialog(int window_x, int window_y) {
    LoginDialog *logindialog = new LoginDialog();
    QObject::connect(logindialog, SIGNAL(translate_user_password(QString,QString)),this, SLOT(verify_user_and_password(QString,QString)));
    this->alert_x = window_x + (mainwindow_width / 2) - (alert_width_bg  / 2);
    this->alert_y = window_y + mainwindow_height - 400;
    logindialog->move(this->alert_x, this->alert_y);
    logindialog->exec();
}

//退出登录
void SessionDispatcher::logout_cdos_account() {
    this->ready_exit_normally();
}

//用户登录成功后处理数据：显示界面、id写入本地配置、开启定时器
void SessionDispatcher::handle_data_after_login_success(QString id,/* QString level, */QString name, QString score) {
    loginOK = true;
    //登录成功后将用户信息显示在界面上
    bool ok;
    QString level = score_count_level(score.toInt(&ok, 10));
    emit updateLoginStatus(name, level, score);

    //将当前用户id写入本地配置文件中
    mSettings->beginGroup("user");
    mSettings->setValue("id", id);
    mSettings->endGroup();
    mSettings->sync();

    //绑定和初始化定时器，每隔30minutes连接服务器一次
    waitTime = 0;
    connect(timer,SIGNAL(timeout()),this,SLOT(connectHttpServer()));
    timer->start(60000*30);//5000
}

//用户查询成功后处理数据：界面刷新数据
void SessionDispatcher::handle_data_after_search_success(/*QString level, */QString score) {
    //查询成功后将用户信息更新在界面上
    bool ok;
    QString level = score_count_level(score.toInt(&ok, 10));
    emit refreshUserInfo(level, score);
    waitTime = 0;
}

//登录失败时或者测试网络失败，通知QML界面
void SessionDispatcher::handle_data_when_login_failed(int status) {
    if(status == 99) {
        waitTime++;
        if(waitTime >= 4){
            waitTime = 0;
            disconnect(timer, SIGNAL(timeout()), this, SLOT(connectHttpServer()));
            if(timer->isActive()) {
                timer->stop();
            }
            emit loginFailedStatus(99); //超时次数到，向主界面发送网络出现错误的信号
            qDebug()<<"connect fail...";
        }else{
            qDebug() << "continue connect...";
            QString requestData = QString("http://www.cdos.com/boxbeta/find_get.php?pp[type]=network");
            QUrl url(requestData);
            httpauth->sendGetRequest(url);
        }
    }
    else {
        loginOK = false;
        emit loginFailedStatus(status);
    }
}

//根据积分计算用户等级
QString SessionDispatcher::score_count_level(int score) {
    return QString::number(qFloor(sqrt((score - 5) / 30 )));
}

QStringList SessionDispatcher::search_city_names_qt(QString search_name) {
    QDBusReply<QStringList> reply = sessioniface->call("search_city_names", search_name);
    return reply.value();
}

QStringList SessionDispatcher::get_geonameid_list_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_geonameid_list");
    return reply.value();
}

QStringList SessionDispatcher::get_longitude_list_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_longitude_list");
    return reply.value();
}

QStringList SessionDispatcher::get_latitude_list_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_latitude_list");
    return reply.value();
}

QString SessionDispatcher::get_yahoo_city_id_qt(QString geonameid) {
    QDBusReply<QString> reply = sessioniface->call("get_yahoo_city_id", geonameid);
    return reply.value();
}

//更加相应的标记去获取需要的天气数据
void SessionDispatcher::accord_flag_access_weather(QString key, QString value) {
    if(key == "forecast" && value == "kobe") {
        get_forecast_dict_qt();
        emit startUpdateForecastWeahter("forecast");
    }
    else if(key == "weather" && value == "kobe") {
        get_current_weather_dict_qt();
        emit startUpdateForecastWeahter("weather");
    }
    else if(key == "pm25" && value == "kobe") {
        get_pm25_str_qt();
        emit startUpdateForecastWeahter("pm25");
    }
    else if(key == "yahoo" && value == "kobe") {
        get_current_yahoo_weather_dict_qt();
        emit startUpdateForecastWeahter("yahoo");
    }
}

void SessionDispatcher::handler_append_cache_data_to_model(QString flag, QString path, QString fileFlag, QString sizeValue) {
    emit appendContentToCacheModel(flag, path, fileFlag, sizeValue);
}

void SessionDispatcher::handler_cache_scan_over(QString flag) {
    emit tellQMLCaheOver(flag);
}

//void SessionDispatcher::handler_cache_path(QString flag, QString path) {
//    emit tellAbsPathToCacheModel(flag, path);
//}

void SessionDispatcher::handler_append_package_data_to_model(QString flag, QString pkgName, QString description, QString sizeValue) {
    emit appendPackageContentToCacheModel(flag, pkgName, description, sizeValue);
}

void SessionDispatcher::handler_package_scan_over() {
    emit tellQMLPackageOver();
}

void SessionDispatcher::handler_append_largest_file_to_model(QString sizeValue, QString path) {
    emit appendLargestContentToModel(sizeValue, path);
}

void SessionDispatcher::handler_largest_scan_over() {
    emit tellQMLLargestOver();
}

void SessionDispatcher::handler_append_cookies_to_model(QString flag, QString domain, QString num) {
    emit appendCookiesContentToModel(flag, domain, num);
}

void SessionDispatcher::handler_cookies_scan_over(QString cookiesFlag) {
    emit tellQMLCookiesOver(cookiesFlag);
}

void SessionDispatcher::handler_scan_complete(QString msg) {
    emit finishScanWork(msg);
}

void SessionDispatcher::handler_scan_process(QString msg) {
    emit isScanning(msg);
}

void SessionDispatcher::handler_total_data_transmit(QString flag, QString msg) {
    emit tellScanResultToQML(flag, msg);
}

QString SessionDispatcher::get_locale_version() {
    QString locale = QLocale::system().name();
    return locale;
}

void SessionDispatcher::onekey_scan_function_qt(QStringList selectedList) {
    sessioniface->call("onekey_scan_function", selectedList);
}


int SessionDispatcher::scan_history_records_qt(QString flag) {
    QDBusReply<int> reply = sessioniface->call("scan_history_records", flag);
    return reply.value();
}

int SessionDispatcher::scan_system_history_qt() {
    QDBusReply<int> reply = sessioniface->call("scan_system_history");
    return reply.value();
}

//int SessionDispatcher::scan_dash_history_qt() {
//    QDBusReply<int> reply = sessioniface->call("scan_dash_history");
//    return reply.value();
//}

//QStringList SessionDispatcher::scan_of_same_qt(QString abspath) {
//    QDBusReply<QStringList> reply = sessioniface->call("scan_of_same", abspath);
//    return reply.value();
//}

QStringList SessionDispatcher::scan_of_large_qt(int size, QString abspath) {
    QDBusReply<QStringList> reply = sessioniface->call("scan_of_large", size, abspath);
    return reply.value();
}

QStringList SessionDispatcher::scan_cookies_records_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("scan_cookies_records");
    return reply.value();
}

void SessionDispatcher::cookies_scan_function_qt(QString flag) {
    sessioniface->call("cookies_scan_function", flag);
}

QStringList SessionDispatcher::get_cache_arglist(int i) {
    QStringList tmp;
    if(i == 0) {
        tmp << "apt" << "software-center" << "thumbnails";
    }
    else if(i == 4) {
        tmp << "apt" << "software-center";
    }
    else if(i == 5) {
        tmp << "apt" << "thumbnails";
    }
    else if(i == 6) {
        tmp << "software-center" << "thumbnails";
    }
    return tmp;
}

QStringList SessionDispatcher::get_browser_cache_arglist() {
    QStringList tmp;
    tmp << "firefox" << "chromium";
    return tmp;
}

QStringList SessionDispatcher::get_package_arglist(int i) {
    QStringList tmp;
    if(i == 0) {
        tmp << "unneed" << "oldkernel" << "configfile";
    }
    else if(i == 4) {
        tmp << "unneed" << "oldkernel";
    }
    else if(i == 5) {
        tmp << "unneed" << "configfile";
    }
    else if(i == 6) {
        tmp << "oldkernel" << "configfile";
    }
    return tmp;
}

void SessionDispatcher::cache_scan_function_qt(QStringList argList, QString flag) {
    sessioniface->call("cache_scan_function", argList, flag);
}

void SessionDispatcher::package_scan_function_qt(QStringList argList) {
    sessioniface->call("package_scan_function", argList);
}

QString SessionDispatcher::getHomePath() {
    QString homepath = QDir::homePath();
    return homepath;
}

void SessionDispatcher::set_page_num(int num) {
    page_num = num;
}

int SessionDispatcher::get_page_num() {
    return page_num;
}

QString SessionDispatcher::get_session_daemon_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_session_daemon");
    return reply.value();
}

void SessionDispatcher::get_system_message_qt() {
    QDBusReply<QMap<QString, QVariant> > reply = sessioniface->call("get_system_message");
    if (reply.isValid()) {
        QMap<QString, QVariant> value = reply.value();
        systemInfo = value;
        //把当前登录的用户名存放到QSetting配置文件中，方便任务管理器使用
        mSettings->beginGroup("user");
        mSettings->setValue("currentName", systemInfo["currrent_user"].toString());
        mSettings->endGroup();
        mSettings->sync();
    }
    else {
        qDebug() << "get pc_message failed!";
    }
}

//运行时，系统的默认配置写到配置文件
void SessionDispatcher::write_default_configure_to_qsetting_file(QString key, QString name, QString value) {
    mSettings->beginGroup(key);
    mSettings->setValue(name, value);
    mSettings->endGroup();
    mSettings->sync();
}

//从Qsetting配置文件中读取系统启动时的默认配置
QString SessionDispatcher::read_default_configure_from_qsetting_file(QString key, QString name) {
    QString result;
    mSettings->beginGroup(key);
    result = mSettings->value(name).toString();
    mSettings->endGroup();
    mSettings->sync();
    return result;
}

//----------------message dialog--------------------
void SessionDispatcher::showFeatureDialog(int window_x, int window_y) {
    MessageDialog *dialog = new MessageDialog();
    this->alert_x = window_x + (mainwindow_width / 2) - (alert_width  / 2);
    this->alert_y = window_y + mainwindow_height - 400;
    dialog->move(this->alert_x, this->alert_y);
//    dialog->move ((QApplication::desktop()->width() - dialog->width())/2,(QApplication::desktop()->height() - dialog->height())/2);
    dialog->show();
}

//----------------checkscreen dialog--------------------
void SessionDispatcher::showCheckscreenDialog(int window_x, int window_y) {
    ModalDialog *dialog = new ModalDialog;
    this->alert_x = window_x + (mainwindow_width / 2) - (alert_width  / 2);
    this->alert_y = window_y + mainwindow_height - 400;
    dialog->move(this->alert_x, this->alert_y);
    dialog->setModal(true);
    dialog->show();
}
//----------------是否安装--------------------
QString SessionDispatcher::checkInstalled(QString m_pkgname)
{
    QString m_version = "";
    if(m_pkgname != "")
    {
        QString  cmd = "/bin/sh "+ QCoreApplication::applicationDirPath()+"/dpkgwrapper.sh";
        cmd = cmd + " " + m_pkgname;
        QProcess *m_proc = new QProcess;;
        m_proc->start(cmd);
        m_proc->waitForFinished(-1);
        if(m_proc->exitCode() == 0)
        {
            //m_isInstalled= true;
            m_version = m_proc->readAllStandardOutput();
            m_version.chop(1);
        }else{
            //m_isInstalled= false;
            m_version = "";
        }
    }
    else
    {
        //m_isInstalled= false;
        m_version = "";
    }
    return m_version;
}

//----------------启动组件--------------------
void SessionDispatcher::startApplicationsSystem(int type, QString cmdStr) {
    QProcess *process = new QProcess;
    //QString  cmd = "cinnamon-settings";
    if(type == 1){
        cmdStr = "gksu " + cmdStr;
    }
    process->start(cmdStr);
}

void SessionDispatcher::showWarningDialog(QString title, QString content, int window_x, int window_y) {
    WarningDialog *dialog = new WarningDialog(title, content);
    this->alert_x = window_x + (mainwindow_width / 2) - (alert_width  / 2);
    this->alert_y = window_y + mainwindow_height - 400;
    dialog->move(this->alert_x, this->alert_y);
    dialog->exec();
}

bool SessionDispatcher::showConfirmDialog(QString title, QString content, int window_x, int window_y) {
    WarningDialog *dialog = new WarningDialog(title, content);
    this->alert_x = window_x + (mainwindow_width / 2) - (alert_width  / 2);
    this->alert_y = window_y + mainwindow_height - 400;
    dialog->move(this->alert_x, this->alert_y);
    dialog-> QWidget::setAttribute(Qt::WA_DeleteOnClose);
    if(dialog->exec()==QDialog::Rejected) {
        return false;
    }
    else {
        return true;
    }
}

//void SessionDispatcher::handler_confirm_cloud_action() {
//    emit this->tellQMLCloudConfirm();
//}

QString SessionDispatcher::getSingleInfo(QString key) {
    QVariant info = systemInfo.value(key);
    return info.toString();
}

/*-----------------------------desktop of beauty-----------------------------*/
bool SessionDispatcher::set_show_desktop_icons_qt(bool flag) {
    QDBusReply<bool> reply = sessioniface->call("set_show_desktop_icons", flag);
    return reply.value();
}

bool SessionDispatcher::get_show_desktop_icons_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_show_desktop_icons");
    return reply.value();
}

bool SessionDispatcher::set_show_homefolder_qt(bool flag) {
    QDBusReply<bool> reply = sessioniface->call("set_show_homefolder", flag);
    return reply.value();
}

bool SessionDispatcher::get_show_homefolder_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_show_homefolder");
    return reply.value();
}
bool SessionDispatcher::set_show_network_qt(bool flag) {
    QDBusReply<bool> reply = sessioniface->call("set_show_network", flag);
    return reply.value();
}

bool SessionDispatcher::get_show_network_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_show_network");
    return reply.value();
}
bool SessionDispatcher::set_show_trash_qt(bool flag) {
    QDBusReply<bool> reply = sessioniface->call("set_show_trash", flag);
    return reply.value();
}

bool SessionDispatcher::get_show_trash_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_show_trash");
    return reply.value();
}
bool SessionDispatcher::set_show_devices_qt(bool flag) {
    QDBusReply<bool> reply = sessioniface->call("set_show_devices", flag);
    return reply.value();
}

bool SessionDispatcher::get_show_devices_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_show_devices");
    return reply.value();
}

/*-----------------------------unity of beauty-----------------------------*/

int SessionDispatcher::get_default_unity_qt(QString name, QString key) {
    QDBusReply<int> reply = sessioniface->call("get_default_unity", name, key);
    return reply.value();
}

void SessionDispatcher::set_default_unity_qt(QString flag, int value) {
    if(flag == "launchersize") {//launcher图标大小
        sessioniface->call("set_default_unity", "icon-size", "int", value);
    }
    else if(flag == "launcherhide") {//launcher自动隐藏
        sessioniface->call("set_default_unity", "launcher-hide-mode", "int", value);
    }
}

void SessionDispatcher::set_default_launcher_have_showdesktopicon_qt() {
    sessioniface->call("set_default_launcher_have_showdesktopicon");
}

bool SessionDispatcher::set_launcher_autohide_qt(bool flag) {
    QDBusReply<bool> reply = sessioniface->call("set_launcher_autohide", flag);
    return reply.value();
}

bool SessionDispatcher::get_launcher_autohide_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_launcher_autohide");
    return reply.value();
}

bool SessionDispatcher::set_launcher_icon_size_qt(int num) {
    QDBusReply<bool> reply = sessioniface->call("set_launcher_icon_size", num);
    return reply.value();
}

int SessionDispatcher::get_launcher_icon_size_qt() {
    QDBusReply<int> reply = sessioniface->call("get_launcher_icon_size");
    return reply.value();
}
bool SessionDispatcher::set_launcher_have_showdesktopicon_qt(bool flag) {
    QDBusReply<bool> reply = sessioniface->call("set_launcher_have_showdesktopicon", flag);
    return reply.value();
}

bool SessionDispatcher::get_launcher_have_showdesktopicon_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_launcher_have_showdesktopicon");
    return reply.value();
}

/*-----------------------------theme of beauty-----------------------------*/
QStringList SessionDispatcher::get_themes_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_themes");
    return reply.value();
}

QString SessionDispatcher::get_theme_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_theme");
    return reply.value();
}

void SessionDispatcher::set_theme_qt(QString theme) {
    sessioniface->call("set_theme", theme);
}

QStringList SessionDispatcher::get_icon_themes_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_icon_themes");
    return reply.value();
}

QString SessionDispatcher::get_icon_theme_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_icon_theme");
    return reply.value();
}

void SessionDispatcher::set_icon_theme_qt(QString theme) {
    sessioniface->call("set_icon_theme", theme);
}

QStringList SessionDispatcher::get_cursor_themes_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_cursor_themes");
    return reply.value();
}

QString SessionDispatcher::get_cursor_theme_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_cursor_theme");
    return reply.value();
}

void SessionDispatcher::set_cursor_theme_qt(QString theme) {
    sessioniface->call("set_cursor_theme", theme);
}

int SessionDispatcher::get_cursor_size_qt() {
    QDBusReply<int> reply = sessioniface->call("get_cursor_size");
    return reply.value();
}
void SessionDispatcher::set_cursor_size_qt(int size) {
    sessioniface->call("set_cursor_size", size);
}

//window theme
QStringList SessionDispatcher::get_window_themes_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_window_themes");
    return reply.value();
}

QString SessionDispatcher::get_current_window_theme_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_current_window_theme");
    return reply.value();
}

void SessionDispatcher::set_window_theme_qt(QString theme) {
    sessioniface->call("set_window_theme", theme);
}

/*-----------------------------font of beauty-----------------------------*/
QString SessionDispatcher::get_default_theme_sring_qt(QString flag/*QString schema, QString key*/) {
    if(flag == "icontheme") {
        QDBusReply<QString> reply = sessioniface->call("get_default_font_sring", "org.gnome.desktop.interface", "icon-theme");
        return reply.value();
    }
    else if(flag == "windowtheme") {
        QDBusReply<QString> reply = sessioniface->call("get_default_font_sring", "org.gnome.desktop.wm.preferences", "theme");
        return reply.value();
    }
    else if(flag == "mousetheme") {
        QDBusReply<QString> reply = sessioniface->call("get_default_font_sring", "org.gnome.desktop.interface", "cursor-theme");
        return reply.value();
    }
    else if(flag == "smoothstyle") {
        QDBusReply<QString> reply = sessioniface->call("get_default_font_sring", "org.gnome.settings-daemon.plugins.xsettings", "hinting");
        return reply.value();
    }
    else if(flag == "antialiasingstyle") {
        QDBusReply<QString> reply = sessioniface->call("get_default_font_sring", "org.gnome.settings-daemon.plugins.xsettings", "antialiasing");
        return reply.value();
    }
    return flag;
}

double SessionDispatcher::get_default_theme_double_qt(QString schema, QString key) {
    QDBusReply<double> reply = sessioniface->call("get_default_font_double", schema, key);
    return reply.value();
}

void SessionDispatcher::set_default_theme_qt(QString flag/*QString schema, QString key, QString type*/) {
    //-------------------字体-------------------
    if(flag == "defaultfont") {
        sessioniface->call("set_default_font", "org.gnome.desktop.interface", "font-name", "string");
    }
    else if(flag == "desktopfont") {
        sessioniface->call("set_default_font", "org.gnome.nautilus.desktop", "font", "string");
    }
    else if(flag == "monospacefont") {
        sessioniface->call("set_default_font", "org.gnome.desktop.interface", "monospace-font-name", "string");
    }
    else if(flag == "globalfontscaling") {
        sessioniface->call("set_default_font", "org.gnome.desktop.interface", "text-scaling-factor", "double");
    }
    else if(flag == "documentfont") {
        sessioniface->call("set_default_font", "org.gnome.desktop.interface", "document-font-name", "string");
    }
    else if(flag == "titlebarfont") {
        sessioniface->call("set_default_font", "org.gnome.desktop.wm.preferences", "titlebar-font", "string");
    }
    else if(flag == "smoothstyle") {
        sessioniface->call("set_default_font", "org.gnome.settings-daemon.plugins.xsettings", "hinting", "string");
    }
    else if(flag == "antialiasingstyle") {
        sessioniface->call("set_default_font", "org.gnome.settings-daemon.plugins.xsettings", "antialiasing", "string");
    }

    else if(flag == "icontheme") {//图标主题
        sessioniface->call("set_default_font", "org.gnome.desktop.interface", "icon-theme", "string");
    }
    else if(flag == "windowtheme") {//窗口主题
        sessioniface->call("set_default_font", "org.gnome.desktop.wm.preferences", "theme", "string");
    }
    else if(flag == "mousetheme") {//鼠标指针主题
        sessioniface->call("set_default_font", "org.gnome.desktop.interface", "cursor-theme", "string");
    }
    else if(flag == "cursorsize") {//光标大小
        sessioniface->call("set_default_font", "org.gnome.desktop.interface", "cursor-size", "int");
    }
}

bool SessionDispatcher::get_default_desktop_bool_qt(QString schema, QString key) {
    QDBusReply<bool> reply = sessioniface->call("get_default_desktop_bool", schema, key);
    return reply.value();
}

void SessionDispatcher::set_default_desktop_qt(QString flag) {
    if(flag == "showdesktopicons") {//显示桌面图标
        sessioniface->call("set_default_desktop", "org.gnome.desktop.background", "show-desktop-icons", "boolean");
    }
    else if(flag == "homeiconvisible") {//显示主文件夹
        sessioniface->call("set_default_desktop", "org.gnome.nautilus.desktop", "home-icon-visible", "boolean");
    }
    else if(flag == "networkiconvisible") {//显示网络
        sessioniface->call("set_default_desktop", "org.gnome.nautilus.desktop", "network-icon-visible", "boolean");
    }
    else if(flag == "trashiconvisible") {//显示回收站
        sessioniface->call("set_default_desktop", "org.gnome.nautilus.desktop", "trash-icon-visible", "boolean");
    }
    else if(flag == "volumesvisible") {//显示挂载卷标
        sessioniface->call("set_default_desktop", "org.gnome.nautilus.desktop", "volumes-visible", "boolean");
    }
}


QString SessionDispatcher::get_default_sound_string_qt(QString flag/*QString schema, QString key*/) {
    if(flag == "soundtheme") {
        QDBusReply<QString> reply = sessioniface->call("get_default_sound_string", "org.gnome.desktop.sound", "theme-name");
        return reply.value();
    }
    return flag;
}

void SessionDispatcher::set_default_sound_qt(QString flag) {
    if(flag == "soundtheme") {//声音主题
        sessioniface->call("set_default_sound", "org.gnome.desktop.sound", "theme-name", "string");
    }
}


QString SessionDispatcher::get_font_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_font");
    return reply.value();
}

bool SessionDispatcher::set_font_qt_default(QString font) {
    QDBusReply<bool> reply = sessioniface->call("set_font", font);
    return reply.value();
}

bool SessionDispatcher::set_font_qt(QString font) {
    QDBusReply<bool> reply = sessioniface->call("set_font", font);
    return reply.value();
}

QString SessionDispatcher::get_desktop_font_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_desktop_font");
    return reply.value();
}

bool SessionDispatcher::set_desktop_font_qt(QString font) {
    QDBusReply<bool> reply = sessioniface->call("set_desktop_font", font);
    return reply.value();
}

bool SessionDispatcher::set_desktop_font_qt_default() {
    QDBusReply<bool> reply = sessioniface->call("set_desktop_font", "Ubuntu 11");
    return reply.value();
}

QString SessionDispatcher::get_document_font_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_document_font");
    return reply.value();
}

bool SessionDispatcher::set_document_font_qt_default(QString font) {
    QDBusReply<bool> reply = sessioniface->call("set_document_font", font);
    return reply.value();
}

bool SessionDispatcher::set_document_font_qt(QString font) {
    QDBusReply<bool> reply = sessioniface->call("set_document_font", font);
    return reply.value();
}

QString SessionDispatcher::get_monospace_font_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_monospace_font");
    return reply.value();
}

bool SessionDispatcher::set_monospace_font_qt_default(QString font) {
    QDBusReply<bool> reply = sessioniface->call("set_monospace_font", font);
    return reply.value();
}

bool SessionDispatcher::set_monospace_font_qt(QString font) {
    QDBusReply<bool> reply = sessioniface->call("set_monospace_font", font);
    return reply.value();
}

QString SessionDispatcher::get_window_title_font_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_window_title_font");
    return reply.value();
}

bool SessionDispatcher::set_window_title_font_qt_default(QString font) {
    QDBusReply<bool> reply = sessioniface->call("set_window_title_font", font);
    return reply.value();
}

bool SessionDispatcher::set_window_title_font_qt(QString font) {
    QDBusReply<bool> reply = sessioniface->call("set_window_title_font", font);
    return reply.value();
}

double SessionDispatcher::get_font_zoom_qt() {
    QDBusReply<double> reply = sessioniface->call("get_font_zoom");
    return reply.value();
}

bool SessionDispatcher::set_font_zoom_qt(double zoom) {
    QDBusReply<bool> reply = sessioniface->call("set_font_zoom", zoom);
    return reply.value();
}

QStringList SessionDispatcher::get_smooth_style_list_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_smooth_style_list");
    return reply.value();
}

QString SessionDispatcher::get_smooth_style_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_smooth_style");
    return reply.value();
}

bool SessionDispatcher::set_smooth_style_qt(QString style) {
    QDBusReply<bool> reply = sessioniface->call("set_smooth_style", style);
    return reply.value();
}

QStringList SessionDispatcher::get_antialiasing_style_list_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_antialiasing_style_list");
    return reply.value();
}

QString SessionDispatcher::get_antialiasing_style_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_antialiasing_style");
    return reply.value();
}

bool SessionDispatcher::set_antialiasing_style_qt(QString style) {
    QDBusReply<bool> reply = sessioniface->call("set_antialiasing_style", style);
    return reply.value();
}


void SessionDispatcher::restore_default_font_signal(QString flag) {
    emit notifyFontStyleToQML(flag); //font_style
}

QString SessionDispatcher::getSelectedFcitxFont() {
     return selectedFcitxFont;//
}

void SessionDispatcher::show_font_dialog(QString flag) {
    KFontDialog *fontDialog = new KFontDialog(mSettings, flag, 0);
    fontDialog->exec();
    if(!selectedFont.isEmpty()) {
        if(flag == "font") {
            set_font_qt(selectedFont);//set font
        }
        else if(flag == "desktopfont") {
            set_desktop_font_qt(selectedFont);//set desktopfont
        }
        else if(flag == "monospacefont") {
            set_monospace_font_qt(selectedFont);//set monospacefont
        }
        else if(flag == "documentfont") {
            set_document_font_qt(selectedFont);//set documentfont
        }
        else if(flag == "titlebarfont") {
            set_window_title_font_qt(selectedFont);//set titlebarfont
        }
        else if(flag == "fcitxfont") {

        }
        selectedFont.clear();
        emit notifyFontStyleToQML(flag); //font_style
    }
}

QString SessionDispatcher::show_folder_dialog() {
    //选择文件夹
    QString dir = QFileDialog::getExistingDirectory(0, tr("Select folder"), QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    return dir;
}

/*-----------------------------scrollbars of beauty-----------------------------*/
bool SessionDispatcher::set_scrollbars_mode_overlay_qt() {
    QDBusReply<bool> reply = sessioniface->call("set_scrollbars_mode_overlay");
    return reply.value();
}

bool SessionDispatcher::set_scrollbars_mode_legacy_qt() {
    QDBusReply<bool> reply = sessioniface->call("set_scrollbars_mode_legacy");
    return reply.value();
}

QString SessionDispatcher::get_scrollbars_mode_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_scrollbars_mode");
    return reply.value();
}

/*-----------------------------touchpad of beauty-----------------------------*/
QString SessionDispatcher::get_default_system_sring_qt(QString flag) {
    if(flag == "wheel-action") {//菜单项旁显示图标
        QDBusReply<QString> reply = sessioniface->call("get_default_system_sring", "org.compiz.gwd", "mouse-wheel-action");
        return reply.value();
    }
    else if(flag == "double-click") {//标题栏双击动作
        QDBusReply<QString> reply = sessioniface->call("get_default_system_sring", "org.gnome.desktop.wm.preferences", "action-double-click-titlebar");
        return reply.value();
    }
    else if(flag == "middle-click") {//标题栏中键动作
        QDBusReply<QString> reply = sessioniface->call("get_default_system_sring", "org.gnome.desktop.wm.preferences", "action-middle-click-titlebar");
        return reply.value();
    }
    else if(flag == "right-click") {//标题栏右键动作
        QDBusReply<QString> reply = sessioniface->call("get_default_system_sring", "org.gnome.desktop.wm.preferences", "action-right-click-titlebar");
        return reply.value();
    }
    return flag;
}

bool SessionDispatcher::get_default_system_bool_qt(QString schema, QString key) {
    QDBusReply<bool> reply = sessioniface->call("get_default_system_bool", schema, key);
    return reply.value();
}

void SessionDispatcher::set_default_system_qt(QString flag) {
    if(flag == "touchpad-enabled") {//启用禁用触摸板
        sessioniface->call("set_default_system", "org.gnome.settings-daemon.peripherals.touchpad", "touchpad-enabled", "boolean");
    }
    else if(flag == "scrollbar-mode") {//滚动条类型
        sessioniface->call("set_default_system", "com.canonical.desktop.interface", "scrollbar-mode", "string");
    }
    else if(flag == "scroll-method") {//触摸板滚动条触发方式
        sessioniface->call("set_default_system", "org.gnome.settings-daemon.peripherals.touchpad", "scroll-method", "string");
    }
    else if(flag == "horiz-scroll-enabled") {//触摸板横向滚动
        sessioniface->call("set_default_system", "org.gnome.settings-daemon.peripherals.touchpad", "horiz-scroll-enabled", "boolean");
    }
    else if(flag == "control-button-position") {//窗口控制按钮位置
        sessioniface->call("set_default_system", "org.gnome.desktop.wm.preferences", "button-layout", "string");
    }
    else if(flag == "menu-with-icons") {//菜单项旁显示图标
        sessioniface->call("set_default_system", "org.gnome.desktop.interface", "menus-have-icons", "boolean");
    }
    else if(flag == "wheel-action") {//标题栏鼠标滚轮动作
        sessioniface->call("set_default_system", "org.compiz.gwd", "mouse-wheel-action", "string");
    }
    else if(flag == "double-click") {//标题栏双击动作
        sessioniface->call("set_default_system", "org.gnome.desktop.wm.preferences", "action-double-click-titlebar", "string");
    }
    else if(flag == "middle-click") {//标题栏中键动作
        sessioniface->call("set_default_system", "org.gnome.desktop.wm.preferences", "action-middle-click-titlebar", "string");
    }
    else if(flag == "right-click") {//标题栏右键动作
        sessioniface->call("set_default_system", "org.gnome.desktop.wm.preferences", "action-right-click-titlebar", "string");
    }
}



bool SessionDispatcher::set_touchpad_enable_qt(bool flag) {
    QDBusReply<bool> reply = sessioniface->call("set_touchpad_enable", flag);
    return reply.value();
}

bool SessionDispatcher::get_touchpad_enable_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_touchpad_enable");
    return reply.value();
}

bool SessionDispatcher::set_touchscrolling_mode_edge_qt() {
    QDBusReply<bool> reply = sessioniface->call("set_touchscrolling_mode_edge");
    return reply.value();
}

bool SessionDispatcher::set_touchscrolling_mode_twofinger_qt() {
    QDBusReply<bool> reply = sessioniface->call("set_touchscrolling_mode_twofinger");
    return reply.value();
}

QString SessionDispatcher::get_touchscrolling_mode_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_touchscrolling_mode");
    return reply.value();
}

bool SessionDispatcher::set_touchscrolling_use_horizontal_qt(bool flag) {
    QDBusReply<bool> reply = sessioniface->call("set_touchscrolling_use_horizontal", flag);
    return reply.value();
}

bool SessionDispatcher::get_touchscrolling_use_horizontal_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_touchscrolling_use_horizontal");
    return reply.value();
}

/*-----------------------------window of beauty-----------------------------*/
void SessionDispatcher::set_window_button_align_left_qt() {
    sessioniface->call("set_window_button_align_left");
}

void SessionDispatcher::set_window_button_align_right_qt() {
    sessioniface->call("set_window_button_align_right");
}

QString SessionDispatcher::get_window_button_align_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_window_button_align");
    return reply.value();
}

bool SessionDispatcher::set_menus_have_icons_qt(bool flag) {
    QDBusReply<bool> reply = sessioniface->call("set_menus_have_icons", flag);
    return reply.value();
}

bool SessionDispatcher::get_menus_have_icons_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_menus_have_icons");
    return reply.value();
}

QStringList SessionDispatcher::get_titlebar_wheel_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_titlebar_wheel");
    return reply.value();
}

QString SessionDispatcher::get_current_titlebar_wheel_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_current_titlebar_wheel");
    return reply.value();
}

void SessionDispatcher::set_titlebar_wheel_qt(QString value) {
    sessioniface->call("set_titlebar_wheel", value);
}

QStringList SessionDispatcher::get_titlebar_double_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_titlebar_double");
    return reply.value();
}

QString SessionDispatcher::get_current_titlebar_double_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_current_titlebar_double");
    return reply.value();
}

void SessionDispatcher::set_titlebar_double_qt(QString value) {
    sessioniface->call("set_titlebar_double", value);
}

QStringList SessionDispatcher::get_titlebar_middle_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_titlebar_middle");
    return reply.value();
}

QString SessionDispatcher::get_current_titlebar_middle_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_current_titlebar_middle");
    return reply.value();
}

void SessionDispatcher::set_titlebar_middle_qt(QString value) {
    sessioniface->call("set_titlebar_middle", value);
}

QStringList SessionDispatcher::get_titlebar_right_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_titlebar_right");
    return reply.value();
}

QString SessionDispatcher::get_current_titlebar_right_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_current_titlebar_right");
    return reply.value();
}

void SessionDispatcher::set_titlebar_right_qt(QString value) {
    sessioniface->call("set_titlebar_right", value);
}


/*-----------------------------sound of beauty-----------------------------*/
void SessionDispatcher::set_login_music_enable_qt(bool flag) {
    sessioniface->call("set_login_music_enable", flag);
}

bool SessionDispatcher::get_login_music_enable_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_login_music_enable");
    return reply.value();
}

QString SessionDispatcher::get_sound_theme_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_sound_theme");
    return reply.value();
}

void SessionDispatcher::set_sound_theme_qt(QString theme) {
    sessioniface->call("set_sound_theme", theme);
}

/*-------------------filemanager of beauty-------------------*/
//bool SessionDispatcher::get_default_filemanager_bool_qt(QString flag) {

//}

//int SessionDispatcher::get_default_filemanager_int_qt(QString flag) {

//}

void SessionDispatcher::set_default_filemanager_qt(QString flag) {
    if(flag == "pathbar") {//路径输入框取代路径栏
        sessioniface->call("set_default_filemanager", "org.gnome.nautilus.preferences", "always-use-location-entry", "boolean");
    }
    else if(flag == "media") {//自动挂载媒体
        sessioniface->call("set_default_filemanager", "org.gnome.desktop.media-handling", "automount", "boolean");
    }
    else if(flag == "folder") {//自动打开文件夹
        sessioniface->call("set_default_filemanager", "org.gnome.desktop.media-handling", "automount-open", "boolean");
    }
    else if(flag == "programs") {//提示自动运行的程序
        sessioniface->call("set_default_filemanager", "org.gnome.desktop.media-handling", "autorun-never", "boolean");
    }
    else if(flag == "iconsize") {//缩略图图标尺寸（像素）
        sessioniface->call("set_default_filemanager", "org.gnome.nautilus.icon-view", "thumbnail-size", "int");
    }
    else if(flag == "cachetime") {//缩略图缓存时间（天数）
        sessioniface->call("set_default_filemanager", "org.gnome.desktop.thumbnail-cache", "maximum-age", "int");
    }
    else if(flag == "maxsize") {//最大缩略图缓存尺寸（MB）
        sessioniface->call("set_default_filemanager", "org.gnome.desktop.thumbnail-cache", "maximum-size", "int");
    }
}

void SessionDispatcher::set_location_replace_pathbar_qt(bool flag) {
    sessioniface->call("set_location_replace_pathbar", flag);
}

bool SessionDispatcher::get_location_replace_pathbar_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_location_replace_pathbar");
    return reply.value();
}

void SessionDispatcher::set_auto_mount_media_qt(bool flag) {
    sessioniface->call("set_auto_mount_media", flag);
}

bool SessionDispatcher::get_auto_mount_media_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_auto_mount_media");
    return reply.value();
}

void SessionDispatcher::set_auto_open_folder_qt(bool flag) {
    sessioniface->call("set_auto_open_folder", flag);
}

bool SessionDispatcher::get_auto_open_folder_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_auto_open_folder");
    return reply.value();
}

void SessionDispatcher::set_prompt_autorun_programs_qt(bool flag) {
    sessioniface->call("set_prompt_autorun_programs", flag);
}

bool SessionDispatcher::get_prompt_autorun_programs_qt() {
    QDBusReply<bool> reply = sessioniface->call("get_prompt_autorun_programs");
    return reply.value();
}

void SessionDispatcher::set_thumbnail_icon_size_qt(int size) {
    sessioniface->call("set_thumbnail_icon_size", size);
}

int SessionDispatcher::get_thumbnail_icon_size_qt() {
    QDBusReply<int> reply = sessioniface->call("get_thumbnail_icon_size");
    return reply.value();
}

void SessionDispatcher::set_thumbnail_cache_time_qt(int value) {
    sessioniface->call("set_thumbnail_cache_time", value);
}

int SessionDispatcher::get_thumbnail_cache_time_qt() {
    QDBusReply<int> reply = sessioniface->call("get_thumbnail_cache_time");
    return reply.value();
}

void SessionDispatcher::set_thumbnail_cache_size_qt(int size) {
    sessioniface->call("set_thumbnail_cache_size", size);
}

int SessionDispatcher::get_thumbnail_cache_size_qt() {
    QDBusReply<int> reply = sessioniface->call("get_thumbnail_cache_size");
    return reply.value();
}

//-----------------------monitorball------------------------
double SessionDispatcher::get_cpu_percent_qt() {
    QDBusReply<double> reply = sessioniface->call("get_cpu_percent");
    return reply.value();
}

QString SessionDispatcher::get_total_memory_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_total_memory");
    return reply.value();
}

QString SessionDispatcher::get_used_memory_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_used_memory");
    return reply.value();
}

QString SessionDispatcher::get_free_memory_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_free_memory");
    return reply.value();
}

QStringList SessionDispatcher::get_network_flow_total_qt() {
    QDBusReply<QStringList> reply = sessioniface->call("get_network_flow_total");
    return reply.value();
}

//-----------------------change skin------------------------
void SessionDispatcher::handler_change_skin(QString skinName) {
    //将得到的更换皮肤名字写入配置文件中
    mSettings->setValue("skin/background", skinName);
    mSettings->sync();
//    //发送开始更换QML界面皮肤的信号
    emit startChangeQMLSkin(skinName);
}

QString SessionDispatcher::setSkin() {
    QString skinName;
    mSettings->beginGroup("skin");
    skinName = mSettings->value("background").toString();
    if(skinName.isEmpty()) {
        skinName = QString("0_bg");
        mSettings->setValue("background", skinName);
    }
    mSettings->endGroup();
    mSettings->sync();
    return skinName;
}

void SessionDispatcher::showSkinWidget() {
//    skin_widget->show();
}

//void SessionDispatcher::showSkinCenter() {
//    skinCenter->show();
//}

void SessionDispatcher::get_forecast_weahter_qt() {
    getCityIdInfo();

    bool flag = Util::id_exists_in_location_file(initCityId);
    if(flag) {//获取中国气象局数据
        QStringList tmplist;
        tmplist << "Kobe" << "Lee";
        KThread *thread = new KThread(tmplist, sessioniface, "get_forecast_weahter", initCityId);
        thread->start();
    }
    else {
        get_yahoo_forecast_dict_qt();
        emit startUpdateForecastWeahter("yahooforecast");
    }
}

void SessionDispatcher::get_forecast_dict_qt() {
    QDBusReply<QMap<QString, QVariant> > reply = sessioniface->call("get_forecast_dict");
    forecastInfo = reply.value();
}

void SessionDispatcher::get_yahoo_forecast_dict_qt() {
    QDBusReply<QMap<QString, QVariant> > reply = sessioniface->call("get_yahoo_forecast_dict");
    yahooforecastInfo = reply.value();
}

void SessionDispatcher::get_current_weather_qt() {
    getCityIdInfo();
    QStringList tmplist;
    tmplist << "Kobe" << "Lee";

    bool flag = Util::id_exists_in_location_file(initCityId);
    if(flag) {//获取中国气象局数据
        KThread *thread = new KThread(tmplist, sessioniface, "get_current_weather", initCityId);
        thread->start();
    }
    else {//获取雅虎气象数据
        QStringList latlon = this->getLatandLon(initCityId);
        KThread *thread = new KThread(latlon, sessioniface, "get_current_yahoo_weather", initCityId);
        thread->start();
    }
}

void SessionDispatcher::get_current_weather_dict_qt() {
    QDBusReply<QMap<QString, QVariant> > reply = sessioniface->call("get_current_weather_dict");
    currentInfo = reply.value();
}

void SessionDispatcher::get_current_yahoo_weather_dict_qt() {
    QDBusReply<QMap<QString, QVariant> > reply = sessioniface->call("get_current_yahoo_weather_dict");
    yahoocurrentInfo = reply.value();
}

void SessionDispatcher::get_current_pm25_qt() {
    getCityIdInfo();
    QStringList tmplist;
    tmplist << "Kobe" << "Lee";
    KThread *thread = new KThread(tmplist, sessioniface, "get_current_pm25", initCityId);
    thread->start();
}

void SessionDispatcher::get_pm25_str_qt() {
    QDBusReply<QString> reply = sessioniface->call("get_pm25_str");
    pm25Info = reply.value();
}

QString SessionDispatcher::access_pm25_str_qt() {
    return pm25Info;
}

int SessionDispatcher::get_current_rate() {
    mSettings->beginGroup("weather");
    int rate = 60;
    rate = mSettings->value("rate").toInt();
    mSettings->endGroup();
    mSettings->sync();
    return rate;
}

bool SessionDispatcher::update_weather_data_qt() {
    getCityIdInfo();
    bool flag = Util::id_exists_in_location_file(initCityId);
    if(flag) {//获取中国气象局数据
        QDBusReply<bool> reply = sessioniface->call("update_weather_data", initCityId);
        return reply.value();
    }
    else {
        QStringList latlon = this->getLatandLon(initCityId);
        KThread *thread = new KThread(latlon, sessioniface, "get_current_yahoo_weather", initCityId);
        thread->start();
        return false;
    }
}

QString SessionDispatcher::getSingleWeatherInfo(QString key, QString flag) {
    QVariant info = "";
    if(flag == "forecast") {
        info = forecastInfo.value(key);
    }
    else if(flag == "current") {
        info = currentInfo.value(key);
    }
    else if(flag == "weathericon") {
        info = "../../img/weather/" + key;
    }
    else if(flag == "yahoo") {
        info = yahoocurrentInfo.value(key);
    }
    else if(flag == "yahooforecast") {
        info = yahooforecastInfo.value(key);
    }
    return info.toString();
}

bool SessionDispatcher::showWizardController() {
    WizardDialog *wizardDialog = new WizardDialog(mSettings, 0);
    connect(wizardDialog, SIGNAL(readyToUpdateRateTime(int)), this, SLOT(handler_change_rate(int)));
    connect(wizardDialog, SIGNAL(readyToUpdateWeatherForWizard()), this, SLOT(handler_change_city()));
    wizardDialog-> QWidget::setAttribute(Qt::WA_DeleteOnClose);
    if(wizardDialog->exec()==QDialog::Rejected) {
        return false;
    }
    else {
        return true;
    }
}

void SessionDispatcher::handler_change_rate(int rate) {
    emit startUpdateRateTime(rate);
}

bool SessionDispatcher::showChangeCityDialog() {
    ChangeCityDialog *cityDialog = new ChangeCityDialog(mSettings);
    cityDialog-> QWidget::setAttribute(Qt::WA_DeleteOnClose);
    connect(cityDialog, SIGNAL(readyToUpdateWeather()), this, SLOT(handler_change_city()));
    if(cityDialog->exec()==QDialog::Rejected) {
        return false;
    }
    else {
        return true;
    }
}

void SessionDispatcher::handler_change_city() {
    emit startChangeQMLCity();
}

int SessionDispatcher::getLengthOfCityList() {
    mSettings->beginGroup("weather");
    QStringList cityList = mSettings->value("places").toStringList();
    mSettings->endGroup();
    mSettings->sync();
    return cityList.size();
}

void SessionDispatcher::initConfigFile() {
    mSettings->beginGroup("user");
    QString id = mSettings->value("id").toString();
    if(id.isEmpty()) {
        mSettings->setValue("id", "0");
    }
    mSettings->endGroup();

    mSettings->beginGroup("weather");
    QString cityId = mSettings->value("cityId").toString();
    //cityId为空时，赋默认值为：101250101
    if(cityId.isEmpty()) {
        cityId = QString("101250101");
        mSettings->setValue("cityId", cityId);
    }
    QStringList idList = mSettings->value("idList").toStringList();
    if(idList.isEmpty()) {
        idList.append("101250101");
        idList.append("101010100");
        idList.append("101020100");
        mSettings->setValue("idList", idList);
    }
    QStringList places = mSettings->value("places").toStringList();
    //places为空时，赋默认值为：湖南,长沙,长沙
    if(places.isEmpty()) {
//        places = QStringList("湖南,长沙,长沙");
        places.append("湖南,长沙,长沙");
        places.append("北京,北京,北京");
        places.append("上海,上海,上海");
        mSettings->setValue("places", places);
    }
    //纬度
    QStringList latitude = mSettings->value("latitude").toStringList();
    if(latitude.isEmpty()) {
        latitude.append("NA");
        latitude.append("NA");
        latitude.append("NA");
        mSettings->setValue("latitude", latitude);
    }
    //经度
    QStringList longitude = mSettings->value("longitude").toStringList();
    if(longitude.isEmpty()) {
        longitude.append("NA");
        longitude.append("NA");
        longitude.append("NA");
        mSettings->setValue("longitude", longitude);
    }
    QString rate = mSettings->value("rate").toString();
    //rate为空时，赋默认值为：60
    if(rate.isEmpty()) {
        rate = QString("60");
        mSettings->setValue("rate", rate);
    }
    mSettings->endGroup();

    mSettings->beginGroup("skin");
    QString backGround = mSettings->value("background").toString();
    //backGround为空时，赋默认值为：0_bg
    if(backGround.isEmpty() || backGround != "0_bg") {
        backGround = QString("0_bg");
        mSettings->setValue("background", backGround);
    }
    mSettings->endGroup();
    mSettings->sync();
}

void SessionDispatcher::getCityIdInfo() {
    mSettings->beginGroup("weather");
    initCityId = mSettings->value("cityId").toString();
    mSettings->endGroup();
    mSettings->sync();
}

QStringList SessionDispatcher::getLatandLon(QString id) {
    QStringList tmp;
    bool flag = false;
    mSettings->beginGroup("weather");
    QStringList idList = mSettings->value("idList").toStringList();
    QStringList latitude = mSettings->value("latitude").toStringList();
    QStringList longitude = mSettings->value("longitude").toStringList();
    mSettings->endGroup();
    mSettings->sync();

    int j = 0;
    for (int i=0; i< idList.length(); i++) {
        if(id == idList[i]) {
            flag = true;
            break;
        }
        j += 1;
    }
    if(flag) {
        flag = false;
        tmp << latitude[j];
        tmp << longitude[j];
    }
    return tmp;
}

void SessionDispatcher::change_maincheckbox_status(QString status) {
    emit startChangeMaincheckboxStatus(status);
}
