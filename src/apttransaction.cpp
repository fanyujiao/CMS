#include "apttransaction.h"
#include "common.h"


AptTransaction::AptTransaction(QObject *parent) :
    QObject(parent)
{
    aptdaemonClient = NULL;

    connect(this, SIGNAL(downloadProgress(QString, QString, QString)), this, SLOT(downloadProgressSlot(QString,QString,QString)));
    connect(this, SIGNAL(installProgress(QString,int)), this, SLOT(installProgressSlot(QString,int)));
    connect(this, SIGNAL(installPackageFinished(QString, QString)), this, SLOT(installPackageFinishedSlot(QString, QString)));
    connect(this, SIGNAL(updateCacheFinished(QString)), this, SLOT(updateCacheFinishedSlot(QString)));
}

/*************************************************
Function:           initApt
Description:      初始化APT类
Parameter:        空
Return: 空
*************************************************/
 void AptTransaction::initApt()
{
    if(NULL == aptdaemonClient)
    {
        aptdaemonClient = new QDBusInterface("org.debian.apt",
                                             "/org/debian/apt",
                                             "org.debian.apt",
                                             QDBusConnection::systemBus());
    }
}

/*************************************************
Function:           aptTransactionRun
Description:      执行aptTransaction， 绑定相关信号
Parameter:        objPath  DBUS objPath
Return:              空
*************************************************/
void AptTransaction::aptTransactionRun(QString objPath)
{
    aptdaemonTrans = new QDBusInterface("org.debian.apt",
                                                                            objPath,
                                                                            "org.debian.apt.transaction",
                                                                            QDBusConnection::systemBus());


    if(!QDBusConnection::systemBus().connect("org.debian.apt",
                                                                                objPath,
                                                                                "org.debian.apt.transaction",
                                                                                "PropertyChanged",
                                                                                 this,
                                                                                 SLOT(propertyChangedSlot(QString, QDBusVariant))))
    {
        qPrintErrorMessage("%s", QDBusConnection::systemBus().lastError().message().toLatin1().data());
    }
    if(!QDBusConnection::systemBus().connect("org.debian.apt",
                                                                          objPath,
                                                                          "org.debian.apt.transaction",
                                                                            "Finished",
                                                                            this,
                                                                            SLOT(finishedSlot(QString))))
    {
        qPrintErrorMessage("%s", QDBusConnection::systemBus().lastError().message().toLatin1().data());
    }
    aptdaemonTrans->call("Run");

}

/*************************************************
Function:           installPackage
Description:      安装软件包
Parameter:      packageName 软件包名
Return: 空
*************************************************/
void AptTransaction::installPackage(QString packageName)
{
    QStringList pkgNames(packageName);
    pkgName = packageName;

    if(NULL != aptdaemonClient)
    {
        QDBusReply<QString> installPkgCallReply = aptdaemonClient->call("InstallPackages", pkgNames);
        if(!installPkgCallReply.isValid())
        {
            qPrintErrorMessage("%s", installPkgCallReply.error().message().toLatin1().data());
            return;
        }
        else
        {
#ifndef QT_NO_DEBUG
            qPrintDebugMessage("%s", installPkgCallReply.value().toLatin1().data());
#endif
            action = "InstallPackage";
             aptTransactionRun(installPkgCallReply.value());

        }
    }
}


/*************************************************
Function:           removePackage
Description:      卸载软件包
Parameter:      packageName 软件包名
Return: 空
*************************************************/
void AptTransaction::removePackage(QString packageName)
{
    QStringList pkgNames(packageName);
    pkgName = packageName;

    if(NULL != aptdaemonClient)
    {
        QDBusReply<QString> removePkgCallReply = aptdaemonClient->call("RemovePackages", pkgNames);
        if(!removePkgCallReply.isValid())
        {
            qPrintErrorMessage("%s", removePkgCallReply.error().message().toLatin1().data());
            return;
        }
        else
        {
#ifndef QT_NO_DEBUG
            qPrintDebugMessage("%s",  removePkgCallReply.value().toLatin1().data());
#endif
            action = "RemovePackage";
             aptTransactionRun(removePkgCallReply.value());
        }
    }
}

/*************************************************
Function:           upgradePackage
Description:      安装软件包
Parameter:      packageName 软件包名
Return: 空
*************************************************/
void AptTransaction::upgradePackage(QString packageName)
{
    QStringList pkgNames(packageName);
    pkgName = packageName;

    if(NULL != aptdaemonClient)
    {
        QDBusReply<QString> upgradePkgCallReply = aptdaemonClient->call("UpgradePackages", pkgNames);
        if(!upgradePkgCallReply.isValid())
        {
            qPrintErrorMessage("%s", upgradePkgCallReply.error().message().toLatin1().data());
            return;
        }
        else
        {
#ifndef QT_NO_DEBUG
            qPrintDebugMessage("%s", upgradePkgCallReply.value().toLatin1().data());
#endif
             action = "UpgradePackage";
             aptTransactionRun(upgradePkgCallReply.value());
        }
    }
}


