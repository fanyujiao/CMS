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

#ifndef FCITXWARNDIALOG_H
#define FCITXWARNDIALOG_H

#include <QDialog>
#include <fcitxwarndialog.h>

namespace Ui {
class FcitxWarnDialog;
}

class FcitxWarnDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FcitxWarnDialog(QWidget *parent = 0);
    ~FcitxWarnDialog();

signals:
    void fcitxWarntest();
public slots:
    void on_okButton_clicked();

private:
    Ui::FcitxWarnDialog *ui;
    QPoint dragPos;
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // FCITXWARNDIALOG_H




