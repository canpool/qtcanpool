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
#ifndef FANCYRIBBON_H
#define FANCYRIBBON_H

#include <QWidget>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyRibbonPage;
class FancyRibbonPrivate;

class QCANPOOL_SHARED_EXPORT FancyRibbon : public QWidget
{
    Q_OBJECT
public:
    explicit FancyRibbon(QWidget *parent = nullptr);
    ~FancyRibbon();

public:
    FancyRibbonPage *addPage(const QString &text);
    void addPage(FancyRibbonPage *page);
    FancyRibbonPage *insertPage(int index, const QString &text);
    void insertPage(int index, FancyRibbonPage *page);

    void removePage(FancyRibbonPage *page);
    void removePage(int index);

    void clearPages();

    FancyRibbonPage *page(int index) const;
    int pageCount() const;
    int currentPageIndex() const;
    QList<FancyRibbonPage *> pages() const;

    int addAction(QAction *action);

    void setHoverColor(const QColor &color);
    void setPressColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setSelectedTextColor(const QColor &color);
    void setBackgroundColor(const QColor &color);

private:
    FancyRibbonPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYRIBBON_H
