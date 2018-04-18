/*************************************************
Copyright :Wuxi National Fundamental Software of China Co., Ltd.
Author:yjfan@nfs-wuxi.com
Date:2016-07-16
Description:接收安装软件相关信息的信号
**************************************************/
#ifndef SINGALS_H
#define SINGALS_H

#include <QObject>
#include "apttransaction.h"
#include <QDebug>

class Singals : public QObject
{
    Q_OBJECT
public:
    explicit Singals(QObject *parent = 0);
    ~Singals();
    AptTransaction *pAptTransaction;
    int progressbar;
    QString PkgName;
    QString Status;
     QString TotalSize;
     QString  Speed;
public slots:
    void downloadProgress(QString pkgName, QString totalSize, QString  speed);
    void installProgress(QString pkgName, int progress);
    void updateCacheFinished(QString status);
    void installPackageFinished(QString pkgName, QString status);
    int getProgress(int progress);
};

#endif // SINGALS_H
