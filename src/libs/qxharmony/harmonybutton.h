/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef HARMONYBUTTON_H
#define HARMONYBUTTON_H

#include "qxharmony_global.h"

#include "qxcontrols/qxcontrol.h"

QX_HARMONY_BEGIN_NAMESPACE

class HarmonyButtonPrivate;

/*!
 * Harmony's button: a word in a capsule, at one of the three levels this family
 * gives an action.
 *
 * The levels are how much surface the word stands on. \c Primary is the one
 * action on a surface and carries the brand's blue at full strength with the
 * word reversed out of it; \c Secondary is the action beside it, standing on the
 * family's grey thinned into the page with the word in the blue - a neutral
 * capsule rather than a thinned blue one, because here the second level is the
 * quiet one and a wash of the brand colour would make two actions compete;
 * \c Text is an action that is only a word.
 *
 * The capsule is taller than the other family's, and it is not an accident: a
 * control here is a target first and a label second, so forty high with twenty
 * either side of the word. The capsule casts nothing - the level this family
 * spends on a lift is the one a surface sits at over the page, and a button is
 * on the page.
 *
 * Pressed, it takes the base's state layer rather than shrinking: this family
 * moves things when they arrive, and a control that moved under the finger would
 * be moving the page.
 */
class QX_HARMONY_EXPORT HarmonyButton : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(Kind kind READ kind WRITE setKind NOTIFY kindChanged)
public:
    /*!
     * How much surface the action stands on, in the order this family ranks
     * them.
     */
    enum class Kind {
        Primary,   /*!< The one action on the surface: the blue itself, word reversed out. */
        Secondary, /*!< The ones beside it: a neutral capsule, the word in the blue. */
        Text       /*!< An action that is only a word: no capsule at all. */
    };
    Q_ENUM(Kind)

    explicit HarmonyButton(QWidget *parent = Q_NULLPTR);
    explicit HarmonyButton(const QString &text, QWidget *parent = Q_NULLPTR);
    ~HarmonyButton() override;

    /*! What the button says. */
    QString text() const;
    /*! How much surface it stands on. */
    Kind kind() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    /*! Replaces what the button says. What it says is also its accessible
     *  name, for as long as the application has not given it a better one. */
    void setText(const QString &text);
    /*! Puts the action at \a kind. */
    void setKind(Kind kind);

Q_SIGNALS:
    /*! Emitted when the word changed. */
    void textChanged(const QString &text);
    /*! Emitted when the level changed. */
    void kindChanged(Kind kind);
protected:
    qreal cornerRadius() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(HarmonyButton)
    QX_DECLARE_PRIVATE(HarmonyButton)
};

QX_HARMONY_END_NAMESPACE

#endif   // HARMONYBUTTON_H
