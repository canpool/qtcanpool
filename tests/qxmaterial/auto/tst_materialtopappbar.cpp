/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialtopappbar.h"

#include <QAccessible>
#include <QAction>
#include <QImage>
#include <QPointer>
#include <QWidget>

QX_MATERIAL_USE_NAMESPACE

namespace
{
using QxMaterial::MaterialTopAppBar;

// An icon is a target of forty-eight keeping four from the edge, so its middle
// is twenty-eight in - and the trailing ones are placed from the right edge
// inwards, which puts the last of them twenty-eight in from that edge.
constexpr int kIconMiddle = 28;

QPoint leadingPoint()
{
    return QPoint(kIconMiddle, kIconMiddle);
}

QPoint trailingPoint(int width, int fromLast)
{
    return QPoint(width - kIconMiddle - fromLast * 48, kIconMiddle);
}

/*! What a press on an icon does: the bar's only answer is which one it landed
 *  on, so the count and the last one are what there is to read. */
struct Triggers {
    int count = 0;
    QAction *last = Q_NULLPTR;

    void listen(QAction *action)
    {
        QObject::connect(action, &QAction::triggered, action, [this, action]() {
            ++count;
            last = action;
        });
    }
};

/*! Where the bar would draw a mark is protected, and the defect this stands for
 *  was a mark rather than a pixel: asking the bar where it would draw is the
 *  question a picture would only be a second answer to. */
class ProbeBar : public MaterialTopAppBar
{
public:
    using MaterialTopAppBar::chromeRect;
};
}   // namespace

class tst_MaterialTopAppBar : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void theBarHoldsWhatItIsGiven();
    void theShapeDecidesHowMuchRoomThereIs();
    void aPressRunsWhatItLandedOn();
    void anIconNobodyCanUseDoesNothing();
    void aPressThatLandsOnNoIconIsNothing();
    void whatIsAtAPoint();
    void beingScrolledUnderLeavesThePage();
    void theKeyboardWalksTheIcons();
    void aBarNobodyHasBeenOnMarksNothing();
    void whatAnAssistiveToolReads();
};

void tst_MaterialTopAppBar::cleanup()
{
    QAccessible::setActive(false);
}

/*! The three places in a bar: the name of the screen, the way out, and the
 *  things that can be done to the screen as a whole. */
void tst_MaterialTopAppBar::theBarHoldsWhatItIsGiven()
{
    std::unique_ptr<MaterialTopAppBar> bar(new MaterialTopAppBar);
    QVERIFY(!bar->leadingAction());
    QVERIFY(bar->trailingActions().isEmpty());

    QAction *nav = bar->setLeadingAction(QStringLiteral("Menu"));
    QAction *search = bar->addTrailingAction(QStringLiteral("Search"));
    QAction *more = bar->addTrailingAction(QStringLiteral("More"));

    QCOMPARE(bar->leadingAction(), nav);
    QCOMPARE(bar->trailingActions(), QList<QAction *>({search, more}));

    // There is one leading place: a second one takes the first one's place
    // rather than standing beside it. Taking the place means the action that
    // stood there is destroyed - which is what makes a pointer the bar handed
    // out good only until the place is filled again - so what is watched here
    // is that the old one is gone, not that the two addresses differ. Addresses
    // are not the bar's to promise: a freed block is routinely handed straight
    // back for the next allocation, and two builds of this file on two systems
    // disagree about it.
    QPointer<QAction> displaced = nav;
    QAction *back = bar->setLeadingAction(QStringLiteral("Back"));
    QCOMPARE(bar->leadingAction(), back);
    QVERIFY(displaced.isNull());

    QPointer<QAction> dropped = search;
    bar->removeTrailingAction(search);
    QCOMPARE(bar->trailingActions(), QList<QAction *>({more}));
    QVERIFY(dropped.isNull());
    bar->clearTrailingActions();
    QVERIFY(bar->trailingActions().isEmpty());
    bar->clearLeadingAction();
    QVERIFY(!bar->leadingAction());

    bar->setTitle(QStringLiteral("Inbox"));
    QCOMPARE(bar->title(), QStringLiteral("Inbox"));
}

/*! The four shapes are told apart by one thing only, which is how much room
 *  the name of the screen gets. */
