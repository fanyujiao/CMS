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

#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QDir>
#include "wizarddialog.h"
#include "ui_wizarddialog.h"
#include <QPixmap>
#include <QBitmap>
#include <QPainter>

#include <QSettings>

#include "locationdialog.h"
#include "util.h"
WizardDialog::WizardDialog(QSettings *mSettings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WizardDialog)
{
    ui->setupUi(this);
    pSettings = mSettings;
    this->setAttribute(Qt::WA_DeleteOnClose);//防止内存泄漏
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    ui->btn_close->setPixmap(QPixmap(":/pixmap/image/closeBtn.png"));
    ui->okBtn->setStyleSheet("QPushButton {border-image:url(:/pixmap/image/ok.png);}"
                "QPushButton:hover{border-image:url(:/pixmap/image/ok-hover.png);}");
    ui->quitBtn->setStyleSheet("QPushButton {border-image:url(:/pixmap/image/ok.png);}"
                "QPushButton:hover{border-image:url(:/pixmap/image/ok-hover.png);}");
    ui->addBtn->setStyleSheet("QPushButton {border-image:url(:/pixmap/image/ok.png);}"
                "QPushButton:hover{border-image:url(:/pixmap/image/ok-hover.png);}");
    ui->delBtn->setStyleSheet("QPushButton {border-image:url(:/pixmap/image/ok.png);}"
                "QPushButton:hover{border-image:url(:/pixmap/image/ok-hover.png);}");

    spinValue = 0;
    newCityName = "";
    newCityId = "";

    ui->btn_close->installEventFilter(this);
    ui->spinBox->installEventFilter(this);
    ui->quitBtn->installEventFilter(this);
    ui->okBtn->installEventFilter(this);
    ui->addBtn->installEventFilter(this);
    ui->delBtn->installEventFilter(this);
    ui->spinBox->setRange(0, 60);
    ui->spinBox->setSingleStep(5);

    loadConf();
    connect(ui->okBtn, SIGNAL(clicked()),this, SLOT(writeWeatherConf()));
    connect(ui->quitBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(setSpinValue(int)));
    connect(ui->addBtn,SIGNAL(clicked()),this,SLOT(addLocation()));
    connect(ui->delBtn,SIGNAL(clicked()),this,SLOT(delLocation()));
}

WizardDialog::~WizardDialog()
{
    delete ui;
}


void WizardDialog::loadConf() {
    pSettings->beginGroup("weather");
    spinValue = pSettings->value("rate").toInt();
    cityList = pSettings->value("places").toStringList();
    newCityId = pSettings->value("cityId").toString();
    pSettings->endGroup();
    pSettings->sync();
    ui->spinBox->setValue(spinValue);
    ui->listWidget->clear();
    int len = cityList.size();
    if (len == 0) {
        ui->delBtn->setEnabled(false);
    }
    else {
        for(int i=0; i<len; i++) {
            ui->listWidget->insertItem(i, cityList.at(i));
        }
        QListWidgetItem *currentitem;
        currentitem = ui->listWidget->item(0);
        ui->listWidget->setCurrentItem(currentitem);
    }
}

void WizardDialog::setLocation(QString cityName, QString cityId, QString lat, QString lon) {
    bool flag = false;
    QStringList idList = pSettings->value("weather/idList").toStringList();
    for(int i = 0; i<idList.length(); i++) {
        if(idList[i] == cityId) {
            flag = true;
            break;
        }
    }
    if(flag) {
        flag = false;
        //警告：        该城市已经存在，请点击 ’[更换城市]‘ 按钮！
        QMessageBox::warning(NULL,
                             tr("Warning:"),
                             tr("The city already exists, please click on the '[Change City]' button!"),
                             QMessageBox::Ok);
    }
    else {
        ui->listWidget->insertItem(0, cityName);
        ui->delBtn->setEnabled(true);
        QListWidgetItem *currentitem;
        currentitem = ui->listWidget->item(0);
        ui->listWidget->setCurrentItem(currentitem);
        newCityName = cityName;
        newCityId = cityId;
        //添加城市后，重新写天气配置到配置文件中
        QStringList listName = pSettings->value("weather/places").toStringList();
        listName.append(newCityName);
        idList.append(newCityId);
        QStringList latitude = pSettings->value("weather/latitude").toStringList();
        latitude.append(lat);
        QStringList longitude = pSettings->value("weather/longitude").toStringList();
        longitude.append(lon);
        pSettings->setValue("weather/places", listName);
        pSettings->setValue("weather/cityId", newCityId);
        pSettings->setValue("weather/idList", idList);
        pSettings->setValue("weather/latitude", latitude);
        pSettings->setValue("weather/longitude", longitude);
        pSettings->sync();
        emit readyToUpdateWeatherForWizard();
    }
}

void WizardDialog::setSpinValue(int value) {
    spinValue = value;
}

