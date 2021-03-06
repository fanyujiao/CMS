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
#include <QApplication>
#include "fcitxcfgwizard.h"
#include "toolkits.h"
#include "systemdispatcher.h"
#include "sessiondispatcher.h"
#include "pc-application.h"
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QtDeclarative>
#include "qmlaudio.h"
#include <QTextCodec>
#include <QProcess>
#include <QDebug>
#include <sys/types.h>
#include "qrangemodel.h"
#include "qstyleitem.h"
#include "qwheelarea.h"
#include "qtmenu.h"
#include "qcursorarea.h"
#include <QDeclarativeExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtCore/QTimer>
#include <QFileSystemModel>
#include <qdeclarative.h>
#include <qdeclarativeextensionplugin.h>
#include <qdeclarativeengine.h>
#include <qdeclarativeitem.h>
#include <qdeclarativeimageprovider.h>
#include <qdeclarativeview.h>
#include <QtSingleApplication>
#include "processmanager.h"
#include "devicemanager.h"
#include <QTranslator>
#include "apttransaction.h"
#include "singals.h"

void registerTypes() {
    qmlRegisterType<Toolkits>("ToolkitsType", 0, 1, "Toolkits");
    qmlRegisterType<SessionDispatcher>("SessionType", 0, 1, "SessionDispatcher");
    qmlRegisterType<SystemDispatcher>("SystemType", 0, 1, "SystemDispatcher");
    qmlRegisterType<ProcessManager>("ProcessType", 0, 1, "ProcessManager");
    qmlRegisterType<DeviceManager>("DeviceType", 0, 1, "DeviceManager");
    qmlRegisterType<FcitxCfgWizard>("FcitxCfgWizard", 0, 1, "FcitxCfgWizard");
    qmlRegisterType<QmlAudio>("AudioType", 0, 1, "QmlAudio");
    qmlRegisterType<QRangeModel>("RangeModelType", 0, 1, "RangeModel");
    qmlRegisterType<QStyleItem>("StyleItemType", 0, 1, "StyleItem");
    qmlRegisterType<QWheelArea>("WheelAreaType", 0, 1, "WheelArea");
    qmlRegisterType<QtMenu>("MenuType", 0, 1, "Menu");
    qmlRegisterUncreatableType<QtMenuBase>("MenuBaseType", 0, 1, "NativeMenuBase", QLatin1String("Do not create objects of type NativeMenuBase"));
    qmlRegisterType<QCursorArea>("CursorAreaType", 0, 1, "CursorArea");
    qmlRegisterType<AptTransaction>("Apt",0,1,"Apt");
 //   qmlRegisterType<Singals>("Signals",0,1,"Signals");
}

int main(int argc, char** argv)
{
    //单程序运行处理
    QtSingleApplication app(argc, argv);
    if (app.isRunning())
        return 0;

    //编码处理
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QString locale = QLocale::system().name();
    QTranslator translator;
    if(locale == "zh_CN") {
        //加载Qt和QML文件的国际化
        if(!translator.load("pc-manager_" + locale + ".qm",
                            ":/translate/translation/"))
            qDebug() << "Load translation file："<< "pc-manager_" + locale + ".qm" << " failed!";
        else
            app.installTranslator(&translator);
    }

    //加载Qt对话框默认的国际化
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + locale,
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    //注册QML模块
    registerTypes();

//    system("/home/saucy/Slider/src/wizard.py");

    //启动画面
    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/pixmap/image/feature.png"));
    splash->setDisabled(true);
    QBitmap objBitmap(313, 209);
    QPainter painter(&objBitmap);
    painter.fillRect(splash->rect(),Qt::white);
    painter.setBrush(QColor(0,0,0));
    painter.drawRoundedRect(splash->rect(), 10,10);
    splash->setMask(objBitmap);
    splash->show();
    splash->showMessage(QObject::tr("starting...."), Qt::AlignHCenter|Qt::AlignBottom, Qt::black);//正在启动中....
    app.processEvents();
    //同时创建主视图对象
    IhuApplication application;
    splash->showMessage(QObject::tr("loading module data...."), Qt::AlignHCenter|Qt::AlignBottom, Qt::black);//正在加载模块数据....
    //数据处理
    application.setup(/*"main.qml"*/);
    //显示主界面，并结束启动画面
    application.showQMLWidget();
    splash->finish(&application);
    delete splash;
    return app.exec();
}


/*
a 	ARRAY 数组
b 	BOOLEAN 布尔值
d 	DOUBLE IEEE 754双精度浮点数
g 	SIGNATURE 类型签名
i 	INT32 32位有符号整数
n 	INT16 16位有符号整数
o 	OBJECT_PATH 对象路径
q 	UINT16 16位无符号整数
s 	STRING 零结尾的UTF-8字符串
t 	UINT64 64位无符号整数
u 	UINT32 32位无符号整数
v 	VARIANT 可以放任意数据类型的容器，数据中包含类型信息。例如glib中的GValue。
x 	INT64 64位有符号整数
y 	BYTE 8位无符号整数
() 	定义结构时使用。例如"(i(ii))"
{} 	定义键－值对时使用。例如"a{us}"

a表示数组，数组元素的类型由a后面的标记决定。例如：
    "as"是字符串数组。
    数组"a(i(ii))"的元素是一个结构。用括号将成员的类型括起来就表示结构了，结构可以嵌套。
    数组"a{sv}"的元素是一个键－值对。"{sv}"表示键类型是字符串，值类型是VARIANT。
*/
