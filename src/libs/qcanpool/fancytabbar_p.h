#ifndef FANCYTABBAR_P_H
#define FANCYTABBAR_P_H

#include <QObject>

#include "fancytabbar.h"

class QBoxLayout;
class FancyTab;

class FancyTabBarPrivate : public QObject
{
    Q_OBJECT
public:
    FancyTabBarPrivate();
    ~FancyTabBarPrivate();
    void updateTabBarPosition();
    void init();

    FancyTabBar::Direction m_direction;

    int m_currentIndex;
    QList<FancyTab*> m_modeTabs;
    QList<FancyTab*> m_actionTabs;
    QMap<QAction*, FancyTab*> m_actionTabMap;

    QBoxLayout *m_layout;
    QBoxLayout *m_modeLayout;
    QBoxLayout *m_menuModeLayout;
    QBoxLayout *m_frontActionLayout;
    QBoxLayout *m_middleActionLayout;
    QBoxLayout *m_backActionLayout;

    QWidget *m_headSpacer;
    QWidget *m_splitLine;

    FancyTabBar::TabStyle m_modeStyle;
    FancyTabBar::TabStyle m_actionStyle;

    FancyTabBar *q;

signals:

public slots:
    void switchTab();
    void menuTriggered(QMouseEvent *e);
};

#endif // FANCYTABBAR_P_H
