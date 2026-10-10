#include "tst_global.h"

#include "qxapp/qxappshell.h"
#include "qxapp/qxtoast.h"
#include "qxapp/qxtoastmanager.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPointer>
#include <QStyle>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QX_APP_USE_NAMESPACE

/* ------------------------------------------------------------------ toast -- */

/*!
 * The toast on its own: its state, the level mapping and the size it asks for.
 * The widget is never shown here - a QWidget answers all of these questions
 * without a window - which keeps the whole case clear of the platform.
 */
class tst_QxToast : public QObject
{
    Q_OBJECT
private slots:
    void defaults();
    void textAndLevel();
    void accentFollowsTheLevel();
    void sizeDependsOnTheText();
    void timeoutAndFade();
    void dismissIsIdempotent();
    void iconAndMessageAreSeparate();
};

void tst_QxToast::defaults()
{
    QWidget host;
    QxToast toast(QStringLiteral("Saved"), QxToast::Information, &host);

    QCOMPARE(toast.timeout(), QxToast::DefaultTimeout);
    QCOMPARE(toast.fadeDuration(), QxToast::DefaultFadeDuration);
    QCOMPARE(toast.isPaused(), false);
    QCOMPARE(toast.isDismissing(), false);
    QCOMPARE(toast.parent(), &host);

    // A toast never competes for the keyboard: a notification that takes the
    // focus away is a bug, not a feature.
    QCOMPARE(toast.focusPolicy(), Qt::NoFocus);
    QVERIFY(toast.testAttribute(Qt::WA_ShowWithoutActivating));
}

void tst_QxToast::textAndLevel()
{
    QWidget host;
    QxToast toast(QStringLiteral("Saved"), QxToast::Success, &host);

    QCOMPARE(toast.text(), QStringLiteral("Saved"));
    QCOMPARE(toast.level(), QxToast::Success);

    toast.setText(QStringLiteral("Sent"));
    QCOMPARE(toast.text(), QStringLiteral("Sent"));

    toast.setLevel(QxToast::Error);
    QCOMPARE(toast.level(), QxToast::Error);

    // Changing the text changes the size, and whoever placed the toast has to
    // hear about it.
    QSignalSpy spy(&toast, &QxToast::sizeHintChanged);
    const QSize before = toast.sizeHint();
    toast.setText(QStringLiteral("A message long enough that wrapping it takes more than one line, "
                                 "which is what makes the plate grow past its minimum height."));
    QCOMPARE(spy.count(), 1);
    QVERIFY(toast.sizeHint().height() > before.height());

    // Setting the same text again is not a change.
    toast.setText(toast.text());
    QCOMPARE(spy.count(), 1);
}

void tst_QxToast::accentFollowsTheLevel()
{
    QWidget host;
    QxToast information(QStringLiteral("i"), QxToast::Information, &host);
    QxToast success(QStringLiteral("s"), QxToast::Success, &host);
    QxToast warning(QStringLiteral("w"), QxToast::Warning, &host);
    QxToast error(QStringLiteral("e"), QxToast::Error, &host);

    // The three levels that carry a meaning of their own land in the corner of
    // the colour wheel they are read in: green, amber, red.
    const int successHue = success.accentColor().hslHue();
    const int warningHue = warning.accentColor().hslHue();
    const int errorHue = error.accentColor().hslHue();
    QVERIFY(successHue >= 100 && successHue <= 140);
    QVERIFY(warningHue >= 30 && warningHue <= 60);
    QVERIFY(errorHue <= 8 || errorHue >= 352);

    // Information is the theme's own accent, so its hue is the highlight's hue.
    // An achromatic theme has no hue to keep, hence the guard.
    const QColor highlight = QApplication::palette().color(QPalette::Highlight);
    if (highlight.hslHue() >= 0) {
        QVERIFY(qAbs(information.accentColor().hslHue() - highlight.hslHue()) <= 2);
    }

    // Four levels have to be four colours, otherwise the mapping says nothing.
    const QList<QColor> accents{information.accentColor(), success.accentColor(), warning.accentColor(),
                                error.accentColor()};
    for (int i = 0; i < accents.count(); ++i) {
        QVERIFY(accents.at(i).isValid());
        QCOMPARE(accents.at(i).alpha(), 255);
        for (int j = i + 1; j < accents.count(); ++j) {
            QVERIFY(accents.at(i) != accents.at(j));
        }
    }
}

