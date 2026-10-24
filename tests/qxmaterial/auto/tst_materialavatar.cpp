/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialavatar.h"

#include "qxcontrols/qxcontrol.h"
#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QSignalSpy>

QX_MATERIAL_USE_NAMESPACE

namespace
{
/*! A picture of one colour, so that where it lands is read as its colour. */
QPixmap plainPicture(const QColor &color, int side = 8)
{
    QPixmap pixmap(side, side);
    pixmap.fill(color);
    return pixmap;
}

QIcon plainIcon(const QColor &color)
{
    return QIcon(plainPicture(color));
}
}   // namespace

class tst_MaterialAvatar : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void threeFacesThreeSides();
    void aNameIsWrittenAsItsFirstLetter();
    void theLettersAreTheAccentsPair();
    void aPictureIsTheWholeFace();
    void anIconStandsInTheMiddle();
    void theShapeCutsTheEdge();
    void theToneChoosesTheStand();
    void whatAnAssistiveToolReads();
private:
    bool m_hadOverride = false;
};

void tst_MaterialAvatar::init()
{
    qtcGiveFonts();
    m_hadOverride = QxTheme::QxThemeManager::instance()->hasAccentOverride();
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor(0, 0, 255));
}

void tst_MaterialAvatar::cleanup()
{
    if (!m_hadOverride) {
        QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    }
}

/*! An avatar is square, and the three sizes are the three sides the family
 *  names - a face is the size it says it is. */
void tst_MaterialAvatar::threeFacesThreeSides()
{
    MaterialAvatar avatar;
    QCOMPARE(avatar.avatarSize(), MaterialAvatar::Size::Medium);
    QCOMPARE(avatar.sizeHint(), QSize(40, 40));
    QCOMPARE(avatar.width(), 40);
    QCOMPARE(avatar.height(), 40);

    int sizeChanges = 0;
    connect(&avatar, &MaterialAvatar::avatarSizeChanged, this, [&sizeChanges](MaterialAvatar::Size) {
        ++sizeChanges;
    });
    avatar.setAvatarSize(MaterialAvatar::Size::Small);
    QCOMPARE(sizeChanges, 1);
    QCOMPARE(avatar.sizeHint(), QSize(24, 24));
    QCOMPARE(avatar.width(), 24);
    QCOMPARE(avatar.height(), 24);

    avatar.setAvatarSize(MaterialAvatar::Size::Large);
    QCOMPARE(avatar.sizeHint(), QSize(56, 56));
    QCOMPARE(avatar.width(), 56);
    QCOMPARE(avatar.height(), 56);

    // Saying the same size twice is not a change.
    avatar.setAvatarSize(MaterialAvatar::Size::Large);
    QCOMPARE(sizeChanges, 2);
}

/*! The letter of a name is its first, in capitals; a name of nothing says
 *  nothing. */
void tst_MaterialAvatar::aNameIsWrittenAsItsFirstLetter()
{
    MaterialAvatar avatar;
    QCOMPARE(avatar.initials(), QString());

    avatar.setName(QStringLiteral("canpool"));
    QCOMPARE(avatar.initials(), QStringLiteral("C"));

    avatar.setName(QStringLiteral("  alice"));
    QCOMPARE(avatar.initials(), QStringLiteral("A"));

    avatar.setName(QStringLiteral("张三"));
    QCOMPARE(avatar.initials(), QStringLiteral("张"));

    avatar.setName(QString());
    QCOMPARE(avatar.initials(), QString());
}

/*! A name with no picture of its own is written in the accent pair: the
 *  letter is white on the primary, and the letter is what the face says. */
