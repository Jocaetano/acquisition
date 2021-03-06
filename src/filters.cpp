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

#include <QGroupBox>
#include <QLineEdit>

#include "filters.h"

const int FILTER_LABEL_WIDTH = 40;

FilterData* Filter::CreateData() {
    return new FilterData(this);
}

FilterData::FilterData(Filter *filter):
    filter_(filter),
    text_query(""),
    min_filled(false),
    max_filled(false),
    r_filled(false),
    g_filled(false),
    b_filled(false)
{}

bool FilterData::Matches(const std::shared_ptr<Item> item) {
    return filter_->Matches(item, this);
}

void FilterData::FromForm() {
    filter_->FromForm(this);
}

void FilterData::ToForm() {
    filter_->ToForm(this);
}

NameSearchFilter::NameSearchFilter(QLayout *parent) {
    Initialize(parent);
}

void NameSearchFilter::FromForm(FilterData *data) {
    data->text_query = textbox_->text().toUtf8().constData();
}

void NameSearchFilter::ToForm(FilterData *data) {
    textbox_->setText(data->text_query.c_str());
}

void NameSearchFilter::ResetForm() {
    textbox_->setText("");
}

bool NameSearchFilter::Matches(const std::shared_ptr<Item> &item, FilterData *data) {
    std::string query = data->text_query;
    std::string name = item->PrettyName();
    std::transform(query.begin(), query.end(), query.begin(), ::tolower);
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    return name.find(query) != std::string::npos;
}

void NameSearchFilter::Initialize(QLayout *parent) {
    textbox_ = new QLineEdit;
    parent->addWidget(textbox_);
    QObject::connect(textbox_, SIGNAL(textEdited(const QString&)),
                     parent->parentWidget()->window(), SLOT(OnSearchFormChange()));
}

MinMaxFilter::MinMaxFilter(QLayout *parent, std::string property):
    property_(property),
    caption_(property)
{
    Initialize(parent);
}

MinMaxFilter::MinMaxFilter(QLayout *parent, std::string property, std::string caption):
    property_(property),
    caption_(caption)
{
    Initialize(parent);
}

void MinMaxFilter::Initialize(QLayout *parent) {
    QWidget *group = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    QLabel *label = new QLabel(caption_.c_str());
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    textbox_min_ = new QLineEdit;
    textbox_max_ = new QLineEdit;
    layout->addWidget(label);
    layout->addWidget(textbox_min_);
    layout->addWidget(textbox_max_);
    group->setLayout(layout);
    parent->addWidget(group);
    textbox_min_->setPlaceholderText("min");
    textbox_max_->setPlaceholderText("max");
    textbox_min_->setFixedWidth(30);
    textbox_max_->setFixedWidth(30);
    label->setFixedWidth(FILTER_LABEL_WIDTH);
    QObject::connect(textbox_min_, SIGNAL(textEdited(const QString&)),
                     parent->parentWidget()->window(), SLOT(OnSearchFormChange()));
    QObject::connect(textbox_max_, SIGNAL(textEdited(const QString&)),
                     parent->parentWidget()->window(), SLOT(OnSearchFormChange()));
}

void MinMaxFilter::FromForm(FilterData *data) {
    data->min_filled = textbox_min_->text().size() > 0;
    data->min = textbox_min_->text().toDouble();
    data->max_filled = textbox_max_->text().size() > 0;
    data->max = textbox_max_->text().toDouble();
}

void MinMaxFilter::ToForm(FilterData *data) {
    if (data->min_filled)
        textbox_min_->setText(QString::number(data->min));
    else
        textbox_min_->setText("");
    if (data->max_filled)
        textbox_max_->setText(QString::number(data->max));
    else
        textbox_max_->setText("");
}

void MinMaxFilter::ResetForm() {
    textbox_min_->setText("");
    textbox_max_->setText("");
}

bool MinMaxFilter::Matches(const std::shared_ptr<Item> &item, FilterData *data) {
    if (IsValuePresent(item)) {
        double value = GetValue(item);
        if (data->min_filled && data->min > value)
            return false;
        if (data->max_filled && data->max < value)
            return false;
        return true;
    } else {
        return !data->max_filled && !data->min_filled;
    }
}

bool SimplePropertyFilter::IsValuePresent(const std::shared_ptr<Item> &item) {
    return item->properties().count(property_);
}

double SimplePropertyFilter::GetValue(const std::shared_ptr<Item> &item) {
    return std::stod(item->properties().at(property_));
}

double RequiredStatFilter::GetValue(const std::shared_ptr<Item> &item) {
    auto &requirements = item->requirements();
    if (requirements.count(property_))
        return requirements.at(property_);
    return 0;
}

ItemMethodFilter::ItemMethodFilter(QLayout *parent, std::function<double (Item *)> func, std::string caption):
    MinMaxFilter(parent, caption, caption),
    func_(func)
{}

