/*
    Copyright 2014 Ilya Zhuravlev

    This file is part of Acquisition.

    Acquisition is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Acquisition is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Acquisition.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <QDialog>
#include <string>
#include <vector>

namespace Ui {
class TabBuyoutsDialog;
}

class MainWindow;
class QSignalMapper;

class TabBuyoutsDialog : public QDialog
{
    Q_OBJECT

public:
    TabBuyoutsDialog(QWidget *parent, MainWindow *app);
    ~TabBuyoutsDialog();
    void Populate();

private slots:
    void on_pushButton_clicked();
    void OnBuyoutChanged(int row);

private:
    MainWindow *app_;
    Ui::TabBuyoutsDialog *ui;
    std::vector<std::string> tab_titles_;
    QSignalMapper *signal_mapper_;
};