void tst_MaterialTopAppBar::theShapeDecidesHowMuchRoomThereIs()
{
    std::unique_ptr<MaterialTopAppBar> bar(new MaterialTopAppBar);
    QCOMPARE(MaterialTopAppBar::heightFor(MaterialTopAppBar::Variant::Small), 56);
    QCOMPARE(MaterialTopAppBar::heightFor(MaterialTopAppBar::Variant::CenterAligned), 56);
    QCOMPARE(MaterialTopAppBar::heightFor(MaterialTopAppBar::Variant::Medium), 112);
    QCOMPARE(MaterialTopAppBar::heightFor(MaterialTopAppBar::Variant::Large), 152);

    QCOMPARE(bar->sizeHint().height(), 56);
    bar->setVariant(MaterialTopAppBar::Variant::Large);
    QCOMPARE(bar->variant(), MaterialTopAppBar::Variant::Large);
    QCOMPARE(bar->sizeHint().height(), 152);
    QCOMPARE(bar->minimumSizeHint().height(), 152);
}

void tst_MaterialTopAppBar::aPressRunsWhatItLandedOn()
{
    std::unique_ptr<MaterialTopAppBar> bar(new MaterialTopAppBar);
    Triggers asked;
    asked.listen(bar->setLeadingAction(QStringLiteral("Menu")));
    QAction *search = bar->addTrailingAction(QStringLiteral("Search"));
    QAction *more = bar->addTrailingAction(QStringLiteral("More"));
    asked.listen(search);
    asked.listen(more);
    bar->resize(400, 56);

    QTest::mouseClick(bar.get(), Qt::LeftButton, Qt::NoModifier, leadingPoint());
    QCOMPARE(asked.count, 1);
    QCOMPARE(asked.last, bar->leadingAction());

    // The trailing ones are placed from the right edge inwards, so the one
    // added last is the one nearest that edge.
    QTest::mouseClick(bar.get(), Qt::LeftButton, Qt::NoModifier, trailingPoint(400, 0));
    QCOMPARE(asked.count, 2);
    QCOMPARE(asked.last, more);

    QTest::mouseClick(bar.get(), Qt::LeftButton, Qt::NoModifier, trailingPoint(400, 1));
    QCOMPARE(asked.count, 3);
    QCOMPARE(asked.last, search);
}

void tst_MaterialTopAppBar::anIconNobodyCanUseDoesNothing()
{
    std::unique_ptr<MaterialTopAppBar> bar(new MaterialTopAppBar);
    Triggers asked;
    QAction *nav = bar->setLeadingAction(QStringLiteral("Menu"));
    nav->setEnabled(false);
    asked.listen(nav);
    bar->resize(400, 56);

    QTest::mouseClick(bar.get(), Qt::LeftButton, Qt::NoModifier, leadingPoint());
    QCOMPARE(asked.count, 0);
}

/*! A bar is not one icon: the name of the screen is not somewhere a press
 *  lands. */
void tst_MaterialTopAppBar::aPressThatLandsOnNoIconIsNothing()
{
    std::unique_ptr<MaterialTopAppBar> bar(new MaterialTopAppBar);
    Triggers asked;
    asked.listen(bar->setLeadingAction(QStringLiteral("Menu")));
    bar->setTitle(QStringLiteral("Inbox"));
    bar->resize(400, 56);

    QTest::mouseClick(bar.get(), Qt::LeftButton, Qt::NoModifier, QPoint(200, kIconMiddle));
    QCOMPARE(asked.count, 0);
}

/*! A bar used as a window's caption is asked one question by its wrapper:
 *  whether a point belongs to an icon or to the caption. The leading icon and
 *  the trailing ones answer alike, because the question is not which row a
 *  point is in - and the answer follows the room the bar has, because that is
 *  what the icons are placed by. */
void tst_MaterialTopAppBar::whatIsAtAPoint()
{
    std::unique_ptr<MaterialTopAppBar> bar(new MaterialTopAppBar);
    QVERIFY(!bar->actionAt(leadingPoint()));

    QAction *nav = bar->setLeadingAction(QStringLiteral("Menu"));
    QAction *more = bar->addTrailingAction(QStringLiteral("More"));
    bar->resize(400, 56);

    QCOMPARE(bar->actionAt(leadingPoint()), nav);
    QCOMPARE(bar->actionAt(trailingPoint(400, 0)), more);
    // The name of the screen is the bar itself, and so is the room beside it.
    QVERIFY(!bar->actionAt(QPoint(200, kIconMiddle)));
    QVERIFY(!bar->actionAt(QPoint(200, 52)));

    // A wider bar moves its trailing icons with its edge, so a point that was
    // on one is on the bar now and a point that was on the bar is on one.
    bar->resize(600, 56);
    QVERIFY(!bar->actionAt(trailingPoint(400, 0)));
    QCOMPARE(bar->actionAt(trailingPoint(600, 0)), more);
}

