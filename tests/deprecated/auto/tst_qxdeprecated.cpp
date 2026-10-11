/*!
 * \file
 *
 * Tests for the QX_<LIB>_DEPRECATED_SINCE gate that all seven libraries in
 * src/libs carry in their *_global.h.
 *
 * The gate is the buffer between "deprecated" and "removed" (K26): a declaration
 * marked in version a.b stays while QX_<LIB>_DEPRECATED_SINCE(a, b) is true, and
 * raising QX_<LIB>_DISABLE_DEPRECATED_BEFORE to the version that removes it is
 * what compiles the declaration out.
 *
 * Two things were wrong with it until 4.3, and both are pinned here:
 *  - the default cutoff was the version being built, so the gate answered false
 *    for every deprecation that had already happened - (0,2,0) > (0,2,0) is
 *    false - and true only for versions that did not exist yet;
 *  - being tied to the version being built, the answer also moved on every minor
 *    bump, so a deprecation that warned in one release stopped warning in the
 *    next one.
 *
 * What raising the cutoff does is tested in tst_qxdeprecated_cutoff.cpp: the
 * cutoff is read when the header is included, so that case needs a translation
 * unit of its own.
 */
#include "tst_global.h"

#include "qxapp/qxapp_global.h"
#include "qxcore/qxcore_global.h"
#include "qxdock/qxdock_global.h"
#include "qxplugin/qxplugin_global.h"
#include "qxribbon/qxribbon_global.h"
#include "qxtheme/qxtheme_global.h"
#include "qxwindow/qxwindow_global.h"

// A gate that can only be read at run time would be useless: what it guards is a
// declaration, so the preprocessor is the one that has to see it.
#if !QX_APP_DEPRECATED_SINCE(0, 1) || !QX_CORE_DEPRECATED_SINCE(0, 1) || !QX_DOCK_DEPRECATED_SINCE(0, 2) ||            \
    !QX_PLUGIN_DEPRECATED_SINCE(0, 2) || !QX_RIBBON_DEPRECATED_SINCE(0, 11) || !QX_THEME_DEPRECATED_SINCE(0, 1) ||     \
    !QX_WINDOW_DEPRECATED_SINCE(0, 2)
#error "the deprecation gate has to be true for a version that has already shipped"
#endif

class tst_QxDeprecated : public QObject
{
    Q_OBJECT

private slots:
    void gateIsTrueForAVersionThatAlreadyShipped();
    void gateIsFalseBelowTheCutoff();
    void cutoffIsNotTheVersionBeingBuilt();
};

void tst_QxDeprecated::gateIsTrueForAVersionThatAlreadyShipped()
{
    // The regression itself: with the 4.2 default every one of these was false,
    // which is why the gate was never used and could not have worked if it had.
    QVERIFY(QX_APP_DEPRECATED_SINCE(QX_APP_VERSION_MAJOR, QX_APP_VERSION_MINOR));
    QVERIFY(QX_CORE_DEPRECATED_SINCE(QX_CORE_VERSION_MAJOR, QX_CORE_VERSION_MINOR));
    QVERIFY(QX_DOCK_DEPRECATED_SINCE(QX_DOCK_VERSION_MAJOR, QX_DOCK_VERSION_MINOR));
    QVERIFY(QX_PLUGIN_DEPRECATED_SINCE(QX_PLUGIN_VERSION_MAJOR, QX_PLUGIN_VERSION_MINOR));
    QVERIFY(QX_RIBBON_DEPRECATED_SINCE(QX_RIBBON_VERSION_MAJOR, QX_RIBBON_VERSION_MINOR));
    QVERIFY(QX_THEME_DEPRECATED_SINCE(QX_THEME_VERSION_MAJOR, QX_THEME_VERSION_MINOR));
    QVERIFY(QX_WINDOW_DEPRECATED_SINCE(QX_WINDOW_VERSION_MAJOR, QX_WINDOW_VERSION_MINOR));
}

void tst_QxDeprecated::gateIsFalseBelowTheCutoff()
{
    // The other side of the boundary: 0.0 is not a version any of these
    // libraries shipped, so a deprecation point there is already gone.
    QVERIFY(!QX_APP_DEPRECATED_SINCE(0, 0));
    QVERIFY(!QX_CORE_DEPRECATED_SINCE(0, 0));
    QVERIFY(!QX_DOCK_DEPRECATED_SINCE(0, 0));
    QVERIFY(!QX_PLUGIN_DEPRECATED_SINCE(0, 0));
    QVERIFY(!QX_RIBBON_DEPRECATED_SINCE(0, 0));
    QVERIFY(!QX_THEME_DEPRECATED_SINCE(0, 0));
    QVERIFY(!QX_WINDOW_DEPRECATED_SINCE(0, 0));
}

void tst_QxDeprecated::cutoffIsNotTheVersionBeingBuilt()
{
    // The 4.2 default was QX_<LIB>_VERSION, and that single line is what made the
    // whole mechanism unusable. Raising the default is a deliberate act that goes
    // with removing an API, so a test failing here is the point, not a nuisance.
    QVERIFY(QX_APP_DISABLE_DEPRECATED_BEFORE != QX_APP_VERSION);
    QVERIFY(QX_CORE_DISABLE_DEPRECATED_BEFORE != QX_CORE_VERSION);
    QVERIFY(QX_DOCK_DISABLE_DEPRECATED_BEFORE != QX_DOCK_VERSION);
    QVERIFY(QX_PLUGIN_DISABLE_DEPRECATED_BEFORE != QX_PLUGIN_VERSION);
    QVERIFY(QX_RIBBON_DISABLE_DEPRECATED_BEFORE != QX_RIBBON_VERSION);
    QVERIFY(QX_THEME_DISABLE_DEPRECATED_BEFORE != QX_THEME_VERSION);
    QVERIFY(QX_WINDOW_DISABLE_DEPRECATED_BEFORE != QX_WINDOW_VERSION);
}

TEST_ADD(tst_QxDeprecated)

#include "tst_qxdeprecated.moc"
