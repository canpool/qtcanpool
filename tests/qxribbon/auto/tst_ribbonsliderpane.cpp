#include "tst_global.h"

#include "qxribbon/ribbonsliderpane.h"

#include <QSignalSpy>
#include <QSlider>
#include <QToolButton>

QX_RIBBON_USE_NAMESPACE

namespace
{

/// A window with the zoom pane in it, the smallest host the pane can be asked to
/// lay its three parts out in.
struct SliderPaneShell {
    QWidget window;
    RibbonSliderPane *pane;

    SliderPaneShell()
        : pane(new RibbonSliderPane(&window))
    {
        window.resize(300, 60);
        window.show();
    }
};

}   // namespace

class tst_RibbonSliderPane : public QObject
{
    Q_OBJECT
private slots:
    void objectNames();
    void rangeAndValue();
    void scrollButtons();
    void scrollButtonsToggle();
    void sliderMovedForwarded();
};

void tst_RibbonSliderPane::objectNames()
{
    SliderPaneShell shell;

    // The names a theme stylesheet targets; a rename here is a theme change.
    QCOMPARE(shell.pane->objectName(), QStringLiteral("qx_RibbonSliderPane"));
    QVERIFY(shell.pane->findChild<QSlider *>(QStringLiteral("qx_RibbonSliderPaneSlider")) != nullptr);
    QCOMPARE(shell.pane->findChildren<QToolButton *>(QStringLiteral("qx_RibbonSliderPaneButton")).count(), 2);
}

void tst_RibbonSliderPane::rangeAndValue()
{
    SliderPaneShell shell;
    shell.pane->setRange(10, 500);
    QSignalSpy changed(shell.pane, &RibbonSliderPane::valueChanged);

    // Ranging before the first value clamps the initial 0 onto the new floor.
    QCOMPARE(shell.pane->value(), 10);
    shell.pane->setValue(100);
    QCOMPARE(shell.pane->value(), 100);
    QCOMPARE(changed.count(), 1);
    QCOMPARE(changed.takeFirst().at(0).toInt(), 100);

    // Both ends clamp, as for any slider.
    shell.pane->setValue(600);
    QCOMPARE(shell.pane->value(), 500);
    shell.pane->setValue(0);
    QCOMPARE(shell.pane->value(), 10);
}

void tst_RibbonSliderPane::scrollButtons()
{
    SliderPaneShell shell;
    shell.pane->setRange(0, 100);
    shell.pane->setSingleStep(5);
    shell.pane->setValue(50);

    // Clicking the buttons steps the value by the single step: the leftmost
    // button decrements, the rightmost increments - the slots they are wired to
    // are protected, so the buttons are the only way in.
    const QList<QToolButton *> buttons =
        shell.pane->findChildren<QToolButton *>(QStringLiteral("qx_RibbonSliderPaneButton"));
    QCOMPARE(buttons.count(), 2);
    QToolButton *minus = buttons.at(0);
    QToolButton *plus = buttons.at(1);
    QVERIFY(minus->geometry().right() < plus->geometry().left());

    QTest::mouseClick(plus, Qt::LeftButton);
    QCOMPARE(shell.pane->value(), 55);
    QTest::mouseClick(minus, Qt::LeftButton);
    QCOMPARE(shell.pane->value(), 50);
}

void tst_RibbonSliderPane::scrollButtonsToggle()
{
    SliderPaneShell shell;

    // The default has both buttons up; turning the scroll buttons off keeps the
    // track alone - the buttons are still children, so the ask is visibility.
    QVERIFY(shell.pane->findChild<QToolButton *>(QStringLiteral("qx_RibbonSliderPaneButton")) != nullptr);
    shell.pane->setScrollButtons(false);
    const QList<QToolButton *> buttons =
        shell.pane->findChildren<QToolButton *>(QStringLiteral("qx_RibbonSliderPaneButton"));
    QCOMPARE(buttons.count(), 2);
    for (QToolButton *button : buttons) {
        QVERIFY(button->isHidden());
    }
}

void tst_RibbonSliderPane::sliderMovedForwarded()
{
    SliderPaneShell shell;
    shell.pane->setRange(0, 100);
    shell.pane->setValue(50);

    // A move with the handle down - what a drag is to the pane - is reported as
    // sliderMoved on the pane, not only on the slider inside.
    QSignalSpy moved(shell.pane, &RibbonSliderPane::sliderMoved);
    QSlider *slider = shell.pane->findChild<QSlider *>(QStringLiteral("qx_RibbonSliderPaneSlider"));
    QVERIFY(slider != nullptr);
    slider->setSliderDown(true);
    QTest::keyClick(slider, Qt::Key_Right);
    QVERIFY(moved.count() >= 1);
}

TEST_ADD(tst_RibbonSliderPane)

#include "tst_ribbonsliderpane.moc"
