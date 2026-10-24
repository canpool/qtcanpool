/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentavatar.h"

#include <QAccessible>
#include <QtGui/QPainter>

QX_FLUENT_USE_NAMESPACE

namespace
{
/*! The darkest pixel in \a room: what the ink comes to where it is thickest.
 *  Comparing how many pixels differ from the page instead answers for the
 *  anti-aliasing rather than for the ink. */
int darkestPixel(const QImage &image, const QRect &room)
{
    int darkest = 255;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            darkest = qMin(darkest, image.pixelColor(x, y).lightness());
        }
    }
    return darkest;
}

/*! How many pixels of \a room are not the page - the face and what stands on
 *  it, however the anti-aliasing got there. */
int facePixels(const QImage &image, const QRect &room, const QColor &page)
{
    int count = 0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            if (image.pixelColor(x, y) != page) {
                ++count;
            }
        }
    }
    return count;
}
}   // namespace

class tst_FluentAvatar : public QObject
{
    Q_OBJECT
private slots:
    void theKeyboardLandsOnIt();
    void init();
    void cleanup();

    void theFaceComesInThreeSides();
    void aNameIsShortenedToTwoLetters();
    void theLettersStandOnTheFace();
    void aPictureStandsOverTheLetters();
    void theFaceIsRound();
    void aDisabledFaceIsFainter();
    void theAccessibleFaceIsAGraphic();
private:
    QWidget *m_page = nullptr;
};

void tst_FluentAvatar::init()
{
    qtcGiveFonts();
    m_page = new QWidget;
    m_page->resize(120, 120);
    m_page->show();
}

void tst_FluentAvatar::cleanup()
{
    delete m_page;
    m_page = nullptr;
}

/*! Three sides, and the avatar keeps the square of the one it is asked for. */
void tst_FluentAvatar::theFaceComesInThreeSides()
{
    FluentAvatar avatar(m_page);
    QCOMPARE(avatar.avatarSize(), FluentAvatar::Size::Medium);
    QCOMPARE(avatar.size(), QSize(40, 40));
    QCOMPARE(avatar.sizeHint(), QSize(40, 40));
    QCOMPARE(avatar.minimumSizeHint(), avatar.sizeHint());

    avatar.setAvatarSize(FluentAvatar::Size::Small);
    QCOMPARE(avatar.size(), QSize(24, 24));
    avatar.setAvatarSize(FluentAvatar::Size::Large);
    QCOMPARE(avatar.size(), QSize(56, 56));
}

/*! A name is shortened the way a face shortens one: the first letters of its
 *  first two words; one word keeps one letter rather than inventing a second. */
void tst_FluentAvatar::aNameIsShortenedToTwoLetters()
{
    FluentAvatar avatar(m_page);
    avatar.setName(QStringLiteral("Ada Lovelace"));
    QCOMPARE(avatar.initials(), QStringLiteral("AL"));

    avatar.setName(QStringLiteral("Grace Brewster Hopper"));
    QCOMPARE(avatar.initials(), QStringLiteral("GB"));

    avatar.setName(QStringLiteral("Plato"));
    QCOMPARE(avatar.initials(), QStringLiteral("P"));

    avatar.setName(QStringLiteral("  ada  lovelace  "));
    QCOMPARE(avatar.initials(), QStringLiteral("AL"));
}

/*! With no picture the letters are what stands on the face, and the face is a
 *  stand rather than the page. */
void tst_FluentAvatar::theLettersStandOnTheFace()
{
    FluentAvatar avatar(m_page);
    avatar.setName(QStringLiteral("Ada Lovelace"));
    avatar.show();

    const QColor page = avatar.palette().color(QPalette::Window);
    const QImage frame = avatar.grab().toImage();
    // The middle of the face: the stand under the letters, which is not the
    // page even where no letter reaches.
    QVERIFY(frame.pixelColor(frame.width() / 2, 4) != page);
    // And the letters themselves are darker than the stand they stand on.
    const QColor stand = frame.pixelColor(frame.width() / 2, 4);
    QVERIFY(facePixels(frame, QRect(8, 12, 24, 20), stand) > 0);
}

/*! A picture stands over the letters: with one, what the face shows is the
 *  picture, not the name. */
void tst_FluentAvatar::aPictureStandsOverTheLetters()
{
    FluentAvatar plain(m_page);
    plain.setName(QStringLiteral("Ada Lovelace"));
    plain.show();
    const QImage lettersFrame = plain.grab().toImage();

    FluentAvatar pictured(m_page);
    pictured.setName(QStringLiteral("Ada Lovelace"));
    QPixmap picture(8, 8);
    picture.fill(QColor(10, 200, 40));
    pictured.setPicture(picture);
    pictured.show();
    const QImage pictureFrame = pictured.grab().toImage();

    QCOMPARE(pictureFrame.pixelColor(pictureFrame.width() / 2, pictureFrame.height() / 2), QColor(10, 200, 40));
    QVERIFY(pictureFrame.pixelColor(pictureFrame.width() / 2, pictureFrame.height() / 2) !=
            lettersFrame.pixelColor(lettersFrame.width() / 2, lettersFrame.height() / 2));
}

/*! The face is round: the corners of the square stay the page's. */
void tst_FluentAvatar::theFaceIsRound()
{
    FluentAvatar avatar(m_page);
    avatar.setName(QStringLiteral("Ada"));
    avatar.show();

    const QColor page = avatar.palette().color(QPalette::Window);
    const QImage frame = avatar.grab().toImage();
    QCOMPARE(frame.pixelColor(0, 0), page);
    QCOMPARE(frame.pixelColor(frame.width() - 1, 0), page);
    QVERIFY(frame.pixelColor(frame.width() / 2, frame.height() / 2) != page);
}

/*! A face that cannot be used is drawn faded, the way everything else is. */
void tst_FluentAvatar::aDisabledFaceIsFainter()
{
    FluentAvatar enabled(m_page);
    enabled.setName(QStringLiteral("Ada Lovelace"));
    enabled.show();
    const QImage enabledFrame = enabled.grab().toImage();

    FluentAvatar disabled(m_page);
    disabled.setName(QStringLiteral("Ada Lovelace"));
    disabled.show();
    disabled.setEnabled(false);
    const QImage disabledFrame = disabled.grab().toImage();

    const QRect letters(8, 12, 24, 20);
    QVERIFY2(darkestPixel(disabledFrame, letters) > darkestPixel(enabledFrame, letters),
             qPrintable(QStringLiteral("enabled %1, disabled %2")
                            .arg(darkestPixel(enabledFrame, letters))
                            .arg(darkestPixel(disabledFrame, letters))));
}

/*! A face says who, and does nothing: it is a graphic, not a control. */
void tst_FluentAvatar::theAccessibleFaceIsAGraphic()
{
    FluentAvatar avatar(m_page);
    avatar.setName(QStringLiteral("Ada Lovelace"));

    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(&avatar);
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::Graphic);
    QCOMPARE(face->text(QAccessible::Name), QStringLiteral("Ada Lovelace"));
    QVERIFY(face->state().readOnly);
}

void tst_FluentAvatar::theKeyboardLandsOnIt()
{
    FluentAvatar widget(m_page);
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QApplication::processEvents();
    QWidget *focus = QApplication::focusWidget();
    QVERIFY(focus == &widget || widget.isAncestorOf(focus));
}

TEST_ADD(tst_FluentAvatar)

#include "tst_fluentavatar.moc"
