/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALTOPAPPBAR_H
#define MATERIALTOPAPPBAR_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

class QAction;
class QKeyEvent;
class QMouseEvent;
class QResizeEvent;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTopAppBarPrivate;

/*!
 * Material's top app bar: the top of a Material screen, and the one thing on
 * every one of them.
 *
 * A screen needs somewhere to say where the user is, somewhere to put the way
 * out or the way back, and somewhere to put the two or three things that can
 * be done to the screen as a whole. Material's answer is one bar with three
 * places in it - a leading icon, the name of the screen, and the trailing
 * icons - and Qt has no such bar: a QToolBar is a row of buttons whose look
 * belongs to the style, a QMenuBar is a menu, and a title is a label the
 * application has to place and keep placed.
 *
 * Which of the four shapes the bar takes is a question about the screen, not
 * about taste. A \c Small bar names the screen and nothing else; a
 * \c CenterAligned one is the same height with the name in the middle, which
 * is what a screen with one thing on it wants; \c Medium and \c Large give the
 * name room of its own under the icons, which is what a screen that is a
 * place - the top of a list the user will scroll a long way through - wants,
 * and is why they are taller rather than bolder.
 *
 * What the bar says about being scrolled under is a change of surface, not a
 * line and not a shadow: at rest it is the page's own colour, and once content
 * has moved under it, it is the surface one step away - which is what \a
 * Material calls on-scroll elevation, and why \c setScrolled() exists rather
 * than the bar watching a scroll area it was never given.
 *
 * An icon here is a QAction, because that is what an application already has:
 * connect to it, disable it, check it, read it back from a menu of its own.
 * The bar owns every action it shows.
 */
class QX_MATERIAL_EXPORT MaterialTopAppBar : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(Variant variant READ variant WRITE setVariant)
    Q_PROPERTY(bool scrolled READ isScrolled WRITE setScrolled)
public:
    /*! The shape the bar takes, and with it how much room the name gets. */
    enum class Variant {
        Small,        /*!< 56 - the name beside the icons. */
        CenterAligned, /*!< 56 - the same room, with the name in the middle. */
        Medium,       /*!< 112 - the name under the icons, at 22. */
        Large         /*!< 152 - the name under the icons, at 36. */
    };
    Q_ENUM(Variant)

    explicit MaterialTopAppBar(QWidget *parent = Q_NULLPTR);
    ~MaterialTopAppBar() override;

    QString title() const;
    void setTitle(const QString &title);

    Variant variant() const;
    void setVariant(Variant variant);

    /*! True once content has moved under the bar, which is the bar's cue to
     *  leave the page's colour for the surface one step away. */
    bool isScrolled() const;
    void setScrolled(bool scrolled);

    /*! Puts \a icon in the leading place - the way out, the way back, or the
     *  way into what is beside this screen - and returns it. There is one
     *  leading place, so a second call takes the first one's place. */
    QAction *setLeadingAction(const QString &text, const QIcon &icon = QIcon());
    void clearLeadingAction();
    QAction *leadingAction() const;

    /*! Adds \a text as a trailing icon and returns it. The bar owns it. */
    QAction *addTrailingAction(const QString &text, const QIcon &icon = QIcon());
    void removeTrailingAction(QAction *action);
    void clearTrailingActions();
    QList<QAction *> trailingActions() const;

    /*! The room the bar asks for at \a variant, which is the whole of what
     *  the four are told apart by. */
    static int heightFor(Variant variant);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
protected:
    /*! Fully round: the wash an icon takes is a disc, which is what Material
     *  gives an icon button - the bar's own corners are square, and are drawn
     *  in the surface, not here. */
    qreal cornerRadius() const override;
    /*! The one icon the pointer or the keyboard is on: a bar that lit up as a
     *  whole would say nothing about which of its icons is about to run. */
    QRectF chromeRect() const override;

    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(MaterialTopAppBar)
    QX_DECLARE_PRIVATE(MaterialTopAppBar)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALTOPAPPBAR_H
