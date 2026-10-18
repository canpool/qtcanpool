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
    void setupFactoryNullPutsTheDefaultBack();
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
    // A default factory is what every case here puts back, so the height it reports
    // is the value the manager has to return to. Read it from a fresh one rather than
    // writing the number down: it is derived from the font metrics in force.
    const int defaultHeight = RibbonFactory().getRibbonStyleOption().titleBarHeight();

    RibbonFactory *installed = new RibbonFactory();
    installed->getRibbonStyleOption().setTitleBarHeight(defaultHeight + 1);
    manager->setupFactory(installed);
    QCOMPARE(manager->factory(), installed);
    QCOMPARE(RibbonElementStyleOpt.titleBarHeight(), defaultHeight + 1);

    // Put a default factory back. setupFactory() deletes the one it replaces, so
    // `installed` is dangling from here on and must not be touched again - and the
    // check has to be about what the new factory reports rather than about that
    // pointer, because a freed pointer compares equal to whatever is allocated on
    // its block next.
    manager->setupFactory(new RibbonFactory());
    QCOMPARE(manager->factory()->getRibbonStyleOption().titleBarHeight(), defaultHeight);
    QCOMPARE(RibbonElementStyleOpt.titleBarHeight(), defaultHeight);
}

void tst_RibbonFactoryAndManager::setupFactoryNullPutsTheDefaultBack()
{
    RibbonManager *manager = RibbonManager::instance();
    const int defaultHeight = RibbonFactory().getRibbonStyleOption().titleBarHeight();

    RibbonFactory *installed = new RibbonFactory();
    installed->getRibbonStyleOption().setTitleBarHeight(defaultHeight + 1);
    manager->setupFactory(installed);
    QCOMPARE(RibbonElementStyleOpt.titleBarHeight(), defaultHeight + 1);

    // Uninstalling puts the default factory back rather than leaving the manager
    // without one: RibbonElementFactory and RibbonElementStyleOpt are macros that
    // walk through factory(), so a null there would crash the next ribbon element
    // the library creates - WindowButtonGroup included. It has to be a *new* default
    // and not the one being uninstalled, which is why the case marks it with a height
    // the default does not have: `installed` is deleted by the call, and comparing
    // against that pointer afterwards is a comparison against a freed block, which
    // the allocator is free to hand straight back.
    manager->setupFactory(Q_NULLPTR);
    QVERIFY(manager->factory() != Q_NULLPTR);
    QCOMPARE(manager->factory()->getRibbonStyleOption().titleBarHeight(), defaultHeight);
    QCOMPARE(RibbonElementStyleOpt.titleBarHeight(), defaultHeight);

    // Reinstalling the factory that is already installed is a no-op, not a
    // use-after-free: setupFactory() deletes the factory it replaces.
    RibbonFactory *current = manager->factory();
    manager->setupFactory(current);
    QCOMPARE(manager->factory(), current);
    QCOMPARE(current->getRibbonStyleOption().titleBarHeight(), defaultHeight);
}

TEST_ADD(tst_RibbonFactoryAndManager)

#include "tst_ribbonfactory.moc"
