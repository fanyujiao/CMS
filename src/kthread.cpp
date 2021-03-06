/*
 * Copyright (C)  National University of Defense Technology(NUDT) & cdos Ltd.
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
 * You should have rec2013eived a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QDebug>
#include "kthread.h"

KThread::KThread(QStringList &arglist, QDBusInterface *systemiface, /*QObject *parent, */QString method, QString flag):QThread(/*parent*/)
{
    iface = systemiface;
    methodName = method;
    list = arglist;
    fileFlag = flag;
}

KThread::~KThread() {
    stop();
}

void KThread::run() {
    if(methodName == "onekey_clean_crufts_function") {
        iface->call("onekey_clean_crufts_function", list);
    }
    else if(methodName == "history_clean_records_function") {
        iface->call("history_clean_records_function", fileFlag);
    }
    else if(methodName == "clean_system_history") {
        iface->call("clean_system_history");
    }
    else if(methodName == "cookies_clean_record_function") {
        iface->call("cookies_clean_record_function", list);
    }
    else if(methodName == "cookies_clean_records_function") {
        iface->call("cookies_clean_records_function", fileFlag);
    }
    else if(methodName == "clean_package_cruft") {
        iface->call("clean_package_cruft", list, fileFlag);
    }
    else if(methodName == "clean_file_cruft") {
        iface->call("clean_file_cruft", list, fileFlag);
    }
    else if(methodName == "get_forecast_weahter") {
        iface->call("get_forecast_weahter", fileFlag);
    }
    else if(methodName == "get_current_weather") {
        iface->call("get_current_weather", fileFlag);
    }
    else if(methodName == "get_current_yahoo_weather") {
        iface->call("get_current_yahoo_weather", list, fileFlag);
    }
    else if(methodName == "get_current_pm25") {
        iface->call("get_current_pm25", fileFlag);
    }
}
void KThread::stop() {
    wait();
}
