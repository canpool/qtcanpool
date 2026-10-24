/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialspeeddial.h"
#include "qxmaterial/materialtokens.h"

#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QtGui/QPainter>
#include <QAccessible>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The dial's own measures, read the way a reader reads them.
constexpr int kButtonSize = 56;
constexpr int kActionSize = 40;
constexpr int kStep = 48;

/*! How long the family says a movement takes, read from its own tokens rather
 *  than spelled here. */
int travelTime()
{
    return MaterialMotion::forDuration(MaterialMotion::Duration::Medium).duration;
}

/*! Ask for less motion for as long as this object lives, and give the switch
 *  back the way it was: a test must not leave the process quieter than it found
 *  it. The manager comes first - it installs the theme's own motion when it
 *  starts, which would otherwise land on top of the switch. */
class QuietMotion
{
public:
    QuietMotion()
    {
        QxTheme::QxThemeManager::instance();
        m_was = QxTheme::isMotionReduced();
        QxTheme::setMotionReduced(true);
    }
    ~QuietMotion()
    {
        QxTheme::setMotionReduced(m_was);
    }
private:
    bool m_was = false;
};

/*! How many pixels of \a room are the accent itself, allowance made for the one
 *  or two of anti-aliasing either side of it. */
int accentPixels(const QImage &image, const QRect &room)
{
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    int count = 0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            const QColor pixel = image.pixelColor(x, y);
            if (qAbs(pixel.red() - accent.red()) <= 8 && qAbs(pixel.green() - accent.green()) <= 8 &&
                qAbs(pixel.blue() - accent.blue()) <= 8) {
                ++count;
            }
        }
    }
    return count;
}

/*! How many pixels of \a room are darker than the page: what a drawn name takes
 *  is not the question - that it is drawn at all is. */
int inkedPixels(const QImage &image, const QRect &room, const QColor &page)
{
    int count = 0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            if (image.pixelColor(x, y).lightness() < page.lightness() - 2) {
                ++count;
            }
        }
    }
    return count;
}

/*! Whether \a pixel is \a expected, allowing for the anti-aliasing of whatever
 *  was drawn over it. */
bool alike(const QColor &pixel, const QColor &expected)
{
    return qAbs(pixel.red() - expected.red()) <= 3 && qAbs(pixel.green() - expected.green()) <= 3 &&
           qAbs(pixel.blue() - expected.blue()) <= 3;
}

/*! How many of \a host's own children are on show: the dial is one of them, and
 *  the actions are the rest - but only while the dial is open. */
int showing(QWidget *host)
{
    int count = 0;
    const QList<QWidget *> children = host->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
    for (const QWidget *child : children) {
        if (child->isVisible()) {
            ++count;
        }
    }
    return count;
}
}   // namespace

class tst_MaterialSpeedDial : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void theDialIsTheButtonAndNothingElse();
    void theActionsComeOutAboveTheButton();
    void theDirectionSaysWhichWayTheyComeOut();
    void theActionsTravelOut();
    void anActionTakenPutsTheDialAway();
    void theButtonIsWhatOpensAndCloses();
    void theKeyboardOpensItAndEscapePutsItAway();
    void everyActionKeepsItsNameOnShow();
    void aDialNobodyCanUseOpensNothing();
    void theButtonAndItsActionsAnswerForThemselves();
};

