/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialskeleton.h"

#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QImage>

QX_MATERIAL_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
/*! How much ink the skeleton put on the page: it is a tint of the page's own
 *  colour, so what is measured is how much darker it made where it stands. */
int ink(QWidget *widget)
{
    const QImage image = widget->grab().toImage().convertToFormat(QImage::Format_ARGB32);
    const int pageGray = qGray(widget->palette().color(QPalette::Window).rgb());
    int total = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            total += qAbs(pageGray - qGray(image.pixel(x, y)));
        }
    }
    return total;
}
}   // namespace

class tst_MaterialSkeleton : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void itIsTheSizeOfWhatItStandsInFor();
    void aDiscIsNotALine();
    void itIsNotSomethingTheUserTakesHoldOf();
    void itBreathes();
    void itHoldsItsBreathWhenLessMotionWasAskedFor();
    void whatAnAssistiveToolReads();
private:
    bool m_wasReduced = false;
    bool m_wasActive = false;
};

void tst_MaterialSkeleton::init()
{
    m_wasReduced = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(false);
    m_wasActive = QAccessible::isActive();
}

void tst_MaterialSkeleton::cleanup()
{
    QxTheme::setMotionReduced(m_wasReduced);
    QAccessible::setActive(m_wasActive);
}

/*! A skeleton is the shape of what is coming, so its shape is the shape of
 *  that: a line where a line will be, a disc where a face will be. */
void tst_MaterialSkeleton::itIsTheSizeOfWhatItStandsInFor()
{
    MaterialSkeleton line;
    QCOMPARE(line.sizeHint(), QSize(120, 16));

    MaterialSkeleton face(MaterialSkeleton::Shape::Circle);
    QCOMPARE(face.sizeHint(), QSize(40, 40));

    line.setShape(MaterialSkeleton::Shape::Circle);
    QCOMPARE(line.sizeHint(), QSize(40, 40));
    QVERIFY(line.shape() == MaterialSkeleton::Shape::Circle);
}

/*! A disc and a line of the same size do not put the same ink on the page: a
 *  disc is round, so its corners stay the page's. */
void tst_MaterialSkeleton::aDiscIsNotALine()
{
    MaterialSkeleton line(MaterialSkeleton::Shape::Rectangle);
    line.resize(40, 40);
    MaterialSkeleton disc(MaterialSkeleton::Shape::Circle);
    disc.resize(40, 40);

    QVERIFY(ink(&disc) < ink(&line));
}

/*! What is not there yet takes neither the pointer nor a stop on the way
 *  through the keyboard. */
void tst_MaterialSkeleton::itIsNotSomethingTheUserTakesHoldOf()
{
    MaterialSkeleton skeleton;
    QCOMPARE(skeleton.focusPolicy(), Qt::NoFocus);
    QVERIFY(skeleton.testAttribute(Qt::WA_TransparentForMouseEvents));
}

/*! It breathes, because a shape that sits perfectly still is a shape the user
 *  has started to read. */
void tst_MaterialSkeleton::itBreathes()
{
    MaterialSkeleton skeleton;
    skeleton.resize(skeleton.sizeHint());

    const int first = ink(&skeleton);
    QTest::qWait(400);
    const int second = ink(&skeleton);
    QTest::qWait(400);
    const int third = ink(&skeleton);
    QVERIFY(second != first || third != second);
}

/*! Breathing is the kind of movement the reduce-motion switch is about, so it
 *  stops - and what is left is still a shape, which is all a skeleton is for. */
void tst_MaterialSkeleton::itHoldsItsBreathWhenLessMotionWasAskedFor()
{
    QxTheme::setMotionReduced(true);
    MaterialSkeleton skeleton;
    skeleton.resize(skeleton.sizeHint());

    const int first = ink(&skeleton);
    QVERIFY(first > 0);
    QTest::qWait(400);
    QCOMPARE(ink(&skeleton), first);
}

/*! It says nothing of its own: what is loading is the application's to name. */
void tst_MaterialSkeleton::whatAnAssistiveToolReads()
{
    MaterialSkeleton skeleton;
    skeleton.setAccessibleName(QStringLiteral("Loading messages"));

    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&skeleton);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::StaticText);
    QCOMPARE(iface->text(QAccessible::Name), QStringLiteral("Loading messages"));
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_MaterialSkeleton)

#include "tst_materialskeleton.moc"