/*************************************************
Function:           updateCache
Description:      更新软件源
Parameter:        空
Return: 空
*************************************************/
void AptTransaction::updateCache()
{
    if(NULL != aptdaemonClient)
    {
        QDBusReply<QString> updateCacheCallReply = aptdaemonClient->call("UpdateCache");
        if(!updateCacheCallReply.isValid())
        {
            qPrintErrorMessage("%s", updateCacheCallReply.error().message().toLatin1().data());
            return;
        }
        else
        {
#ifndef QT_NO_DEBUG
            qPrintDebugMessage("%s", updateCacheCallReply.value().toLatin1().data());
#endif
            action = "UpdateCache";
             aptTransactionRun(updateCacheCallReply.value());
        }
    }
}

/*************************************************
Function:           propertyChangedSlot
Description:      DBus Signal Slot函数, 接收aptdaemon 相关属性变化
Parameter:        property 属性名称
                           qVariant 属性value
Return: 空
*************************************************/

void AptTransaction::propertyChangedSlot(QString property, QDBusVariant qVariant)
{
    if("Progress" == property)
    {
        if("InstallPackage" == action)
        {
           // qDebug()<<"progress : "<<qVariant.variant().toInt();
            emit installProgress(pkgName, qVariant.variant().toInt());
        }
        return;
    }

    if("ProgressDetails" == property)
    {
        ProgressDetailsMetaType progressDetails ;
        qVariant.variant().value<QDBusArgument>() >> progressDetails;
        if(0 == progressDetails.currentItems &&
           0 == progressDetails.totalItems &&
           0 == progressDetails.currentyBytes &&
           0 == progressDetails.totalBytes &&
           0 == progressDetails.currentCps &&
           0 == progressDetails.eta )
        {
            return;
        }
        if("InstallPackage" == action)
        {
            emit downloadProgress(pkgName,
                                                      byteSizeConvertToStr(progressDetails.totalBytes),
                                                      byteSizeConvertToStr((quint64)progressDetails.currentCps));
        }

//#ifndef QT_NO_DEBUG
//        qPrintDebugMessage("%s  ProgressDetails itemsDone: %d  itemsTotal  %d bytesDone %lld bytesTtotal %lld peed %f eta %lld",
//                           pkgName.toLatin1().data(),
//                           progressDetails.currentItems,
//                           progressDetails.totalItems,
//                           progressDetails.currentyBytes,
//                           progressDetails.totalBytes,
//                           progressDetails.currentCps,
//                           progressDetails.eta);
//#endif
        return;
    }
}

/*************************************************
Function:           finishedSlot
Description:      DBus Signal Slot函数, 相关操作完成
Parameter:        status 执行状态值（是否成功等）
Return: 空
*************************************************/
void AptTransaction::finishedSlot(QString status)
{
#ifndef QT_NO_DEBUG
    qPrintDebugMessage("%s : %s", pkgName.toLatin1().data(), status.toLatin1().data());
#endif

    if("UpdateCache" == action)
    {
        emit updateCacheFinished( status);
    }
    if("InstallPackage" == action)
    {
        emit installPackageFinished(pkgName, status);
       // qDebug() << "emit install finish";
    }

    if(NULL != aptdaemonTrans)
    {
        delete aptdaemonTrans;
        aptdaemonTrans = NULL;
    }
    pkgName = "";
    action = "";
}


QString AptTransaction::byteSizeConvertToStr(quint64 byteSize)
{
    QString retVal ;
    double dbSize;
    if(byteSize >= 1024 * 1024 *1024)
    {
        dbSize = (double)byteSize/(1024 * 1024 *1024);
        retVal = QString::number(dbSize, 'f', 2) + " GB";
    }
    else if(byteSize >= 1024 * 1024)
    {
        dbSize = (double)byteSize/(1024 * 1024 ) ;
        retVal = QString::number(dbSize, 'f', 2) + " MB";
    }
    else if(byteSize >= 1024 )
    {
        dbSize = (double)byteSize/(1024 ) ;
        retVal = QString::number(dbSize, 'f', 2) + " KB";
    }
    else if(byteSize < 1024)
    {
        dbSize = (double)byteSize;
        retVal = QString::number(dbSize, 'f', 2) + " B";
    }
    return retVal;
}



void AptTransaction::downloadProgressSlot(QString pkgName, QString totalSize, QString  speed)
{
    PkgName = pkgName;
    Speed = speed;
    TotalSize = totalSize;
}

void AptTransaction::installProgressSlot(QString pkgName, int progress)
{
    progressbar = progress;
    PkgName = pkgName;
 //  qDebug() << "install 信号" << progress;
}

void AptTransaction::updateCacheFinishedSlot(QString status)
{
    Status = status;
}

void AptTransaction::installPackageFinishedSlot(QString pkgName, QString status)
{
    PkgName = pkgName;
    Status = status;
    progressbar = 100;
 //   qDebug()  << "xinhao::" << status;
}

int AptTransaction::getProgress(int progress)
{
    //qDebug() << "progress:" << progressbar << "222" << progress;
    return progressbar;
}

QString AptTransaction::getPagename(int progress)
{
   // qDebug() << "pkgname:" << PkgName<< progress;
    return PkgName;
}

QString AptTransaction::getStatus()
{
  //  qDebug() << "1111"  << Status;
    return Status;
}