namespace
{
QWidget *makePage()
{
    auto *page = new QWidget;
    page->resize(460, 320);
    page->show();
    QTest::qWaitForWindowExposed(page);
    return page;
}

/*! A dial in the lower right of the page (or the upper right, when what comes
 *  out of it goes down), with three actions whose names want room of different
 *  lengths - so a test can tell "the circles line up" from "the boxes do". */
MaterialSpeedDial *makeDial(QWidget *page, MaterialSpeedDial::Direction direction = MaterialSpeedDial::Direction::Up)
{
    auto *dial = new MaterialSpeedDial(page);
    dial->setText(QStringLiteral("Create"));
    dial->addAction(QStringLiteral("Folder"));
    dial->addAction(QStringLiteral("Upload a file"));
    dial->addAction(QStringLiteral("Note"));
    dial->setDirection(direction);
    dial->move(380, direction == MaterialSpeedDial::Direction::Up ? 260 : 20);
    dial->show();
    QTest::qWaitForWindowExposed(dial);
    return dial;
}

/*! The button itself: the one child of the dial that stands in its square. */
QWidget *buttonOf(MaterialSpeedDial *dial)
{
    return dial->childAt(QPoint(kButtonSize / 2, kButtonSize / 2));
}

/*! The action standing where action \a index says. The actions are the host's
 *  children - they fly outside the dial's own square - so a test reaches them
 *  the way a reader does: through the host. */
QWidget *actionOf(QWidget *page, MaterialSpeedDial *dial, int index)
{
    const QRect room = dial->actionRect(index);
    if (room.isEmpty()) {
        return Q_NULLPTR;
    }
    return page->childAt(dial->mapTo(page, room.center()));
}
}   // namespace

void tst_MaterialSpeedDial::initTestCase()
{
    // Nothing here reads a glyph, but the names are measured, and a platform
    // with no font at all is not the one the rest of the suite runs on.
    qtcGiveFonts();
    QxTheme::QxThemeManager::instance();
    QxTheme::setMotionReduced(false);
}

/*! The dial is the button's own square. What comes out of it stands outside that
 *  square, in the host, and is not there until it is asked for. */
void tst_MaterialSpeedDial::theDialIsTheButtonAndNothingElse()
{
    QWidget *page = makePage();
    MaterialSpeedDial *dial = makeDial(page);

    QCOMPARE(dial->isOpen(), false);
    QCOMPARE(dial->actionCount(), 3);
    QCOMPARE(dial->actionName(0), QStringLiteral("Folder"));
    QCOMPARE(dial->actionName(3), QString());
    QCOMPARE(dial->sizeHint(), QSize(kButtonSize, kButtonSize));
    QCOMPARE(dial->minimumSizeHint(), dial->sizeHint());
    QCOMPARE(dial->size(), QSize(kButtonSize, kButtonSize));

    // Closed, every action rests on the button, and none of them is on show: the
    // only thing the page has is the dial.
    QCOMPARE(dial->actionRect(0).size(), QSize(kActionSize, kActionSize));
    QCOMPARE(dial->actionRect(1), dial->actionRect(0));
    QCOMPARE(dial->actionRect(-1), QRect());
    QCOMPARE(dial->actionRect(3), QRect());
    QCOMPARE(showing(page), 1);
}

/*! Opened, the actions stand above the button in a column, their circles lined
 *  up with the button's right edge - the names then reach as far left as they
 *  need to, which is why the boxes are not all the same width. */
void tst_MaterialSpeedDial::theActionsComeOutAboveTheButton()
{
    QuietMotion quiet;
    QWidget *page = makePage();
    MaterialSpeedDial *dial = makeDial(page);
    dial->open();
    QCOMPARE(dial->isOpen(), true);
    QCOMPARE(showing(page), 1 + dial->actionCount());

    for (int i = 0; i < dial->actionCount(); ++i) {
        const QRect room = dial->actionRect(i);
        QCOMPARE(room.height(), kActionSize);
        QCOMPARE(room.right(), kButtonSize - 1);
        QCOMPARE(room.top(), -kActionSize - 8 - i * kStep);
        QVERIFY(room.width() > kActionSize);
    }
    // The longest name makes the widest box.
    QVERIFY(dial->actionRect(1).width() > dial->actionRect(2).width());

    // What is out stands in the host, in the place the dial says it does.
    const QRect room = dial->actionRect(0);
    const QWidget *action = actionOf(page, dial, 0);
    QVERIFY(action);
    QCOMPARE(action->geometry(), QRect(dial->mapTo(page, room.topLeft()), room.size()));
}