void tst_MaterialAvatar::theLettersAreTheAccentsPair()
{
    MaterialAvatar avatar;
    const QImage bare = avatar.grab().toImage();

    avatar.setName(QStringLiteral("Grace"));
    const QImage written = avatar.grab().toImage();
    QVERIFY(written != bare);

    // How a name is written is the letter, not the rest of it: two names
    // that start with the same letter are the same face.
    MaterialAvatar other;
    other.setName(QStringLiteral("gregory"));
    QCOMPARE(other.grab().toImage(), written);

    // The letter is white - the pair of the primary under it.
    bool white = false;
    for (int y = 0; y < written.height() && !white; ++y) {
        for (int x = 0; x < written.width(); ++x) {
            if (written.pixelColor(x, y) == QColor(Qt::white)) {
                white = true;
                break;
            }
        }
    }
    QVERIFY(white);
}

/*! A picture is the whole face: it is stretched over the stand, edges and
 *  all. */
void tst_MaterialAvatar::aPictureIsTheWholeFace()
{
    MaterialAvatar avatar;
    const QColor red(200, 30, 30);
    avatar.setPicture(plainPicture(red));

    const QImage image = avatar.grab().toImage();
    // The middle of the face is the picture, and so is a point a quarter in.
    QCOMPARE(image.pixelColor(image.width() / 2, image.height() / 2), red);
    QCOMPARE(image.pixelColor(image.width() / 4, image.height() / 2), red);
}

/*! An icon is the face's middle, not its edges: it keeps a margin of the
 *  stand around it that the picture does not. */
void tst_MaterialAvatar::anIconStandsInTheMiddle()
{
    MaterialAvatar avatar;
    const QColor green(20, 140, 70);
    avatar.setIcon(plainIcon(green));

    const QImage image = avatar.grab().toImage();
    QCOMPARE(image.pixelColor(image.width() / 2, image.height() / 2), green);
    // A quarter in along the middle line is still the stand - the accent.
    MaterialAvatar bare;
    QCOMPARE(image.pixelColor(image.width() / 4, image.height() / 2),
             bare.grab().toImage().pixelColor(image.width() / 4, image.height() / 2));
}

/*! The circle keeps no corner and the square keeps every one of them. */
void tst_MaterialAvatar::theShapeCutsTheEdge()
{
    const QColor stand(0, 0, 255);

    MaterialAvatar circle;
    const QImage round = circle.grab().toImage();
    QVERIFY(round.pixelColor(0, 0) != stand);

    MaterialAvatar square;
    square.setShape(MaterialAvatar::Shape::Square);
    const QImage cut = square.grab().toImage();
    QCOMPARE(cut.pixelColor(0, 0), stand);
    // The whole of a square's edge is the stand; a circle's is not.
    QVERIFY(round.pixelColor(0, 0) != cut.pixelColor(0, 0));

    MaterialAvatar rounded;
    rounded.setShape(MaterialAvatar::Shape::Rounded);
    const QImage softened = rounded.grab().toImage();
    QVERIFY(softened != round);
    QVERIFY(softened != cut);
}

/*! The stand is one of the family's two surfaces: the primary by default,
 *  the page's own variant when the quiet tone is asked for. */
void tst_MaterialAvatar::theToneChoosesTheStand()
{
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    MaterialAvatar primary;
    const QImage standing = primary.grab().toImage();
    QCOMPARE(standing.pixelColor(standing.width() / 4, standing.height() / 2), accent);

    MaterialAvatar neutral;
    neutral.setTone(MaterialAvatar::Tone::Neutral);
    const QImage quiet = neutral.grab().toImage();
    QVERIFY(quiet.pixelColor(quiet.width() / 4, quiet.height() / 2) != accent);

    // Two surfaces, two faces: the quiet one is not the primary one.
    QVERIFY(quiet != standing);
}

/*! A screen reader is told who the face is - the name, not its letter - and
 *  that there is nothing to press. */
void tst_MaterialAvatar::whatAnAssistiveToolReads()
{
    MaterialAvatar avatar;
    avatar.setName(QStringLiteral("Grace Hopper"));

    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&avatar);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Graphic);
    QCOMPARE(iface->text(QAccessible::Name), QStringLiteral("Grace Hopper"));
    QVERIFY(iface->state().readOnly);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_MaterialAvatar)

#include "tst_materialavatar.moc"