double ItemMethodFilter::GetValue(const std::shared_ptr<Item> &item) {
    return func_(&*item);
}

double SocketsFilter::GetValue(const std::shared_ptr<Item> &item) {
    return item->sockets();
}

double LinksFilter::GetValue(const std::shared_ptr<Item> &item) {
    return item->links();
}

SocketsColorsFilter::SocketsColorsFilter(QLayout *parent) {
    Initialize(parent, "Colors");
}

// TODO(xyz): ugh, a lot of copypasta below, perhaps this could be done
// in a nice way?
void SocketsColorsFilter::Initialize(QLayout *parent, const char* caption) {
    QWidget *group = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    QLabel *label = new QLabel(caption);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    textbox_r_ = new QLineEdit;
    textbox_r_->setPlaceholderText("R");
    textbox_g_ = new QLineEdit;
    textbox_g_->setPlaceholderText("G");
    textbox_b_ = new QLineEdit;
    textbox_b_->setPlaceholderText("B");
    layout->addWidget(label);
    layout->addWidget(textbox_r_);
    layout->addWidget(textbox_g_);
    layout->addWidget(textbox_b_);
    group->setLayout(layout);
    parent->addWidget(group);
    textbox_r_->setFixedWidth(25);
    textbox_g_->setFixedWidth(25);
    textbox_b_->setFixedWidth(25);
    label->setFixedWidth(FILTER_LABEL_WIDTH);
    QObject::connect(textbox_r_, SIGNAL(textEdited(const QString&)),
                     parent->parentWidget()->window(), SLOT(OnSearchFormChange()));
    QObject::connect(textbox_g_, SIGNAL(textEdited(const QString&)),
                     parent->parentWidget()->window(), SLOT(OnSearchFormChange()));
    QObject::connect(textbox_b_, SIGNAL(textEdited(const QString&)),
                     parent->parentWidget()->window(), SLOT(OnSearchFormChange()));
}

void SocketsColorsFilter::FromForm(FilterData *data) {
    data->r_filled = textbox_r_->text().size() > 0;
    data->g_filled = textbox_g_->text().size() > 0;
    data->b_filled = textbox_b_->text().size() > 0;
    data->r = textbox_r_->text().toInt();
    data->g = textbox_g_->text().toInt();
    data->b = textbox_b_->text().toInt();
}

void SocketsColorsFilter::ToForm(FilterData *data) {
    if (data->r_filled)
        textbox_r_->setText(QString::number(data->r));
    if (data->g_filled)
        textbox_g_->setText(QString::number(data->g));
    if (data->b_filled)
        textbox_b_->setText(QString::number(data->b));
}

void SocketsColorsFilter::ResetForm() {
    textbox_r_->setText("");
    textbox_g_->setText("");
    textbox_b_->setText("");
}

bool SocketsColorsFilter::Check(int need_r, int need_g, int need_b, int got_r, int got_g, int got_b, int got_w) {
    int diff = std::max(0, need_r - got_r) + std::max(0, need_g - got_g)
        + std::max(0, need_b - got_b);
    return diff <= got_w;
}

bool SocketsColorsFilter::Matches(const std::shared_ptr<Item> &item, FilterData *data) {
    if (!data->r_filled && !data->g_filled && !data->b_filled)
        return true;
    int need_r = 0, need_g = 0, need_b = 0;
    if (data->r_filled)
        need_r = data->r;
    if (data->g_filled)
        need_g = data->g;
    if (data->b_filled)
        need_b = data->b;
    return Check(need_r, need_g, need_b, item->sockets_r(), item->sockets_g(), item->sockets_b(),
        item->sockets_w());
}

LinksColorsFilter::LinksColorsFilter(QLayout *parent) {
    Initialize(parent, "Linked");
}

bool LinksColorsFilter::Matches(const std::shared_ptr<Item> &item, FilterData *data) {
    if (!data->r_filled && !data->g_filled && !data->b_filled)
        return true;
    int need_r = 0, need_g = 0, need_b = 0;
    if (data->r_filled)
        need_r = data->r;
    if (data->g_filled)
        need_g = data->g;
    if (data->b_filled)
        need_b = data->b;
    int current = 0, got_r = 0, got_g = 0, got_b = 0, got_w = 0;
    for (auto &socket : item->json()["sockets"]) {
        if (current != socket["group"].asInt()) {
            if (Check(need_r, need_g, need_b, got_r, got_g, got_b, got_w))
                return true;
            got_r = got_g = got_b = 0;
        }
        current = socket["group"].asInt();
        switch (socket["attr"].asString()[0]) {
        case 'S':
            ++got_r;
            break;
        case 'D':
            ++got_g;
            break;
        case 'I':
            ++got_b;
            break;
        case 'G':
            ++got_w;
            break;
        }
    }
    return Check(need_r, need_g, need_b, got_r, got_g, got_b, got_w);
}
