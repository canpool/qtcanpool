/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTNUMBERBOX_H
#define FLUENTNUMBERBOX_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentNumberBoxPrivate;
class FluentNumberBoxAccessible;

/*!
 * Fluent's number box: a field with a number in it and two chevrons beside it.
 *
 * QSpinBox is a styled widget with a spin box in it - the up and down arrows
 * stacked in a corner, asked for by a style that any Fluent application has to
 * replace anyway. This one is drawn: the field is an opening in the page with a
 * hairline round it, and the stepper is two chevrons in a column of their own,
 * each one a command rather than half of a dial.
 *
 * What it keeps from Qt is the part worth keeping: the text is a QLineEdit, so
 * selection, the cursor, the clipboard and an input method all behave the way
 * the platform taught the user to expect. What it adds is the rule that makes a
 * number box a number box - the value is the truth and the text is a way of
 * saying it. Text that is not a number never reaches the value, and leaving the
 * field puts the text back to what the value is, so the two cannot drift apart.
 */
class QX_FLUENT_EXPORT FluentNumberBox : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(int singleStep READ singleStep WRITE setSingleStep)
    Q_PROPERTY(int pageStep READ pageStep WRITE setPageStep)
public:
    explicit FluentNumberBox(QWidget *parent = Q_NULLPTR);
    ~FluentNumberBox() override;

    int value() const;
    int minimum() const;
    int maximum() const;
    int singleStep() const;
    int pageStep() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setValue(int value);
    void setMinimum(int minimum);
    void setMaximum(int maximum);
    void setRange(int minimum, int maximum);
    void setSingleStep(int step);
    void setPageStep(int step);

Q_SIGNALS:
    /*! Emitted whenever the value changes, whatever changed it. */
    void valueChanged(int value);
protected:
    qreal cornerRadius() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    /*!
     * Watches the editor, because the editor is where the keyboard and the
     * wheel arrive: with the field carrying the focus, the control would
     * otherwise only hear about the pointer on its stepper.
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    QAccessibleInterface *accessibleInterface() override;
private:
    /*! The half of the stepper at \a pos: 0 the upper chevron, 1 the lower one,
     *  -1 neither. */
    int stepperAt(const QPointF &pos) const;
    QRectF stepperRect(int half) const;
    /*! Adds \a delta to the value, clamped, and makes the field follow. */
    void step(int delta);
    /*! Reads the field: a number becomes the value, anything else is dropped,
     *  and the field is put back to what the value is either way. */
    void commitEditor();
    /*! Writes the value into the field. \a force overwrites what the user is
     *  typing; without it, a field that has the keyboard is left alone. */
    void syncEditor(bool force);
    bool handleKey(QKeyEvent *event);

    Q_DISABLE_COPY(FluentNumberBox)
    QX_DECLARE_PRIVATE(FluentNumberBox)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTNUMBERBOX_H