void WizardDialog::writeWeatherConf() {
    QString strValue = QString::number(spinValue, 10);
    pSettings->setValue("weather/rate", strValue);
    pSettings->sync();
    emit readyToUpdateRateTime(spinValue);
    this->accept();
}

void WizardDialog::addLocation() {
    LocationDialog *locationDialog = new LocationDialog();
    connect(locationDialog, SIGNAL(sendCityInfo(QString, QString, QString, QString)), this, SLOT(setLocation(QString, QString, QString, QString)));
    locationDialog->exec();
}

void WizardDialog::delLocation() {
    QString currentStr = ui->listWidget->currentItem()->text();
    QStringList listName = pSettings->value("weather/places").toStringList();
    QStringList idList = pSettings->value("weather/idList").toStringList();
    QStringList latitude = pSettings->value("weather/latitude").toStringList();
    QStringList longitude = pSettings->value("weather/longitude").toStringList();
    QStringList newList;
    int size;
    int index = 0;
    QString cityId;
    size = listName.size();
    for(int i =0; i<size; ++i) {
        if(listName.at(i) != currentStr) {
            newList.append(listName.at(i));
        }
        else {
            index = i;
        }

    }
    if(newList.size()==0)//城市列表为空时
    {
        ui->delBtn->setEnabled(false);
        ui->listWidget->clear();
        pSettings->setValue("weather/places", newList);
        pSettings->setValue("weather/cityId", "");
        pSettings->setValue("weather/idList", newList);
        pSettings->setValue("weather/latitude", newList);
        pSettings->setValue("weather/longitude", newList);
        pSettings->sync();
    }
    else {
        //将删除某个城市后的新城市列表显示在界面上
        ui->listWidget->clear();
        for(int j=0; j<newList.size(); ++j)
        {
            ui->listWidget->insertItem(j, newList.at(j));
        }
        QListWidgetItem *currentitem;
        currentitem = ui->listWidget->item(0);
        ui->listWidget->setCurrentItem(currentitem);

        //将删除某个城市后的新城市列表写入配置中
        pSettings->setValue("weather/places", newList);


        //将删除某个城市后的新城市id列表写入配置中
        QStringList newidList;
        size = idList.size();
        for(int i =0; i<size; ++i) {
            if(i != index) {
                newidList.append(idList.at(i));
            }
        }
        cityId = newidList[0];
        pSettings->setValue("weather/idList", newidList);


        //将删除某个城市后的新城市纬度列表写入配置中
        QStringList newlat;
        size = latitude.size();
        for(int i =0; i<size; ++i) {
            if(i != index) {
                newlat.append(latitude.at(i));
            }
        }
        pSettings->setValue("weather/latitude", newlat);


        //将删除某个城市后的新城市经度列表写入配置中
        QStringList newlon;
        size = longitude.size();
        for(int i =0; i<size; ++i) {
            if(i != index) {
                newlon.append(longitude.at(i));
            }
        }
        pSettings->setValue("weather/longitude", newlon);


        pSettings->setValue("weather/cityId", cityId);
        pSettings->sync();
        emit readyToUpdateWeatherForWizard();
    }
}


bool WizardDialog::eventFilter(QObject *obj, QEvent *event) {
    if(obj == ui->btn_close){
            if(event->type() == QEvent::Enter){
                ui->btn_close->setPixmap(QPixmap(":/pixmap/image/closeBtn-hover.png"));
            }else if(event->type() == QEvent::Leave){
                ui->btn_close->setPixmap(QPixmap(":/pixmap/image/closeBtn.png"));
            }else if(event->type() == QEvent::MouseButtonPress){
                ui->btn_close->setPixmap(QPixmap(":/pixmap/image/closeBtn-hover.png"));
            }else if(event->type() == QEvent::MouseButtonRelease){
                QMouseEvent *me = (QMouseEvent *)event;
                QLabel *lb = (QLabel *)obj;
                if(me->x() > 0 && me->x() < lb->width() && me->y() > 0 && me->y() < lb->height()){
                    this->close();
                }else{
                    ui->btn_close->setPixmap(QPixmap(":/pixmap/image/closeBtn.png"));
                }
            } else {
                return QObject::eventFilter(obj, event);
            }
    }
    if(obj == ui->spinBox || obj == ui->quitBtn || obj == ui->okBtn || obj == ui->addBtn || obj == ui->delBtn)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *me = (QMouseEvent *)event;
            dragPos = me->globalPos() - frameGeometry().topLeft();
        }else if(event->type() == QEvent::MouseButtonRelease)
        {
            setWindowOpacity(1);
        }
    }
    return QObject::eventFilter(obj, event);
}

void WizardDialog::mousePressEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton) {
        dragPos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void WizardDialog::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton )
    {
        move(event->globalPos() - dragPos);
        setWindowOpacity(1);//0.9
    }
    event->accept();
}

void WizardDialog::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
    {
        setWindowOpacity(1);
    }
    event->accept();
}