/*! Which way they come out is the caller's to say. */
void tst_MaterialSpeedDial::theDirectionSaysWhichWayTheyComeOut()
{
    QuietMotion quiet;
    QWidget *page = makePage();
    MaterialSpeedDial *dial = makeDial(page, MaterialSpeedDial::Direction::Down);
    dial->open();

    QCOMPARE(dial->direction(), MaterialSpeedDial::Direction::Down);
    QCOMPARE(dial->actionRect(0).top(), kButtonSize + 8);
    QCOMPARE(dial->actionRect(1).top(), kButtonSize + 8 + kStep);
    QCOMPARE(dial->actionRect(0).right(), kButtonSize - 1);

    // A spy cannot read a scoped enum back without the type being registered
    // with the meta-object system under the name the signal is declared with,
    // so the signal is listened to directly instead: what a reader of this
    // class gets is the direction itself, not a QVariant of it.
    QList<MaterialSpeedDial::Direction> said;
    QObject::connect(dial, &MaterialSpeedDial::directionChanged, dial, [&said](MaterialSpeedDial::Direction direction) {
        said.append(direction);
    });
    dial->setDirection(MaterialSpeedDial::Direction::Up);
    QCOMPARE(said.count(), 1);
    QCOMPARE(said.at(0), MaterialSpeedDial::Direction::Up);
    QCOMPARE(dial->actionRect(0).top(), -kActionSize - 8);
    // Saying the same thing twice is not a change.
    dial->setDirection(MaterialSpeedDial::Direction::Up);
    QCOMPARE(said.count(), 1);
}

/*! They come out over the family's movement, and are simply out when the
 *  application asked for less motion. */
void tst_MaterialSpeedDial::theActionsTravelOut()
{
    QWidget *page = makePage();
    MaterialSpeedDial *dial = makeDial(page);
    const int resting = dial->actionRect(0).top();
    const int out = -kActionSize - 8;
    QCOMPARE(resting, kButtonSize - kActionSize);

    dial->open();
    // A third of the way through, the action is between the button and its place
    // and not yet there. (The first frame has not ticked, so asking immediately
    // would be asking before the movement started.)
    QTest::qWait(travelTime() / 3);
    const int travelling = dial->actionRect(0).top();
    QVERIFY2(travelling < resting && travelling > out,
             qPrintable(QStringLiteral("%1 is not between %2 and %3").arg(travelling).arg(resting).arg(out)));

    QTest::qWait(travelTime() + 100);
    QCOMPARE(dial->actionRect(0).top(), out);

    // Put away, they travel back and are gone.
    dial->close();
    QTest::qWait(travelTime() + 100);
    QCOMPARE(dial->actionRect(0).top(), resting);
    QCOMPARE(showing(page), 1);
}

/*! An action taken is an answer: the dial puts itself away, and says which
 *  action it was. */
