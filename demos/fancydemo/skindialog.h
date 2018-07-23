/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#ifndef SKINDIALOG_H
#define SKINDIALOG_H

#include "qcanpool/fancydialog.h"
#include <QMap>

class TinyTabWidget;
class SkinItem;
class QComboBox;
class SkinDialog : public FancyDialog
{
    Q_OBJECT
public:
    explicit SkinDialog(QWidget *parent = 0);

private:
    void createWindow();

    QString skinName();

signals:
    void changeSkin(QString skinName);
    void themeChanged();

public slots:

private slots:
    void slotItemClicked();
    void slotThemeChanged(int index);

private:
    TinyTabWidget *m_pTabWidget;
    SkinItem *m_pLastSkinItem;
    QList<SkinItem*> m_skinItemList;

    QComboBox *m_pThemeComboBox;
    QMap<QString, QString> m_themeMap; // themeName, fileName
};

#endif // SKINDIALOG_H
