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
#include "skindialog.h"
#include "cwidgets.h"
#include "theme.h"
#include "explorer.h"

#include "qcanpool/tinytabwidget.h"
#include "qcanpool/skinitem.h"
#include "qcanpool/centralarea.h"

#define SKIN_NUM           15
#define SKIN_RES_NAME(n)   QString(":/skin/%1").arg(n)

SkinDialog::SkinDialog(QWidget *parent)
    : FancyDialog(parent)
{
    m_pTabWidget = 0;
    m_pLastSkinItem = 0;
    m_skinItemList.clear();
    m_pThemeComboBox = 0;
    m_themeMap.clear();

    createWindow();

    setFixedSize(800, 600); //
    setWindowTitle(tr("Change Skin Center"));
    setWindowIcon(QIcon(":/main/logo"));
}

void SkinDialog::createWindow()
{
    QWidget *pCentralWidget = new QWidget();

    m_pTabWidget = new TinyTabWidget();
    m_pTabWidget->setHoverColor(Theme::g_tinyTabHoverColor);
    m_pTabWidget->setPressColor(Theme::g_tinyTabPressColor);
    m_pTabWidget->setTextColor(Theme::g_tinyTabTextColor);
    m_pTabWidget->setSelectedTextColor(Theme::g_tinyTabSelectedTextColor);
    m_pTabWidget->setBackgroundColor(Theme::g_tinyTabBackgroundColor);
    m_pTabWidget->setNormalColor(Theme::g_tinyTabNormalColor);
    m_pTabWidget->setSlideColor(Theme::g_tinyTabSlideColor);

    CentralArea *area = new CentralArea();
    m_pTabWidget->addTab(area, tr("All page"));

    QWidget *allPageWidget = new QWidget();
    QGridLayout *pGridLayout = new QGridLayout();
    pGridLayout->setSpacing(15);
    pGridLayout->setContentsMargins(20, 10, 10, 10);
    allPageWidget->setLayout(pGridLayout);
    area->setWidget(allPageWidget);

    QString currentSkin = skinName();
    QStringList skinNameList;
    for(int i = 0; i < SKIN_NUM; i++)
    {
        skinNameList<<SKIN_RES_NAME(i);
    }
    for(int i = 0; i < SKIN_NUM; i++){
        SkinItem *item = new SkinItem();
        item->setMinimumHeight(150);
        item->setPixmapName(skinNameList.at(i));
        if(currentSkin == skinNameList.at(i)){
            m_pLastSkinItem = item;
        }
        m_skinItemList.append(item);
        connect(item, SIGNAL(clicked()), this, SLOT(slotItemClicked()));
        pGridLayout->addWidget(item, i/3, i%3);
    }

    ///
    m_pThemeComboBox = new QComboBox();
    QFileInfoList themeFiles = Explorer::getThemeFiles();
    foreach (QFileInfo info, themeFiles) {
        QString fileName = info.absoluteFilePath();
        QString themeName = Theme::themeName(fileName);
        m_pThemeComboBox->addItem(themeName, fileName);
    }
    QString currentThemeName = Theme::themeName(Theme::themeFileName());
    int index = m_pThemeComboBox->findText(currentThemeName);
    if(index != -1){
        m_pThemeComboBox->setCurrentIndex(index);
    }

    connect(m_pThemeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(slotThemeChanged(int)));
    m_pThemeComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QHBoxLayout *themeLayout = new QHBoxLayout();
    themeLayout->setContentsMargins(10, 5, 10, 5);
    themeLayout->setSpacing(10);
    themeLayout->addWidget(new QLabel(tr("Theme:")));
    themeLayout->addWidget(m_pThemeComboBox);
    themeLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_pTabWidget);
    layout->addLayout(themeLayout);
    pCentralWidget->setLayout(layout);
    setCentralWidget(pCentralWidget);

    // init
    if(!m_pLastSkinItem){
        m_pLastSkinItem = m_skinItemList.at(0);
    }
    m_pLastSkinItem->select(true);
    emit changeSkin(m_pLastSkinItem->pixmapName());
}

QString SkinDialog::skinName()
{
    QSettings settings("Canpool", "qtcanpool");
    QString skinName = settings.value("skin").toString();
    return skinName;
}

void SkinDialog::slotItemClicked()
{
    SkinItem *item = qobject_cast<SkinItem *>(sender());

    if(m_pLastSkinItem == item) return;
    if(m_pLastSkinItem){
        m_pLastSkinItem->select(false);
    }
    m_pLastSkinItem = item;
    m_pLastSkinItem->select(true);

    QString skinName = item->pixmapName();

    update();
    emit changeSkin(skinName);
}

void SkinDialog::slotThemeChanged(int index)
{
    QString fileName = m_pThemeComboBox->itemData(index).toString();
    Theme::setTheme(fileName);
    emit themeChanged();
}