void tst_QxToast::sizeDependsOnTheText()
{
    QWidget host;
    QxToast one(QStringLiteral("Saved"), QxToast::Information, &host);
    QxToast many(QStringLiteral("The device did not answer the third time in a row, so the "
                                "connection was given up and the whole queue was emptied."),
                 QxToast::Warning, &host);

    // Same width, taller plate: the longer text wrapped.
    QVERIFY(many.heightForWidth(300) > one.heightForWidth(300));

    // The plate keeps a minimum height, so a one word message is still a bar
    // rather than a line of text.
    QVERIFY(one.heightForWidth(300) >= 2 * QxToast::ShadowMargin + 34);

    // The widget is wider than its plate: the shadow needs room around it.
    QCOMPARE(one.sizeHint().width(), QxToast::MaxPlateWidth + 2 * QxToast::ShadowMargin);
    QVERIFY(one.sizeHint().height() > 0);
}

void tst_QxToast::timeoutAndFade()
{
    QWidget host;
    QxToast toast(QStringLiteral("Saved"), QxToast::Information, &host);

    toast.setTimeout(2500);
    QCOMPARE(toast.timeout(), 2500);

    // A non-positive timeout is the documented "stays until dismissed" case.
    toast.setTimeout(0);
    QCOMPARE(toast.timeout(), 0);
    toast.setTimeout(-1);
    QCOMPARE(toast.timeout(), -1);

    // Zero turns the animation off, which is what makes the life cycle immediate.
    toast.setFadeDuration(0);
    QCOMPARE(toast.fadeDuration(), 0);
    toast.setFadeDuration(-20);
    QCOMPARE(toast.fadeDuration(), 0);
    toast.setFadeDuration(300);
    QCOMPARE(toast.fadeDuration(), 300);
}

void tst_QxToast::dismissIsIdempotent()
{
    QWidget host;
    QxToast toast(QStringLiteral("Saved"), QxToast::Error, &host);
    toast.setFadeDuration(0);

    int dismissRequested = 0;
    int dismissed = 0;
    connect(&toast, &QxToast::dismissRequested, this, [&dismissRequested]() {
        ++dismissRequested;
    });
    connect(&toast, &QxToast::dismissed, this, [&dismissed]() {
        ++dismissed;
    });

    toast.dismiss();
    // Leaving takes one request and one confirmation, and with the animation off
    // the confirmation is synchronous.
    QCOMPARE(dismissRequested, 1);
    QCOMPARE(dismissed, 1);
    QCOMPARE(toast.isDismissing(), true);
    QVERIFY(toast.isHidden());

    // Asking again changes nothing: the toast is already on its way out, and a
    // dismissal is not something that can happen twice.
    toast.dismiss();
    QCOMPARE(dismissRequested, 1);
    QCOMPARE(dismissed, 1);
}

void tst_QxToast::iconAndMessageAreSeparate()
{
    QWidget host;
    QxToast toast(QStringLiteral("Saved"), QxToast::Warning, &host);

    // The icon is the artwork the style already uses for this meaning rather
    // than a pixmap of our own, so it keeps its meaning on every platform.
    QVERIFY(!toast.style()->standardIcon(QStyle::SP_MessageBoxWarning).isNull());

    // One label carries the icon, the other one the message.
    const QList<QLabel *> labels = toast.findChildren<QLabel *>();
    QCOMPARE(labels.count(), 2);

    // Neither of them takes the mouse: the click belongs to the toast, and it is
    // the toast that decides what a click does.
    for (QLabel *label : labels) {
        QVERIFY(label->testAttribute(Qt::WA_TransparentForMouseEvents));
    }
}

