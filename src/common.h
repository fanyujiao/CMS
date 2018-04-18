/*************************************************
Copyright :Wuxi National Fundamental Software of China Co., Ltd.
Author:yjfan@nfs-wuxi.com
Date:2016-07-15
Description:公共方法及配置
**************************************************/

#ifndef COMMON_H
#define COMMON_H
#include<QtDebug>


#define SOCIALCONTACTTOOLSET_DATABASE_CONNECTION_NAME "SocialContactToolsetDBConnection"


/***** 自定义输出格式 ******/
#define qPrintErrorMessage(format, ...) qDebug("[Error]   %s Line:%d,   "format,  __FILE__, __LINE__ , ##__VA_ARGS__);
#define qPrintWarningMessage(format, ...) qDebug("[Warning]   %s Line:%d,   "format,  __FILE__, __LINE__ , ##__VA_ARGS__);
#define qPrintCriticalMessage(format, ...) qDebug("[Critical]   %s Line:%d,   "format,  __FILE__, __LINE__ , ##__VA_ARGS__);
#define qPrintDebugMessage(format, ...) qDebug("[Debug]   %s Line:%d,   "format,  __FILE__, __LINE__ , ##__VA_ARGS__);

/******* example *******/
/***
#ifndef QT_NO_DEBUG
    qPrintErrorMessage("%s", "Error");
    qPrintWarningMessage("%s", "Warning");
    qPrintCriticalMessage("%s", "Critical");
    qPrintDebugMessage("%s  %d", "Debug", 10);
#endif

***/

extern int gScreenWidth;
extern int gScreenHeight;



#endif // COMMON_H
