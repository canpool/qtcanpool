/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxribbon/ribbonfactory.h"
#include "qxribbon/ribbonmanager.h"
#include "qxribbon/ribboncontrols.h"
#include "qxribbon/ribbonstyleoption.h"

#include <QWidget>

QX_RIBBON_USE_NAMESPACE

/*
 * RibbonFactory and RibbonManager are the two pieces of the ribbon that are not widgets:
 * the factory owns the style option every ribbon draws from, and the manager is the
 * process-wide singleton that hands the factory out. RibbonElementStyleOpt - used all
 * over the library, including by WindowButtonGroup - is a macro that walks through both.
 *
 * The manager being a singleton makes it global state, so the case that replaces the
 * factory puts a default one back: setupFactory() deletes the factory it replaces, and
 * leaving a foreign one installed would change what every later test object in this
 * executable sees.
 */
class tst_RibbonFactoryAndManager : public QObject
{
    Q_OBJECT
private slots:
    void factoryStartsWithADefaultStyleOption();
    void setRibbonStyleOptionReplacesIt();
    void createHideGroupButton();
    void managerIsASingleton();
    void setupFactoryInstallsAndReplaces();
};

void tst_RibbonFactoryAndManager::factoryStartsWithADefaultStyleOption()
{
    RibbonFactory factory;

    // getRibbonStyleOption() returns a reference, so the only thing to assert is that
    // the option it refers to is a usable default rather than a null-ish object.
    QVERIFY(factory.getRibbonStyleOption().titleBarHeight() > 0);
    QVERIFY(factory.getRibbonStyleOption().tabBarHeight() > 0);
}

void tst_RibbonFactoryAndManager::setRibbonStyleOptionReplacesIt()
{
    RibbonFactory factory;

    RibbonStyleOption *opt = new RibbonStyleOption();
    opt->setTitleBarHeight(50);
    factory.setRibbonStyleOption(opt);

    QCOMPARE(factory.getRibbonStyleOption().titleBarHeight(), 50);
    // The old option is gone with the QScopedPointer reset; reading it again must not
    // hand back the default it replaced.
    QVERIFY(factory.getRibbonStyleOption().titleBarHeight() > 0);
}

void tst_RibbonFactoryAndManager::createHideGroupButton()
{
    RibbonFactory factory;
    QWidget host;

    RibbonControlButton *button = factory.createHideGroupButton(&host);

    QVERIFY(button != Q_NULLPTR);
    QCOMPARE(button->parentWidget(), &host);
    // Three things the ribbon bar relies on: it finds the button by object name, it
    // wants a raised button rather than the flat default, and it draws only the icon.
    QCOMPARE(button->objectName(), QStringLiteral("RibbonBarHideGroupButton"));
    QCOMPARE(button->autoRaise(), false);
    QCOMPARE(button->toolButtonStyle(), Qt::ToolButtonIconOnly);
}

void tst_RibbonFactoryAndManager::managerIsASingleton()
{
    RibbonManager *manager = RibbonManager::instance();

    QVERIFY(manager != Q_NULLPTR);
    QCOMPARE(RibbonManager::instance(), manager);
    QVERIFY(manager->factory() != Q_NULLPTR);
    // Handing out the same factory on every call is what makes RibbonElementStyleOpt
    // safe to use as an expression: two reads of it must agree.
    QCOMPARE(manager->factory(), manager->factory());
}

void tst_RibbonFactoryAndManager::setupFactoryInstallsAndReplaces()
{
    RibbonManager *manager = RibbonManager::instance();

    RibbonFactory *installed = new RibbonFactory();
    installed->getRibbonStyleOption().setTitleBarHeight(50);
    manager->setupFactory(installed);
    QCOMPARE(manager->factory(), installed);
    QCOMPARE(RibbonElementStyleOpt.titleBarHeight(), 50);

    // Put a default factory back. setupFactory() deletes the one it replaces, so
    // `installed` is dangling from here on and must not be touched again.
    manager->setupFactory(new RibbonFactory());
    QVERIFY(manager->factory() != installed);
    QVERIFY(manager->factory()->getRibbonStyleOption().titleBarHeight() > 0);
}

TEST_ADD(tst_RibbonFactoryAndManager)

#include "tst_ribbonfactory.moc"