/* ---------------------------------------------------------------- manager -- */

/*!
 * The stack. Everything asserted here is the queue the manager reports - who is
 * in it, in which order and when they leave - because that is the part that has
 * to be right on every platform. The widget is placed and shown, but nothing is
 * ever compared pixelwise: the rendering is smoke tested offscreen and left at
 * that.
 */
class tst_QxToastManager : public QObject
{
    Q_OBJECT
private slots:
    void defaults();
    void withoutAHost();
    void stacking();
    void eviction();
    void timeoutDismisses();
    void persistentStays();
    void hoverHoldsTheCountdown();
    void clickDismisses();
    void placement();
    void followsTheHost();
    void dismissAll();
    void outlivesNothing();
    void shellIntegration();
private:
    /*! The rectangle the plate of \a toast occupies inside its own widget. */
    static QRect plateRect(const QxToast *toast);
};

QRect tst_QxToastManager::plateRect(const QxToast *toast)
{
    return QRect(toast->x() + QxToast::ShadowMargin, toast->y() + QxToast::ShadowMargin,
                 toast->width() - 2 * QxToast::ShadowMargin, toast->height() - 2 * QxToast::ShadowMargin);
}

void tst_QxToastManager::defaults()
{
    QWidget host;
    QxToastManager manager(&host);

    QCOMPARE(manager.host(), &host);
    QCOMPARE(manager.maxVisible(), 3);
    QCOMPARE(manager.position(), QxToastManager::TopRight);
    QCOMPARE(manager.margin(), 12);
    QCOMPARE(manager.spacing(), 8);
    QCOMPARE(manager.maxWidth(), 360);
    QCOMPARE(manager.fadeDuration(), QxToast::DefaultFadeDuration);
    QCOMPARE(manager.count(), 0);
    QVERIFY(manager.toasts().isEmpty());
    QVERIFY(manager.toastAt(0) == Q_NULLPTR);
    QVERIFY(manager.toastAt(-1) == Q_NULLPTR);

    // The limits are the ones a plate can really take, so the width the manager
    // places at is never a different number from the width the toast draws.
    manager.setMaxVisible(0);
    QCOMPARE(manager.maxVisible(), 1);
    manager.setMaxWidth(10000);
    QCOMPARE(manager.maxWidth(), QxToast::MaxPlateWidth);
    manager.setMaxWidth(1);
    QCOMPARE(manager.maxWidth(), QxToast::MinPlateWidth);
    manager.setMargin(-5);
    QCOMPARE(manager.margin(), 0);
    manager.setSpacing(-5);
    QCOMPARE(manager.spacing(), 0);
    manager.setFadeDuration(-5);
    QCOMPARE(manager.fadeDuration(), 0);

    manager.setPosition(QxToastManager::BottomLeft);
    QCOMPARE(manager.position(), QxToastManager::BottomLeft);
}

void tst_QxToastManager::withoutAHost()
{
    // A manager with no host has nowhere to put a toast; it says so rather than
    // handing out one that nobody owns.
    QxToastManager manager(Q_NULLPTR);
    QVERIFY(manager.host() == Q_NULLPTR);
    QVERIFY(manager.show(QStringLiteral("Nowhere")) == Q_NULLPTR);
    QCOMPARE(manager.count(), 0);

    // Placing and emptying an empty stack is harmless.
    manager.dismissAll();
    QCOMPARE(manager.count(), 0);
}