/*! What it means that content has moved under the bar: it stops being the
 *  page and becomes the surface one step away. That change is the whole of
 *  what Material calls on-scroll elevation. */
void tst_MaterialTopAppBar::beingScrolledUnderLeavesThePage()
{
    std::unique_ptr<MaterialTopAppBar> bar(new MaterialTopAppBar);
    bar->resize(400, 56);
    QVERIFY(!bar->isScrolled());

    // A corner well clear of the icons and of the name, so what is read there
    // is the surface and nothing drawn on top of it.
    const QColor before = bar->grab().toImage().convertToFormat(QImage::Format_ARGB32).pixelColor(2, 52);
    bar->setScrolled(true);
    QVERIFY(bar->isScrolled());
    const QColor after = bar->grab().toImage().convertToFormat(QImage::Format_ARGB32).pixelColor(2, 52);

    QVERIFY(after != before);
    QCOMPARE(after, bar->palette().color(QPalette::AlternateBase));
    QCOMPARE(before, bar->palette().color(QPalette::Window));
}

/*! A row of icons is one stop on the way through the keyboard, not one per
 *  icon: the arrows move which one is on and Enter runs it. */
void tst_MaterialTopAppBar::theKeyboardWalksTheIcons()
{
    std::unique_ptr<MaterialTopAppBar> bar(new MaterialTopAppBar);
    Triggers asked;
    QAction *nav = bar->setLeadingAction(QStringLiteral("Menu"));
    QAction *search = bar->addTrailingAction(QStringLiteral("Search"));
    asked.listen(nav);
    asked.listen(search);
    bar->resize(400, 56);

    QTest::keyClick(bar.get(), Qt::Key_Enter);
    QCOMPARE(asked.count, 1);
    QCOMPARE(asked.last, nav);

    QTest::keyClick(bar.get(), Qt::Key_Right);
    QTest::keyClick(bar.get(), Qt::Key_Space);
    QCOMPARE(asked.count, 2);
    QCOMPARE(asked.last, search);

    // The ends are walls.
    QTest::keyClick(bar.get(), Qt::Key_Right);
    QTest::keyClick(bar.get(), Qt::Key_Enter);
    QCOMPARE(asked.count, 3);
    QCOMPARE(asked.last, search);

    QTest::keyClick(bar.get(), Qt::Key_Left);
    QTest::keyClick(bar.get(), Qt::Key_Enter);
    QCOMPARE(asked.count, 4);
    QCOMPARE(asked.last, nav);
}

/*! A bar nobody has pointed at and nobody has walked to has no mark on it. The
 *  first trailing action used to wear the ring for good: adding it put the
 *  keyboard's place on the first icon before the keyboard had been anywhere,
 *  and the mark that says "the keyboard is here" said it about a bar that did
 *  not have the keyboard at all.
 *
 *  Only the empty answer is asserted. The other half - that a bar which does
 *  hold the keyboard marks where the keyboard is - needs a widget that has
 *  focus, and the platform the tests run on in CI has none to give; what proves
 *  this one is the fallback run, where putting the place back on the first icon
 *  makes it fail. */
void tst_MaterialTopAppBar::aBarNobodyHasBeenOnMarksNothing()
{
    ProbeBar bar;
    bar.resize(400, 56);
    bar.addTrailingAction(QStringLiteral("Search"));
    QVERIFY2(bar.chromeRect().isEmpty(), "a mark where nobody has been");
}

void tst_MaterialTopAppBar::whatAnAssistiveToolReads()
{
    std::unique_ptr<MaterialTopAppBar> bar(new MaterialTopAppBar);
    bar->setLeadingAction(QStringLiteral("Menu"));
    QAccessible::setActive(true);
    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(bar.get());
    QVERIFY(face);
    // A row of things that can be done to the screen, which is what a toolbar
    // is to an assistive tool.
    QCOMPARE(face->role(), QAccessible::ToolBar);
}

TEST_ADD(tst_MaterialTopAppBar)

#include "tst_materialtopappbar.moc"