void tst_MaterialSpeedDial::anActionTakenPutsTheDialAway()
{
    QuietMotion quiet;
    QWidget *page = makePage();
    MaterialSpeedDial *dial = makeDial(page);
    dial->open();

    QSignalSpy taken(dial, &MaterialSpeedDial::actionTriggered);
    QSignalSpy opened(dial, &MaterialSpeedDial::openChanged);
    QTest::mouseClick(actionOf(page, dial, 1), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(taken.count(), 1);
    QCOMPARE(taken.at(0).at(0).toInt(), 1);
    QCOMPARE(opened.count(), 1);
    QCOMPARE(opened.at(0).at(0).toBool(), false);
    QCOMPARE(dial->isOpen(), false);
    QCOMPARE(showing(page), 1);
}

/*! The button is what opens it, and what closes it again. */
void tst_MaterialSpeedDial::theButtonIsWhatOpensAndCloses()
{
    QuietMotion quiet;
    QWidget *page = makePage();
    MaterialSpeedDial *dial = makeDial(page);

    QTest::mouseClick(buttonOf(dial), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(dial->isOpen(), true);
    QTest::mouseClick(buttonOf(dial), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(dial->isOpen(), false);
}

/*! The keyboard opens it, and Escape puts it away - the same Escape that puts
 *  away everything else that came out of a button. */
void tst_MaterialSpeedDial::theKeyboardOpensItAndEscapePutsItAway()
{
    QuietMotion quiet;
    QWidget *page = makePage();
    MaterialSpeedDial *dial = makeDial(page);
    QWidget *button = buttonOf(dial);
    button->setFocus();
    QVERIFY(button->hasFocus());

    QTest::keyClick(button, Qt::Key_Space);
    QCOMPARE(dial->isOpen(), true);
    QTest::keyClick(button, Qt::Key_Escape);
    QCOMPARE(dial->isOpen(), false);

    QTest::keyClick(button, Qt::Key_Return);
    QCOMPARE(dial->isOpen(), true);
}

/*! Every action keeps its name on show: the way to find out what a round button
 *  does is not to point at it. */
void tst_MaterialSpeedDial::everyActionKeepsItsNameOnShow()
{
    QuietMotion quiet;
    QWidget *page = makePage();
    MaterialSpeedDial *dial = makeDial(page);
    dial->open();

    const QImage frame = page->grab().toImage();
    const QColor page_ = page->palette().color(QPalette::Window);
    const QRect room = dial->actionRect(1);
    const QRect onPage(dial->mapTo(page, room.topLeft()), room.size());

    // The name is drawn in the part of the box the circle is not in.
    const QRect names(onPage.left(), onPage.top(), onPage.width() - kActionSize - 12, onPage.height());
    QVERIFY2(inkedPixels(frame, names, page_) > 0, qPrintable(QString::number(inkedPixels(frame, names, page_))));

    // The circle is the page's own colour, so that the accent still says which
    // circle is the one the screen is for.
    const QPoint inside(onPage.right() - kActionSize / 2 - 6, onPage.center().y());
    QVERIFY2(alike(frame.pixelColor(inside), page_), qPrintable(frame.pixelColor(inside).name()));

    // And the button is the accent.
    const QRect button(dial->mapTo(page, QPoint(0, 0)), dial->size());
    const QPoint inButton = button.center() + QPoint(-16, 16);
    QVERIFY2(alike(frame.pixelColor(inButton), QxTheme::QxThemeManager::instance()->accentColor()),
             qPrintable(frame.pixelColor(inButton).name()));
}

/*! A dial nobody can use opens nothing, and is drawn with less ink. */
void tst_MaterialSpeedDial::aDialNobodyCanUseOpensNothing()
{
    QuietMotion quiet;
    QWidget *page = makePage();
    MaterialSpeedDial *dial = makeDial(page);
    const QRect button(dial->mapTo(page, QPoint(0, 0)), dial->size());
    const int lit = accentPixels(page->grab().toImage(), button);
    QVERIFY(lit > 0);

    dial->setEnabled(false);
    QTest::mouseClick(buttonOf(dial), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(dial->isOpen(), false);
    QCOMPARE(accentPixels(page->grab().toImage(), button), 0);
}

/*! To an assistive tool the button is a button and each action is one, and the
 *  names are the ones the application gave. */
void tst_MaterialSpeedDial::theButtonAndItsActionsAnswerForThemselves()
{
    QuietMotion quiet;
    QWidget *page = makePage();
    MaterialSpeedDial *dial = makeDial(page);
    dial->open();

    QAccessibleInterface *button = QAccessible::queryAccessibleInterface(buttonOf(dial));
    QVERIFY(button);
    QCOMPARE(button->role(), QAccessible::Button);
    QCOMPARE(button->text(QAccessible::Name), QStringLiteral("Create"));

    QAccessibleInterface *action = QAccessible::queryAccessibleInterface(actionOf(page, dial, 1));
    QVERIFY(action);
    QCOMPARE(action->role(), QAccessible::Button);
    QCOMPARE(action->text(QAccessible::Name), QStringLiteral("Upload a file"));
}

TEST_ADD(tst_MaterialSpeedDial)

#include "tst_materialspeeddial.moc"