void tst_QxToastManager::stacking()
{
    QWidget host;
    QxToastManager manager(&host);
    manager.setFadeDuration(0);

    int shown = 0;
    QList<QxToast *> seen;
    connect(&manager, &QxToastManager::toastShown, this, [&shown, &seen](QxToast *toast) {
        ++shown;
        seen.append(toast);
    });

    QxToast *first = manager.show(QStringLiteral("one"));
    QxToast *second = manager.show(QStringLiteral("two"), QxToast::Warning);
    QxToast *third = manager.show(QStringLiteral("three"), QxToast::Error, 5000);

    QVERIFY(first != Q_NULLPTR);
    QCOMPARE(manager.count(), 3);
    QCOMPARE(shown, 3);
    QCOMPARE(seen.count(), 3);

    // The stack is in arrival order, and the oldest one is the first to leave.
    const QList<QxToast *> stack = manager.toasts();
    QCOMPARE(stack.count(), 3);
    QCOMPARE(stack.at(0), first);
    QCOMPARE(stack.at(1), second);
    QCOMPARE(stack.at(2), third);
    QCOMPARE(manager.toastAt(0), first);
    QCOMPARE(manager.toastAt(2), third);
    QCOMPARE(manager.toastAt(3), static_cast<QxToast *>(Q_NULLPTR));
    QCOMPARE(manager.contains(second), true);
    QCOMPARE(manager.contains(Q_NULLPTR), false);

    QCOMPARE(first->text(), QStringLiteral("one"));
    QCOMPARE(second->level(), QxToast::Warning);
    QCOMPARE(third->timeout(), 5000);

    // The toasts belong to the host, which is what puts them above everything
    // else the window holds.
    QCOMPARE(first->parentWidget(), &host);
}

void tst_QxToastManager::eviction()
{
    QWidget host;
    QxToastManager manager(&host);
    manager.setFadeDuration(0);
    manager.setMaxVisible(2);

    QList<QString> dismissed;
    connect(&manager, &QxToastManager::toastDismissed, this, [&dismissed](QxToast *toast) {
        dismissed.append(toast->text());
    });

    QxToast *one = manager.show(QStringLiteral("one"));
    QxToast *two = manager.show(QStringLiteral("two"));
    QCOMPARE(manager.count(), 2);
    QCOMPARE(dismissed.count(), 0);

    // The third one is the reason the first one leaves: the column never gets
    // taller than the screen can take.
    QxToast *three = manager.show(QStringLiteral("three"));
    QCOMPARE(manager.count(), 2);
    QCOMPARE(dismissed.count(), 1);
    QCOMPARE(dismissed.at(0), QStringLiteral("one"));
    QCOMPARE(manager.contains(one), false);

    QList<QxToast *> stack = manager.toasts();
    QCOMPARE(stack.count(), 2);
    QCOMPARE(stack.at(0), two);
    QCOMPARE(stack.at(1), three);

    QxToast *four = manager.show(QStringLiteral("four"));
    QCOMPARE(manager.count(), 2);
    QCOMPARE(dismissed.count(), 2);
    QCOMPARE(dismissed.at(1), QStringLiteral("two"));

    // Shrinking the limit evicts on the spot, and the newest one survives: the
    // message that just arrived is the one worth reading.
    manager.setMaxVisible(1);
    QCOMPARE(manager.count(), 1);
    QCOMPARE(manager.toastAt(0), four);
}

void tst_QxToastManager::timeoutDismisses()
{
    QWidget host;
    QxToastManager manager(&host);
    manager.setFadeDuration(0);

    int dismissed = 0;
    connect(&manager, &QxToastManager::toastDismissed, this, [&dismissed]() {
        ++dismissed;
    });

    manager.show(QStringLiteral("gone soon"), QxToast::Information, 60);
    QCOMPARE(manager.count(), 1);

    QTest::qWait(200);
    QCOMPARE(manager.count(), 0);
    QCOMPARE(dismissed, 1);

    // Every toast keeps its own clock, so a long one outlives a short one.
    manager.show(QStringLiteral("short"), QxToast::Information, 40);
    manager.show(QStringLiteral("long"), QxToast::Information, 400);
    QCOMPARE(manager.count(), 2);
    QTest::qWait(200);
    QCOMPARE(manager.count(), 1);
    QCOMPARE(manager.toastAt(0)->text(), QStringLiteral("long"));
}

