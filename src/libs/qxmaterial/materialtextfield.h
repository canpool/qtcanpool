/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALTEXTFIELD_H
#define MATERIALTEXTFIELD_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

class QLineEdit;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTextFieldPrivate;

/*!
 * Material's text field: one line of text with a label that knows its place.
 *
 * QLineEdit's placeholder is not this label and cannot be made into it: a
 * placeholder disappears the moment there is something to say, and this label
 * does the opposite - it is the name of the field, so it goes on standing while
 * the field is filled, only smaller and out of the way. That movement is the
 * whole of what a Material text field is: while the field is empty and has no
 * keyboard the label reads as the prompt; as soon as either is true it floats,
 * and the field says what it is holding as well as what it wants.
 *
 * The two variants are the two answers Material gives to "where does the field
 * end":
 *
 * - \c Filled is an opening with a floor: a container tinted with the page's
 *   own ink, an indicator line under it that thickens and takes the accent
 *   while the field has the keyboard;
 * - \c Outlined is a stroke with a gap: the label floats onto the outline and
 *   opens a window in it, because a stroke behind a twelve point label is a
 *   stroke through the label.
 */
class QX_MATERIAL_EXPORT MaterialTextField : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(Variant variant READ variant WRITE setVariant)
    Q_PROPERTY(QString label READ label WRITE setLabel)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
public:
    enum class Variant {
        Filled,  /*!< A tinted container with an indicator line under it. */
        Outlined /*!< A stroke, with the label sitting in a gap in it. */
    };
    Q_ENUM(Variant)

    explicit MaterialTextField(QWidget *parent = Q_NULLPTR);
    ~MaterialTextField() override;

    Variant variant() const;
    QString label() const;
    QString text() const;

    /*! The line editor the field writes through. It is the field's own child
     *  and the field is what the label and the stroke answer to, so this is
     *  handed out for the one thing the field cannot do for a subclass: a
     *  field whose writing is not the point - a picker, which says a day the
     *  calendar chose - has to put the editor out of the writing's way. */
    QLineEdit *lineEdit() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setVariant(Variant variant);
    void setLabel(const QString &label);
    void setText(const QString &text);
    void clear();

Q_SIGNALS:
    /*! Emitted whenever the text changes, whether typed or set. */
    void textChanged(const QString &text);
    /*! The field was left, or Enter was pressed: the reading is finished. */
    void editingFinished();
    void returnPressed();
protected:
    qreal cornerRadius() const override;
    /*!
     * Nothing: a text field is not a control that answers a pointer with a
     * wash, and its focus is the indicator line or the stroke, which it draws
     * itself. An empty chrome is the base's own way of being told there is
     * nothing of that kind to draw.
     */
    QRectF chromeRect() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    /*! The field itself: the whole widget when filled, inset from the top when
     *  outlined so the floated label has somewhere to stand. */
    QRectF fieldRect() const;
    /*! Where the label stands at \a progress: 0 is in the field, 1 is out of
     *  the way. */
    QRectF labelRect(qreal progress) const;
    QFont labelFont(qreal progress) const;
    /*! How far out of the way the label is right now: 0 or 1 unless a move is
     *  under way, in which case it is where the move has got to. The answer is
     *  read from the field rather than remembered, because a label whose place
     *  depends on an event that reached this widget can end up standing in the
     *  text it names. */
    qreal labelProgress() const;
    /*! Sends the label to where the field says it belongs, animating unless
     *  the theme has motion turned off. */
    void updateLabel();

    Q_DISABLE_COPY(MaterialTextField)
    QX_DECLARE_PRIVATE(MaterialTextField)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALTEXTFIELD_H
