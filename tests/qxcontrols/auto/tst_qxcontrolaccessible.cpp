/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcontrols/qxcontrol.h"
#include "qxcontrols/qxcontrolaccessible.h"

#include <QAccessible>
#include <QApplication>
#include <QWidget>

QX_CONTROLS_USE_NAMESPACE

namespace
{
/*! A control that has a number in it: a role and a state cannot say it, so
 *  the control brings a face that can. */
class ValuedControl : public QxControl
{
public:
    QAccessibleInterface *accessibleInterface() override
    {
        return new QxControlAccessible(this, QAccessible::Slider);
    }
};

/*! An accessible interface is not owned by its caller and cannot be deleted:
 *  the registry hands it out and takes it back. */
void releaseInterface(QAccessibleInterface *iface)
{
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! A control that knows what it is: a check box that is on or off. */
class ToggleControl : public QxControl
{
public:
    bool on = false;

    QAccessible::Role accessibleRole() const override
    {
        return QAccessible::CheckBox;
    }
    void fillAccessibleState(QAccessible::State *state) const override
    {
        state->checkable = true;
        state->checked = on;
    }
};
}   // namespace

class tst_QxControlAccessible : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aControlThatDoesNotSayIsAClient();
    void theRoleIsWhatTheControlSays();
    void theStateCarriesWhatTheControlAdds();
    void onlyControlsGetTheFace();
    void aControlWithMoreToSayBringsItsOwnFace();
private:
    bool m_wasActive = false;
};

void tst_QxControlAccessible::init()
{
    // Accessibility is only consulted while it is active, which on a desktop
    // without an assistive tool it is not.
    m_wasActive = QAccessible::isActive();
    QAccessible::setActive(true);
}

void tst_QxControlAccessible::cleanup()
{
    QAccessible::setActive(m_wasActive);
}

/*! The honest answer while a control has not said what it is: the role a
 *  plain widget gets, not a guess. */
void tst_QxControlAccessible::aControlThatDoesNotSayIsAClient()
{
    QxControl control;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&control);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Client);
    releaseInterface(iface);
}

/*! What a control says it is is what an assistive tool is told. */
void tst_QxControlAccessible::theRoleIsWhatTheControlSays()
{
    ToggleControl control;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&control);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::CheckBox);
    releaseInterface(iface);
}

/*! A control fills in what is true of it on top of what any widget answers:
 *  focusable comes from the widget, checked from the control. */
void tst_QxControlAccessible::theStateCarriesWhatTheControlAdds()
{
    ToggleControl control;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&control);
    QVERIFY(iface);

    QVERIFY(iface->state().focusable);
    QVERIFY(iface->state().checkable);
    QVERIFY(!iface->state().checked);
    releaseInterface(iface);

    control.on = true;
    QAccessibleInterface *onIface = QAccessible::queryAccessibleInterface(&control);
    QVERIFY(onIface);
    QVERIFY(onIface->state().checked);
    releaseInterface(onIface);
}

/*! The factory answers for controls and leaves everything else to the
 *  factories that were there before it. */
void tst_QxControlAccessible::onlyControlsGetTheFace()
{
    QWidget plain;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&plain);
    QVERIFY(iface);
    QVERIFY(dynamic_cast<QxControlAccessible *>(iface) == nullptr);
    releaseInterface(iface);

    QxControl control;
    QAccessibleInterface *controlIface = QAccessible::queryAccessibleInterface(&control);
    QVERIFY(dynamic_cast<QxControlAccessible *>(controlIface) != nullptr);
    releaseInterface(controlIface);
}

/*! A role and a state describe most controls; a control that carries a value
 *  answers with a face of its own and the factory hands that one out. */
void tst_QxControlAccessible::aControlWithMoreToSayBringsItsOwnFace()
{
    ValuedControl control;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&control);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Slider);
    releaseInterface(iface);
}

TEST_ADD(tst_QxControlAccessible)

#include "tst_qxcontrolaccessible.moc"