void tst_QxToastManager::persistentStays()
{
    QWidget host;
    QxToastManager manager(&host);
    manager.setFadeDuration(0);

    // A negative timeout is the "until something dismisses it" case; the clock
    // must not take it away behind the caller's back.
    QxToast *toast = manager.show(QStringLiteral("stays"), QxToast::Warning, -1);
    QCOMPARE(manager.count(), 1);
    QTest::qWait(200);
    QCOMPARE(manager.count(), 1);
    QCOMPARE(manager.toastAt(0), toast);
}

void tst_QxToastManager::hoverHoldsTheCountdown()
{
    QWidget host;
    QxToastManager manager(&host);
    manager.setFadeDuration(0);

    QxToast *toast = manager.show(QStringLiteral("read me"), QxToast::Information, 80);
    QCOMPARE(manager.count(), 1);

    // Hovering holds the countdown back: a message that is being read must not
    // vanish from under the pointer.
    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(toast, &enter);
    QCOMPARE(toast->isPaused(), true);
    QTest::qWait(200);
    QCOMPARE(manager.count(), 1);
    QCOMPARE(toast->isPaused(), true);

    // Leaving gives back the time that was left rather than the whole timeout.
    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(toast, &leave);
    QCOMPARE(toast->isPaused(), false);
    QCOMPARE(manager.count(), 1);

    QTest::qWait(300);
    QCOMPARE(manager.count(), 0);
}

