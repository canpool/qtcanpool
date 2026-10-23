/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CUPERTINOTEXTFIELD_H
#define CUPERTINOTEXTFIELD_H

#include "qxcupertino_global.h"

#include "qxcontrols/qxcontrol.h"

class QLineEdit;

QX_CUPERTINO_BEGIN_NAMESPACE

class CupertinoTextFieldPrivate;

/*!
 * Cupertino's text field: a rounded field of tinted page, with room to type in.
 *
 * What makes it this family's is that it is a *filled* field - a wash of the
 * system's grey over the page rather than a box outlined on it - and that the
 * focus is said by a ring in the accent rather than by the field growing a
 * border. A field that is empty and not in use is quiet: no ring, no cursor,
 * nothing but the placeholder.
 *
 * The editing is Qt's: the field is a QLineEdit without a frame, sitting inside
 * the surface this control paints. Writing a text editor again would mean
 * writing selections, IMEs, undo and the clipboard again, and the rules of this
 * repository allow a control to be a composition - qxmaterial's text field is
 * one as well.
 *
 * \c Kind::Search draws the magnifier at the leading edge and asks for a
 * search-shaped placeholder; the two are one control because a search field is
 * a text field whose leading decoration is part of what it is, not a button the
 * application puts beside it.
 */
class QX_CUPERTINO_EXPORT CupertinoTextField : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(Kind kind READ kind WRITE setKind)
    Q_PROPERTY(bool clearButtonEnabled READ isClearButtonEnabled WRITE setClearButtonEnabled)
public:
    /*! What the field is for: typing into, or searching. */
    enum class Kind {
        Plain, /*!< A field the application owns. */
        Search /*!< A field with a magnifier at the leading edge. */
    };
    Q_ENUM(Kind)

    explicit CupertinoTextField(QWidget *parent = Q_NULLPTR);
    ~CupertinoTextField() override;

    QString text() const;
    QString placeholderText() const;
    Kind kind() const;
    /*! Whether the field offers a way to empty itself while it has something in
     *  it. On by default. */
    bool isClearButtonEnabled() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setText(const QString &text);
    void setPlaceholderText(const QString &text);
    void setKind(Kind kind);
    void setClearButtonEnabled(bool enabled);
    /*! Empties the field, as the clear button does. */
    void clear();

Q_SIGNALS:
    /*! The text changed, by typing, by clearing or by the application. */
    void textChanged(const QString &text);
    void returnPressed();
    /*! The field was emptied by its own clear button or by Escape. */
    void cleared();
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void changeEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    bool hitTest(const QPointF &pos) const override;

    QAccessible::Role accessibleRole() const override;
private:
    void syncEdit();
    void updateClearVisibility();
    void layoutEdit();
    QRectF fieldRect() const;
    QRectF magnifierRect() const;

    Q_DISABLE_COPY(CupertinoTextField)
    QX_DECLARE_PRIVATE(CupertinoTextField)
};

QX_CUPERTINO_END_NAMESPACE

#endif   // CUPERTINOTEXTFIELD_H
