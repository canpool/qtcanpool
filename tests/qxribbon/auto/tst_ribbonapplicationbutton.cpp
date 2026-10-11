#include "tst_global.h"

#include "qxribbon/ribbonapplicationbutton.h"

#include <QIcon>
#include <QPixmap>

QX_RIBBON_USE_NAMESPACE

/*
 * The application button is the one control the ribbon itself owns rather than hosts: it
 * sits left of the tab bar, and every ribbon style paints its own version of it. What the
 * class contributes is the starting point all of those styles share, and it is all set in
 * the constructor - which is why the three constructors delegating to one another is the
 * thing worth testing. Nothing here depends on a style or a platform.
 */
class tst_RibbonApplicationButton : public QObject
{
    Q_OBJECT
private slots:
    void defaults();
    void textConstructorDelegatesAndKeepsTheText();
    void iconConstructorDelegatesAndKeepsBoth();
};

void tst_RibbonApplicationButton::defaults()
{
    RibbonApplicationButton button;

    // NoFocus: the button must never take the focus ring away from the page.
    QCOMPARE(button.focusPolicy(), Qt::NoFocus);
    // Flat: the frame belongs to the ribbon style, not to the platform style.
    QCOMPARE(button.isFlat(), true);
    // A floor for the narrowest style, which still has to draw something.
    QCOMPARE(button.minimumWidth(), 40);
    QVERIFY(button.text().isEmpty());
    QVERIFY(button.icon().isNull());
}

void tst_RibbonApplicationButton::textConstructorDelegatesAndKeepsTheText()
{
    RibbonApplicationButton button(QStringLiteral("File"));

    QCOMPARE(button.text(), QStringLiteral("File"));
    QVERIFY(button.icon().isNull());
    // Delegation: the same contract as the default constructor.
    QCOMPARE(button.focusPolicy(), Qt::NoFocus);
    QCOMPARE(button.isFlat(), true);
    QCOMPARE(button.minimumWidth(), 40);
}

void tst_RibbonApplicationButton::iconConstructorDelegatesAndKeepsBoth()
{
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::red);

    RibbonApplicationButton button(QIcon(pixmap), QStringLiteral("File"));

    QCOMPARE(button.text(), QStringLiteral("File"));
    QVERIFY(!button.icon().isNull());
    QCOMPARE(button.focusPolicy(), Qt::NoFocus);
    QCOMPARE(button.minimumWidth(), 40);
}

TEST_ADD(tst_RibbonApplicationButton)

#include "tst_ribbonapplicationbutton.moc"