void tst_QxToastManager::clickDismisses()
{
    QWidget host;
    QxToastManager manager(&host);
    manager.setFadeDuration(0);

    QxToast *toast = manager.show(QStringLiteral("click me"));
    QCOMPARE(manager.count(), 1);

    int clicked = 0;
    connect(toast, &QxToast::clicked, this, [&clicked]() {
        ++clicked;
    });

    // Sent rather than posted: what matters here is the widget's reaction, and a
    // real click needs a window, which is what this suite stays away from. The
    // three position arguments are local, window and global - the spelling both
    // Qt 5.15 and Qt 6 take without reaching for a deprecated one.
    const QPointF spot(5, 5);
    QMouseEvent press(QEvent::MouseButtonPress, spot, spot, spot, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent release(QEvent::MouseButtonRelease, spot, spot, spot, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(toast, &press);
    QApplication::sendEvent(toast, &release);

    QCOMPARE(clicked, 1);
    QCOMPARE(manager.count(), 0);

    // A button other than the left one is not a dismissal.
    QxToast *other = manager.show(QStringLiteral("right click"));
    QMouseEvent otherPress(QEvent::MouseButtonPress, spot, spot, spot, Qt::RightButton, Qt::RightButton,
                           Qt::NoModifier);
    QMouseEvent otherRelease(QEvent::MouseButtonRelease, spot, spot, spot, Qt::RightButton, Qt::NoButton,
                             Qt::NoModifier);
    QApplication::sendEvent(other, &otherPress);
    QApplication::sendEvent(other, &otherRelease);
    QCOMPARE(manager.count(), 1);
}

void tst_QxToastManager::placement()
{
    QWidget host;
    host.resize(400, 300);
    QxToastManager manager(&host);
    manager.setFadeDuration(0);
    manager.setPosition(QxToastManager::TopRight);
    manager.setMargin(12);
    manager.setSpacing(8);
    manager.setMaxWidth(360);

    QxToast *first = manager.show(QStringLiteral("one"));
    QxToast *second = manager.show(QStringLiteral("two messages, one column, both the same width"));
    QCOMPARE(manager.count(), 2);

    // One column: every plate is as wide as the widest one allowed.
    QCOMPARE(plateRect(first).width(), 360);
    QCOMPARE(plateRect(second).width(), 360);

    // Anchored to the top right corner.
    QCOMPARE(plateRect(first).right(), host.width() - manager.margin() - 1);
    QCOMPARE(plateRect(first).top(), manager.margin());

    // Stacked downwards, one spacing apart.
    QCOMPARE(plateRect(second).top(), plateRect(first).bottom() + manager.spacing() + 1);

    // Anchored to the bottom left corner, the column grows upwards.
    manager.setPosition(QxToastManager::BottomLeft);
    QCOMPARE(plateRect(first).left(), manager.margin());
    QCOMPARE(plateRect(second).bottom(), host.height() - manager.margin() - 1);
    QCOMPARE(plateRect(second).top(), plateRect(first).bottom() + manager.spacing() + 1);
}

void tst_QxToastManager::followsTheHost()
{
    QWidget host;
    host.resize(400, 300);
    // Shown, because a widget that was never given a window keeps its new size
    // to itself: no QEvent::Resize, and the stack would stay where it was.
    host.show();
    QxToastManager manager(&host);
    manager.setFadeDuration(0);
    manager.setPosition(QxToastManager::TopRight);
    manager.setMaxWidth(200);

    QxToast *toast = manager.show(QStringLiteral("anchored"));
    const int before = toast->x();

    // The stack is anchored to the edges of the host, so a resize moves it.
    host.resize(700, 300);
    QCOMPARE(manager.count(), 1);
    QCOMPARE(toast->x(), before + 300);
    QCOMPARE(plateRect(toast).right(), host.width() - manager.margin() - 1);

    // A host narrower than the requested width shrinks the plate instead of
    // pushing it out of the window.
    host.resize(220, 300);
    QVERIFY(plateRect(toast).width() <= host.width() - 2 * manager.margin());
    QVERIFY(toast->x() >= 0);
}

void tst_QxToastManager::dismissAll()
{
    QWidget host;
    QxToastManager manager(&host);
    manager.setFadeDuration(0);

    manager.show(QStringLiteral("one"), QxToast::Information, -1);
    manager.show(QStringLiteral("two"), QxToast::Information, -1);
    manager.show(QStringLiteral("three"), QxToast::Information, -1);
    QCOMPARE(manager.count(), 3);

    int dismissed = 0;
    connect(&manager, &QxToastManager::toastDismissed, this, [&dismissed]() {
        ++dismissed;
    });

    manager.dismissAll();
    QCOMPARE(manager.count(), 0);
    QCOMPARE(dismissed, 3);

    // Twice is harmless.
    manager.dismissAll();
    QCOMPARE(manager.count(), 0);
    QCOMPARE(dismissed, 3);
}

void tst_QxToastManager::outlivesNothing()
{
    // The toasts are children of the host and this manager is parented to it, so
    // closing the window takes the whole stack with it.
    QWidget *host = new QWidget;
    QxToastManager *manager = new QxToastManager(host, host);
    manager->setFadeDuration(0);
    manager->show(QStringLiteral("gone"), QxToast::Information, -1);
    QCOMPARE(manager->count(), 1);

    QPointer<QxToastManager> guard = manager;
    delete host;
    QVERIFY(guard.isNull());
}

void tst_QxToastManager::shellIntegration()
{
    QxAppShell shell;

    // The shell creates the stack on first use and keeps it, so a caller can tune
    // the placement before the first message ever arrives.
    QxToastManager *manager = shell.toastManager();
    QVERIFY(manager != Q_NULLPTR);
    QCOMPARE(shell.toastManager(), manager);
    QCOMPARE(manager->host(), &shell);

    QxToast *toast = shell.showToast(QStringLiteral("Saved"), QxToast::Success);
    QVERIFY(toast != Q_NULLPTR);
    QCOMPARE(manager->count(), 1);
    QCOMPARE(manager->toastAt(0), toast);
    QCOMPARE(toast->text(), QStringLiteral("Saved"));
    QCOMPARE(toast->level(), QxToast::Success);
    QCOMPARE(toast->timeout(), QxToast::DefaultTimeout);

    // The toast is part of the shell, which is what puts it over the ribbon, the
    // rail and the docks.
    QCOMPARE(toast->parentWidget(), &shell);

    QxToast *later = shell.showToast(QStringLiteral("Failed"), QxToast::Error, 9000);
    QCOMPARE(manager->count(), 2);
    QCOMPARE(later->timeout(), 9000);
}

TEST_ADD(tst_QxToast)
TEST_ADD(tst_QxToastManager)

#include "tst_qxtoast.moc"
