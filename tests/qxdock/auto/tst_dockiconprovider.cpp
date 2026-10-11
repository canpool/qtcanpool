/*!
 * \file
 *
 * DockIconProvider is the one class in qxdock that is pure bookkeeping: a table
 * with one slot per Qx::DockIcon, handed out to whoever draws a title bar or a
 * tab. It is also the only one of the eight uncovered exported classes that can
 * be driven end to end without a window, so the tests read like a container.
 *
 * What is worth pinning is the part a caller can get wrong: the table starts
 * empty, registering one icon does not disturb the others, and registering again
 * replaces rather than accumulates.
 */
#include "tst_global.h"

#include "qxdock/dockiconprovider.h"

#include <QPixmap>

QX_DOCK_USE_NAMESPACE

class tst_DockIconProvider : public QObject
{
    Q_OBJECT

private slots:
    void nothingIsRegisteredByDefault();
    void registeringOneIconLeavesTheOthersAlone();
    void registeringAgainReplaces();
};

void tst_DockIconProvider::nothingIsRegisteredByDefault()
{
    DockIconProvider provider;

    for (int i = 0; i < Qx::NDockIconSizes; ++i) {
        QVERIFY(provider.customIcon(static_cast<Qx::DockIcon>(i)).isNull());
    }
}

void tst_DockIconProvider::registeringOneIconLeavesTheOthersAlone()
{
    DockIconProvider provider;

    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::red);
    const QIcon icon(pixmap);

    provider.registerCustomIcon(Qx::DockTabCloseIcon, icon);

    QVERIFY(!provider.customIcon(Qx::DockTabCloseIcon).isNull());
    QCOMPARE(provider.customIcon(Qx::DockTabCloseIcon).actualSize(QSize(32, 32)), QSize(16, 16));

    // A QIcon is implicitly shared, so the neighbouring slots staying empty is
    // what proves the write went to one slot and not into shared state.
    QVERIFY(provider.customIcon(Qx::DockPanelMenuIcon).isNull());
    QVERIFY(provider.customIcon(Qx::DockPanelCloseIcon).isNull());
}

void tst_DockIconProvider::registeringAgainReplaces()
{
    DockIconProvider provider;

    QPixmap first(16, 16);
    first.fill(Qt::red);
    QPixmap second(32, 32);
    second.fill(Qt::blue);

    provider.registerCustomIcon(Qx::DockAutoHideIcon, QIcon(first));
    QCOMPARE(provider.customIcon(Qx::DockAutoHideIcon).actualSize(QSize(64, 64)), QSize(16, 16));

    provider.registerCustomIcon(Qx::DockAutoHideIcon, QIcon(second));
    QCOMPARE(provider.customIcon(Qx::DockAutoHideIcon).actualSize(QSize(64, 64)), QSize(32, 32));

    // An empty icon is a value, not a no-op: it clears the slot again.
    provider.registerCustomIcon(Qx::DockAutoHideIcon, QIcon());
    QVERIFY(provider.customIcon(Qx::DockAutoHideIcon).isNull());
}

TEST_ADD(tst_DockIconProvider)

#include "tst_dockiconprovider.moc"
