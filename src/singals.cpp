#include "singals.h"

Singals::Singals(QObject *parent) :
    QObject(parent)
{
    pAptTransaction = new AptTransaction(this);
   if ( pAptTransaction != NULL )
    //    pAptTransaction->initApt();
   // qDebug() << "signal slot";
   {
        connect(pAptTransaction, SIGNAL(downloadProgress(QString, QString, QString)), this, SLOT(downloadProgress(QString, QString, QString)));
        connect(pAptTransaction, SIGNAL(installProgress(QString,int)), this, SLOT(installProgress(QString,int)));
        connect(pAptTransaction, SIGNAL(installPackageFinished(QString, QString)), this, SLOT(installPackageFinished(QString, QString)));
        connect(pAptTransaction, SIGNAL(updateCacheFinished(QString)), this, SLOT(updateCacheFinished(QString)));
   }
}

Singals::~Singals(){
    qDebug()<<"  disConstruct ";
}

void Singals::downloadProgress(QString pkgName, QString totalSize, QString  speed)
{
    PkgName = pkgName;
    Speed = speed;
    TotalSize = totalSize;
}

void Singals::installProgress(QString pkgName, int progress)
{
    progressbar = progress;
    PkgName = pkgName;
   qDebug() << "install 信号" << progress;
}

void Singals::updateCacheFinished(QString status)
{
    Status = status;
}

void Singals::installPackageFinished(QString pkgName, QString status)
{
    PkgName = pkgName;
    Status = status;
    progressbar = 100;
}

int Singals::getProgress(int progress)
{
    qDebug() << "11111" << progressbar << "222" << progress;
    return progressbar;
}
