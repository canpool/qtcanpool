/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CUPERTINOLISTSECTION_H
#define CUPERTINOLISTSECTION_H

#include "qxcupertino_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

QX_CUPERTINO_BEGIN_NAMESPACE

class CupertinoListTilePrivate;
class CupertinoListSectionPrivate;

/*!
 * One row of a grouped list: an icon, a title, an optional line under it, and
 * something at the end that says what the row does.
 *
 * The row is not the group: it knows where it sits in its group and turns the
 * corners that the group's card turns - the first row turns the top two, the
 * last the bottom two, and a row alone turns all four - because the card is
 * drawn row by row rather than behind them. That is what makes the group look
 * like one surface with rows on it rather than a stack of cards.
 *
 * The row paints itself and takes the press itself; what it does with it is the
 * application's, which is why a press only reports clicked().
 */
class QX_CUPERTINO_EXPORT CupertinoListTile : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString subtitle READ subtitle WRITE setSubtitle)
    Q_PROPERTY(Trailing trailing READ trailing WRITE setTrailing)
public:
    /*! Where the row sits in its group, which decides the corners it turns. */
    enum class Position {
        Only,   /*!< Alone: all four. */
        First,  /*!< Top of the card: the two above. */
        Middle, /*!< Between rows: none. */
        Last    /*!< Bottom of the card: the two below. */
    };
    Q_ENUM(Position)

    /*! What the end of the row says. */
    enum class Trailing {
        None,     /*!< Nothing: the row is not a way to somewhere else. */
        Chevron,  /*!< A turn to the right: the row opens something. */
        Checkmark /*!< A tick: this row is the one that is chosen. */
    };
    Q_ENUM(Trailing)

    explicit CupertinoListTile(QWidget *parent = Q_NULLPTR);
    ~CupertinoListTile() override;

    QString title() const;
    QString subtitle() const;
    QIcon icon() const;
    Trailing trailing() const;
    Position position() const;

    /*! A widget at the end of the row, for a row that carries a control rather
     *  than a mark - a switch, a value. It takes the place of trailing(). */
    QWidget *trailingWidget() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setTitle(const QString &title);
    void setSubtitle(const QString &subtitle);
    void setIcon(const QIcon &icon);
    void setTrailing(Trailing trailing);
    void setTrailingWidget(QWidget *widget);
    /*! Called by the group that owns the row; a row on its own is \c Only. */
    void setPosition(Position position);

Q_SIGNALS:
    void clicked();
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    /*! The row tracks the pointer with its own wash, which knows about the
     *  corners the row turns; the base's wash is a rectangle of one radius. */
    QRectF chromeRect() const override;

    QAccessible::Role accessibleRole() const override;
private:
    /*! Puts trailingWidget() at the end of the row, on the line of the title. */
    void layoutTrailing();

    Q_DISABLE_COPY(CupertinoListTile)
    QX_DECLARE_PRIVATE(CupertinoListTile)
};

/*!
 * A group of rows: a caption above, and one card made of the rows below it.
 *
 * This is the shape an iOS settings page is made of, and the reason it is a
 * control and not a QGroupBox with a style: the rows are the card. The group
 * places them, gives the first and the last the corners the card has, and says
 * nothing else - each row draws its own line and its own share of the surface.
 */
class QX_CUPERTINO_EXPORT CupertinoListSection : public QWidget
{
    Q_OBJECT
public:
    explicit CupertinoListSection(QWidget *parent = Q_NULLPTR);
    ~CupertinoListSection() override;

    /*! The caption above the card. Empty means no caption and no space for
     *  one. */
    QString header() const;

    /*! Takes ownership of \a tile and puts it at the end of the card. */
    void addTile(CupertinoListTile *tile);
    /*! The one-call form: makes a row, fills it in and returns it. */
    CupertinoListTile *addTile(const QString &title, const QString &subtitle = QString(), const QIcon &icon = QIcon());

    QSize sizeHint() const override;

public Q_SLOTS:
    void setHeader(const QString &header);
private:
    /*! Tells every row where it sits, which is what its corners follow. */
    void assignPositions();

    Q_DISABLE_COPY(CupertinoListSection)
    QX_DECLARE_PRIVATE(CupertinoListSection)
};

QX_CUPERTINO_END_NAMESPACE

#endif   // CUPERTINOLISTSECTION_H
