#include "tst_global.h"

#include "qxribbon/ribboncontainers.h"

#include <QLabel>
#include <QWidget>

QX_RIBBON_USE_NAMESPACE

/*
 * RibbonCtrlContainer wraps one arbitrary widget with an icon and a title, laid out to
 * their left (horizontal) or above them (vertical). The header is two QLabels owned by
 * the container and named "logo" and "title", which is how a stylesheet reaches them and
 * how this test reaches them too.
 *
 * The container takes ownership of the widget handed to setWidget(): it reparents it, and
 * replacing it deletes the old one through deleteLater().
 */
class tst_RibbonCtrlContainer : public QObject
{
    Q_OBJECT
private:
    static QLabel *header(RibbonCtrlContainer *container, const QString &name)
    {
        return container->findChild<QLabel *>(name);
    }

private slots:
    void defaultsToHorizontalAndEmpty();
    void setWidgetTakesOwnership();
    void replacingTheWidgetSwapsIt();
    void textAndIconRoundTrip();
    void iconSizeDefaultAndRoundTrip();
    void constructorOrientationAgreesWithSetOrientation();
    void orientationDrivesTheHeaderAlignment();
    void visibilityTogglesReachTheHeader();
};

void tst_RibbonCtrlContainer::defaultsToHorizontalAndEmpty()
{
    RibbonCtrlContainer container;

    QCOMPARE(container.orientation(), Qt::Horizontal);
    QVERIFY(container.widget() == Q_NULLPTR);
    QVERIFY(container.text().isEmpty());
    QVERIFY(container.icon().isNull());
}

void tst_RibbonCtrlContainer::setWidgetTakesOwnership()
{
    RibbonCtrlContainer container;
    QWidget *widget = new QWidget;

    container.setWidget(widget);

    QCOMPARE(container.widget(), widget);
    // Reparenting is what makes the widget follow the container's geometry and die with
    // it; a caller that only stores the pointer would silently leak the widget.
    QCOMPARE(widget->parentWidget(), &container);
}

void tst_RibbonCtrlContainer::replacingTheWidgetSwapsIt()
{
    RibbonCtrlContainer container;
    container.setWidget(new QWidget);

    QWidget *second = new QWidget;
    container.setWidget(second);

    QCOMPARE(container.widget(), second);
    QCOMPARE(second->parentWidget(), &container);
    // The replaced widget is disposed of with deleteLater(), so it is gone by the time
    // control returns to the event loop.
}

void tst_RibbonCtrlContainer::textAndIconRoundTrip()
{
    RibbonCtrlContainer container;

    container.setText(QStringLiteral("Font"));
    QCOMPARE(container.text(), QStringLiteral("Font"));

    QVERIFY(container.icon().isNull());
    container.setIcon(QIcon(QStringLiteral(":/qxribbon/res/group_option.png")));
    QVERIFY(!container.icon().isNull());
}

void tst_RibbonCtrlContainer::iconSizeDefaultAndRoundTrip()
{
    RibbonCtrlContainer container;

    QCOMPARE(container.iconSize(), QSize(18, 18));

    container.setIconSize(QSize(32, 32));
    QCOMPARE(container.iconSize(), QSize(32, 32));
}

void tst_RibbonCtrlContainer::constructorOrientationAgreesWithSetOrientation()
{
    // Building the container vertical has to land in the same state as switching it
    // vertical later: same orientation, same icon alignment.
    RibbonCtrlContainer vertical(Qt::Vertical);
    QCOMPARE(vertical.orientation(), Qt::Vertical);
    QCOMPARE(header(&vertical, QStringLiteral("logo"))->alignment(), Qt::Alignment(Qt::AlignLeft));

    RibbonCtrlContainer horizontal;
    QCOMPARE(horizontal.orientation(), Qt::Horizontal);
    QCOMPARE(header(&horizontal, QStringLiteral("logo"))->alignment(), Qt::Alignment(Qt::AlignCenter));
}

void tst_RibbonCtrlContainer::orientationDrivesTheHeaderAlignment()
{
    RibbonCtrlContainer container;
    QLabel *logo = header(&container, QStringLiteral("logo"));
    QVERIFY(logo != Q_NULLPTR);

    QCOMPARE(container.orientation(), Qt::Horizontal);
    QCOMPARE(logo->alignment(), Qt::Alignment(Qt::AlignCenter));

    // Vertical stacks the header above the widget, where a centred icon floats in the
    // middle of a full-width row - hence the switch to left.
    container.setOrientation(Qt::Vertical);
    QCOMPARE(container.orientation(), Qt::Vertical);
    QCOMPARE(logo->alignment(), Qt::Alignment(Qt::AlignLeft));

    container.setOrientation(Qt::Horizontal);
    QCOMPARE(container.orientation(), Qt::Horizontal);
    QCOMPARE(logo->alignment(), Qt::Alignment(Qt::AlignCenter));
}

void tst_RibbonCtrlContainer::visibilityTogglesReachTheHeader()
{
    RibbonCtrlContainer container;
    QLabel *logo = header(&container, QStringLiteral("logo"));
    QLabel *title = header(&container, QStringLiteral("title"));
    QVERIFY(logo != Q_NULLPTR);
    QVERIFY(title != Q_NULLPTR);

    QVERIFY(!logo->isHidden());
    QVERIFY(!title->isHidden());

    container.setIconVisible(false);
    QVERIFY(logo->isHidden());
    QVERIFY(!title->isHidden());

    container.setTitleVisible(false);
    QVERIFY(logo->isHidden());
    QVERIFY(title->isHidden());
}

TEST_ADD(tst_RibbonCtrlContainer)

#include "tst_ribbonctrlcontainer.moc"
