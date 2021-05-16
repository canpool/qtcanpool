/***************************************************************************
 **
 **  Copyright (C) 2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#include "fancyribbonpage.h"
#include "fancyribbongroup.h"
#include <QHBoxLayout>

QCANPOOL_BEGIN_NAMESPACE

class FancyRibbonPagePrivate : public QObject
{
    Q_OBJECT
public:
    FancyRibbonPagePrivate();
    ~FancyRibbonPagePrivate();

    void init();

    bool validateIndex(int index) const
    { return index >= 0 && index < m_listGroups.count(); }

    void insertGroup(int index, FancyRibbonGroup *group);
    void removeGroup(int index);
    int groupIndex(FancyRibbonGroup *group);

public:
    FancyRibbonPage *q;
    QHBoxLayout *m_layout;

    QList<FancyRibbonGroup *> m_listGroups;

    QString m_title;
};

FancyRibbonPagePrivate::FancyRibbonPagePrivate()
    : m_title(QString("page"))
{}

FancyRibbonPagePrivate::~FancyRibbonPagePrivate()
{}

void FancyRibbonPagePrivate::init()
{
    m_layout = new QHBoxLayout();
    m_layout->setSpacing(1);
    m_layout->setMargin(0);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addLayout(m_layout);
    mainLayout->addStretch();

    q->setLayout(mainLayout);
}

void FancyRibbonPagePrivate::insertGroup(int index, FancyRibbonGroup *group)
{
    if (validateIndex(index)) {
        m_listGroups.insert(index, group);
        m_layout->insertWidget(index, group);
    } else {
        m_listGroups.append(group);
        m_layout->addWidget(group);
    }
}

void FancyRibbonPagePrivate::removeGroup(int index)
{
    if (validateIndex(index)) {
        FancyRibbonGroup *group = m_listGroups.at(index);
        m_listGroups.removeAt(index);
        m_layout->removeWidget(group);
    }
}

int FancyRibbonPagePrivate::groupIndex(FancyRibbonGroup *group)
{
    return m_listGroups.indexOf(group);
}

FancyRibbonPage::FancyRibbonPage(QWidget *parent)
    : QWidget(parent)
    , d(new FancyRibbonPagePrivate)
{
    d->q = this;
    d->init();
}

FancyRibbonPage::~FancyRibbonPage()
{
    delete d;
}

void FancyRibbonPage::addGroup(FancyRibbonGroup *group)
{
    insertGroup(-1, group);
}

FancyRibbonGroup *FancyRibbonPage::addGroup(const QString &title)
{
    return insertGroup(-1, title);
}

void FancyRibbonPage::insertGroup(int index, FancyRibbonGroup *group)
{
    d->insertGroup(index, group);
}

FancyRibbonGroup *FancyRibbonPage::insertGroup(int index, const QString &title)
{
    FancyRibbonGroup *group = new FancyRibbonGroup(title);
    insertGroup(index, group);
    return group;
}

void FancyRibbonPage::removeGroup(FancyRibbonGroup *group)
{
    removeGroup(d->groupIndex(group));
}

void FancyRibbonPage::removeGroup(int index)
{
    d->removeGroup(index);
}

void FancyRibbonPage::clearGroups()
{
    for (int i = d->m_listGroups.count() - 1; i >= 0; i--) {
        removeGroup(i);
    }
}

QString FancyRibbonPage::title() const
{
    return d->m_title;
}

void FancyRibbonPage::setBackgroundColor(const QColor &color)
{
    QPalette palette;
    palette.setColor(QPalette::Window, color);
    this->setPalette(palette);
    this->setAutoFillBackground(true);
}

void FancyRibbonPage::setTitle(const QString &title)
{
    d->m_title = title;
}

QCANPOOL_END_NAMESPACE

#include "fancyribbonpage.moc"
