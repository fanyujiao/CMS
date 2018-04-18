/*************************************************
Copyright :Wuxi National Fundamental Software of China Co., Ltd.
Author:yjfan@nfs-wuxi.com
Date:2016-07-15
Description:APT相关操作
**************************************************/

#ifndef APTTRANSACTION_H
#define APTTRANSACTION_H

#include <QObject>
#include <QDebug>
#include <QtDBus/QDBusConnection>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDBusVariant>
#include <QDBusMetaType>
#include <QDBusArgument>
#include <QDBusVariant>

/****QT自定ProgressDetails  S义类型***/
struct ProgressDetailsMetaType
{
    quint32  currentItems;
    quint32  totalItems;
    quint64  currentyBytes;
    quint64  totalBytes;
    double currentCps;
    quint64  eta;
};

Q_DECLARE_METATYPE(ProgressDetailsMetaType)

const QDBusArgument &operator>>(const QDBusArgument &argument, ProgressDetailsMetaType &progressDetailsMetaType);


class AptTransaction : public QObject
{
    Q_OBJECT
public:
    explicit AptTransaction(QObject *parent = 0);


    int progressbar;
    QString PkgName;
    QString Status;
     QString TotalSize;
     QString  Speed;

private:
    QDBusInterface *aptdaemonClient;
    QDBusInterface *aptdaemonTrans;
    QString pkgName;
    QString action;

public:
  Q_INVOKABLE  void initApt();
  Q_INVOKABLE  void installPackage(QString packageName);
    Q_INVOKABLE void removePackage(QString packageName);
   Q_INVOKABLE  void upgradePackage(QString packageName);
  Q_INVOKABLE   void updateCache();
    QString byteSizeConvertToStr(quint64 byteSize);

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, ProgressDetailsMetaType &progressDetailsMetaType)
    {
        argument.beginStructure();
        argument >> progressDetailsMetaType.currentItems
                          >> progressDetailsMetaType.totalItems
                          >> progressDetailsMetaType.currentyBytes
                          >> progressDetailsMetaType.totalBytes
                          >> progressDetailsMetaType.currentCps
                          >> progressDetailsMetaType.eta;
        argument.endStructure();

        return argument;
    }

private:
    void aptTransactionRun(QString objPath);
public slots:
    void propertyChangedSlot(QString property, QDBusVariant  qVariant);
    void finishedSlot(QString status) ;
    void downloadProgressSlot(QString pkgName, QString totalSize, QString  speed);
    void installProgressSlot(QString pkgName, int progress);
    void updateCacheFinishedSlot(QString status);
    void installPackageFinishedSlot(QString pkgName, QString status);
    int getProgress(int progress);
    QString getPagename(int progress);
    QString getStatus();

signals:
    void updateCacheFinished(QString status);
   //void installProgress(QString status, int progress, QString totalSize , QString  speed);
    void downloadProgress(QString pkgName, QString totalSize, QString  speed);
    void installProgress(QString pkgName, int progress);
    void installPackageFinished(QString pkgName, QString status);

};

#endif // APTTRANSACTION_H
