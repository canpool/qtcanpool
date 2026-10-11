#include "tst_global.h"

#include "qxribbon/ribbonstyleoption.h"
#include "qxribbon/ribbonbar.h"

QX_RIBBON_USE_NAMESPACE

class tst_RibbonStyleOption : public QObject
{
    Q_OBJECT
private slots:
    void defaultRelations();
    void baseHeightScales();
    void explicitTitleTabSurvivesRecalc();
};

// The four ribbon-bar heights are derived from (titleBar, tabBar, groupOther)
// with fixed deltas, so the *differences* between them never depend on
// groupOther() and are safe to assert on any platform.
void tst_RibbonStyleOption::defaultRelations()
{
    RibbonStyleOption opt;
    const int title = opt.titleBarHeight();
    const int tab = opt.tabBarHeight();
    const int office3 = opt.ribbonBarHeight(RibbonBar::OfficeStyle);
    const int wps3 = opt.ribbonBarHeight(RibbonBar::WpsLiteStyle);
    const int office2 = opt.ribbonBarHeight(RibbonBar::OfficeStyleTwoRow);
    const int wps2 = opt.ribbonBarHeight(RibbonBar::WpsLiteStyleTwoRow);

    QCOMPARE(office3 - wps3, tab);
    QCOMPARE(office3 - office2, tab);
    QCOMPARE(office3 - wps2, tab * 2);
    QCOMPARE(wps3 - wps2, tab);
    QCOMPARE(office2 - wps2, tab);
    QVERIFY(title > 0);
    QVERIFY(tab > 0);
}

void tst_RibbonStyleOption::baseHeightScales()
{
    RibbonStyleOption opt;
    opt.setBaseHeight(40);
    QCOMPARE(opt.titleBarHeight(), static_cast<int>(40 * 1.65));
    QCOMPARE(opt.tabBarHeight(), static_cast<int>(40 * 1.5));

    // deltas must still hold after scaling
    const int tab = opt.tabBarHeight();
    const int office3 = opt.ribbonBarHeight(RibbonBar::OfficeStyle);
    const int wps3 = opt.ribbonBarHeight(RibbonBar::WpsLiteStyle);
    const int office2 = opt.ribbonBarHeight(RibbonBar::OfficeStyleTwoRow);
    const int wps2 = opt.ribbonBarHeight(RibbonBar::WpsLiteStyleTwoRow);
    QCOMPARE(office3 - wps3, tab);
    QCOMPARE(office3 - office2, tab);
    QCOMPARE(office3 - wps2, tab * 2);
}

void tst_RibbonStyleOption::explicitTitleTabSurvivesRecalc()
{
    RibbonStyleOption opt;
    opt.setTitleBarHeight(50);
    opt.setTabBarHeight(30);
    QCOMPARE(opt.titleBarHeight(), 50);
    QCOMPARE(opt.tabBarHeight(), 30);

    // recalc() must keep the explicit title/tab and only recompute the derived ribbon heights
    opt.recalc();
    QCOMPARE(opt.titleBarHeight(), 50);
    QCOMPARE(opt.tabBarHeight(), 30);

    const int tab = opt.tabBarHeight();
    const int office3 = opt.ribbonBarHeight(RibbonBar::OfficeStyle);
    const int wps3 = opt.ribbonBarHeight(RibbonBar::WpsLiteStyle);
    const int office2 = opt.ribbonBarHeight(RibbonBar::OfficeStyleTwoRow);
    const int wps2 = opt.ribbonBarHeight(RibbonBar::WpsLiteStyleTwoRow);
    QCOMPARE(office3 - wps3, tab);
    QCOMPARE(office3 - office2, tab);
    QCOMPARE(office3 - wps2, tab * 2);
}

TEST_ADD(tst_RibbonStyleOption)

#include "tst_ribbonstyleoption.moc"
