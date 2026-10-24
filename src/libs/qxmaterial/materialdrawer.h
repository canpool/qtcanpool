/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALDRAWER_H
#define MATERIALDRAWER_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

class QAction;
class QKeyEvent;
class QMouseEvent;
class QResizeEvent;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialDrawerPrivate;

/*!
 * Material's navigation drawer: the destinations that do not fit on the edge,
 * kept in a panel that comes and goes.
 *
 * Material has three answers to "where can I go from here" and they are three
 * answers to three screen sizes: a rail at the side of a wide one, a bar along
 * the bottom of a narrow one, and this - a panel that comes out over the page
 * when the destinations are too many for either. The panel is the same list of
 * destinations the rail shows, drawn the same way, only wider: a pill behind
 * the icon of the one in force, the icon and its name turning to the accent
 * with it, and room for the name to be a name rather than a caption.
 *
 * It comes in two of Material's three variants, and the variant is a question
 * about what the drawer does to the page rather than about how it looks:
 *
 * - \c Temporary comes out **over** the page, on a scrim that dims it, and
 *   goes away when the scrim or Escape is pressed. It stands where it is put,
 *   so it is not a child of a layout: a drawer laid out by the page would push
 *   the page aside, which is the other variant's job.
 * - \c Persistent is part of the page: it takes its width from the layout
 *   while it is open and gives it back when it closes, so the page grows and
 *   shrinks beside it. There is no scrim, because nothing is covered.
 *
 * A temporary drawer slides in from the edge it is on rather than appearing,
 * and asked for less motion it simply is where it is going. A persistent one
 * does not slide: it gives the page its room back the moment it closes,
 * because what moves then is the page's geometry and not the drawer.
 *
 * A drawer holds destinations and the things a list of destinations needs -
 * section headings between them and a rule where a section ends - because a
 * navigation drawer with nothing but destinations in it is a rail with a
 * wider label column.
 *
 * \sa MaterialNavigationRail, MaterialBottomNavigation
 */
class QX_MATERIAL_EXPORT MaterialDrawer : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(bool open READ isOpen WRITE setOpen NOTIFY openChanged)
public:
    /*!
     * What the drawer does to the page. \c Temporary covers it and dims it;
     * \c Persistent takes its room and gives it back.
     */
    enum class Variant {
        Temporary,
        Persistent
    };
    Q_ENUM(Variant)

    explicit MaterialDrawer(QWidget *parent = Q_NULLPTR);
    ~MaterialDrawer() override;

    Variant variant() const;
    void setVariant(Variant variant);

    /*! Adds \a label as a destination and returns it. The drawer owns it. */
    QAction *addDestination(const QString &label, const QIcon &icon = QIcon());
    void removeDestination(QAction *action);
    /*! Puts a heading over the destinations that follow it, ended by a rule. */
    void addSection(const QString &title);
    /*! Ends a section: a rule across the drawer. */
    void addDivider();
    void clear();

    QList<QAction *> destinations() const;
    QAction *currentDestination() const;
    int currentIndex() const;

    bool isOpen() const;

    /*! Where destination \a index stands in the drawer, in the drawer's own
     *  coordinates; an empty rectangle when there is no such destination or it
     *  has been scrolled out of sight. */
    QRect destinationRect(int index) const;

    /*! Hands \a widget the top of the drawer, above every destination. It is
     *  where Material puts a title and the account the destinations belong to. */
    void setHeaderWidget(QWidget *widget);
    QWidget *headerWidget() const;

    /*! How far the panel travels, in device independent pixels. */
    static int panelWidth();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void setCurrentDestination(QAction *action);
    void setOpen(bool open);
    void open();
    void close();
    void toggle();

Q_SIGNALS:
    void currentIndexChanged(int index);
    void currentDestinationChanged(QAction *action);
    void openChanged(bool open);
protected:
    /*! Nothing: a drawer is a column of destinations with a pill behind one of
     *  them, and the wash and the ring belong to a destination. */
    QRectF chromeRect() const override;

    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(MaterialDrawer)
    QX_DECLARE_PRIVATE(MaterialDrawer)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALDRAWER_H
