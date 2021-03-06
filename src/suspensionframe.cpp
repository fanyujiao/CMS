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
#include "suspensionframe.h"
#include "ui_suspensionframe.h"
#include <QMouseEvent>
SuspensionFrame::SuspensionFrame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SuspensionFrame)
{
    ui->setupUi(this);

    this->setWindowOpacity(1.0);
    ui->title1->setText(tr("System runs smoothly"));//系统运行流畅
    ui->title2->setText(tr("No need to accelerate"));//无需进行加速
    this->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QDesktopWidget *desktop = QApplication::desktop();
    this->move(desktop->width() - this->width(), 80);
    ratio_sus = 0;

    locale_Lan = this->get_locale_version();
    if(locale_Lan == "zh_CN") {
        ui->fastBtn->setStyleSheet("QPushButton {border-image:url(:/pixmap/image/upshiftZH.png);color: white; font-size: 12px}"
                    "QPushButton:hover{border-image:url(:/pixmap/image/upshiftZH-hover.png);}"
                    "QPushButton:pressed{border-image:url(:/pixmap/image/upshiftZH-press.png);}");
    }
    else {
        ui->fastBtn->setStyleSheet("QPushButton {border-image:url(:/pixmap/image/upshiftEN.png);color: white; font-size: 12px}"
                    "QPushButton:hover{border-image:url(:/pixmap/image/upshiftEN-hover.png);}"
                    "QPushButton:pressed{border-image:url(:/pixmap/image/upshiftEN-press.png);}");
    }

    ui->descBtn->setStyleSheet("QPushButton {border:0px;color:#1790c9}");
    ui->descBtn->setText(tr("Details"));//详细
    ui->descBtn->setFlat(true);
    ui->descBtn->setFocusPolicy(Qt::NoFocus);
    this->resize(265, 173);
}

SuspensionFrame::~SuspensionFrame() {
    delete ui;
}

QString SuspensionFrame::get_locale_version() {
    QString locale = QLocale::system().name();
    return locale;
}

void SuspensionFrame::get_sysc_data(QString upspeed, QString downspeed, QString ratio, int used_memory, QString free_memory, QString cpu_ratio) {
    ratio_sus = ratio.toInt();
    ui->uplabel->setText(upspeed);
    ui->downlabel->setText(downspeed);
    ui->ratiolabel->setText(ratio + "%");
    ui->ratiolabel_2->setText(cpu_ratio + "%");
    ui->memory_1->setText(tr("%1").arg(used_memory)+"M");
    ui->memory_2->setText(free_memory+"M");
    update();
}

void SuspensionFrame::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }

}

void SuspensionFrame::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton ) {
        move(event->globalPos() - dragPos);
        setWindowOpacity(0.5);
    }
    event->accept();
}

void SuspensionFrame::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        setWindowOpacity(1);
    }
    event->accept();
}

void SuspensionFrame::on_descBtn_clicked() {
    if(this->height() == 173) {
        this->resize(265, 288);
    }
    else if(this->height() == 288) {
        this->resize(265, 173);
    }
}

void SuspensionFrame::on_fastBtn_clicked() {
    emit accelerate_memory();
}

QSize SuspensionFrame::sizeHint()const {
    return QSize(height(),height());
}

void SuspensionFrame::resizeEvent(QResizeEvent *event) {
    wheel=QImage(event->size(),QImage::Format_ARGB32_Premultiplied);
    wheel.fill(palette().background().color());
    update();
}

void SuspensionFrame::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QStyleOption opt;
    QPixmap memory;
    QPixmap background;
    background.load(":/pixmap/image/accelerate-bg.png");
    memory.load(":/pixmap/image/memory.png");
    painter.drawPixmap(0,0, background);

    painter.setRenderHint(QPainter::Antialiasing);  //消除锯齿
    wheel.fill(Qt::transparent);
    blister.load(":/pixmap/image/blister-big.png");
    //线性渐变
    QLinearGradient linearGradient(76,10,76,76);
    //创建了一个QLinearGradient对象实例，参数为起点和终点坐标，可作为颜色渐变的方向
    painter.setPen(Qt::transparent);
    QString color1;
    QString color2;
    QString color3;

    color1=(ratio_sus == 100) ? "#ff2f00" : "transparent";
    if (ratio_sus == 0)
    {
        color2="transparent";
        color3="transparent";
    }
    else if(ratio_sus > 80) {
        color2="#d13625";
        color3="#d5311e";
        ui->title1->setText(tr("Computer runs slowly"));//电脑运行缓慢
        ui->title2->setText(tr("Using 'quick clean' ?"));//使用一键加速？
    }
    else {
        color2="#00b0ff";
        color3="#006eff";
        ui->title1->setText(tr("System runs smoothly"));//系统运行流畅
        ui->title2->setText(tr("No need to accelerate"));//无需进行加速
//        blister.load(":/pixmap/image/blister-big.png");
    }

    linearGradient.setColorAt(0.0,color1);
    linearGradient.setColorAt( 1.0 - ratio_sus * 0.01,color1);
    linearGradient.setColorAt((ratio_sus <= 0) ? 0.0 : (1.0 - ratio_sus * 0.01 + 0.01),color2);
    linearGradient.setColorAt(1.0,color3);
    painter.setBrush(QBrush(linearGradient));
    painter.drawEllipse(7,7,65,65);
    opt.init(this);
    painter.drawImage(0,0,wheel);
    painter.drawPixmap(44,231,memory,0,0,memory.width()*(ratio_sus* 0.01),memory.height());
    painter.drawPixmap(7,7, blister);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&painter,this);
}
