/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALIMAGELIST_H
#define MATERIALIMAGELIST_H

#include "qxmaterial_global.h"

#include <QtGui/QPixmap>
#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialImageListPrivate;

/*!
 * Material's image list: a grid of pictures, each with its name on it.
 *
 * What a picture is here is a tile - the photograph, and a bar across one edge
 * of it carrying the name and, when there is one, the line under the name. The
 * pictures are given to the list and the list places them: a tile is drawn, not
 * a container the application fills, because there are only three things a
 * picture tile holds and the list can draw all three. What a page wants beyond
 * them is not a tile - a photograph with a badge and a menu on it is a card,
 * and that is another widget.
 *
 * Four things worth knowing, all of them answers to questions the grid asks:
 *
 * - **A tile may take more than one cell.** \c setItemSpan gives a picture two
 *   columns and two rows, which is what a grid of photographs calls "this one
 *   is the subject". The list *packs* rather than wraps: it walks the cells in
 *   reading order and puts each picture in the first place its span fits. A
 *   wrapping row would leave the hole under a two-row picture empty unless the
 *   application ordered its pictures to fit, which is the list's arithmetic and
 *   not the application's.
 * - **The bar goes at whichever edge the picture was given.** At the foot by
 *   default; at the head when \c setItemBarPosition says so - a picture whose
 *   subject is low down wants its name high up.
 * - **The bar is a scrim, not a surface.** It darkens the photograph under it
 *   and lets the words be white, because what is behind the words is a picture
 *   the theme has never seen and cannot promise contrast for. That is the one
 *   place in this family where a colour does not come from the palette, which
 *   is why it is a colour words are read against rather than a surface.
 * - **The list does not scroll.** Its height is the rows it needs, which the
 *   spans decide and never the width, so a page that has more pictures than its
 *   room scrolls and the grid stays a grid.
 *
 * A trailing widget stands at the bar's far end - the star on a photograph, the
 * cross that takes it away - and it is the application's own widget, not an
 * icon this list paints. Its clicks are its own: a press that lands on it is
 * not a press on the picture.
 */
class QX_MATERIAL_EXPORT MaterialImageList : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int columns READ columns WRITE setColumns)
    Q_PROPERTY(int rowHeight READ rowHeight WRITE setRowHeight)
    Q_PROPERTY(int gap READ gap WRITE setGap)
public:
    /*! Which edge of the tile the bar sits on. */
    enum class BarPosition {
        Bottom, /*!< At the foot of the picture: the default. */
        Top     /*!< At its head. */
    };
    Q_ENUM(BarPosition)

    explicit MaterialImageList(QWidget *parent = Q_NULLPTR);
    ~MaterialImageList() override;

    /*! How many pictures the list holds. */
    int count() const;
    QPixmap itemPixmap(int index) const;
    QString itemTitle(int index) const;
    QString itemSubtitle(int index) const;
    /*! The columns picture \a index asks for - which a grid narrower than that
     *  reads as its own width. One unless it was told else. */
    int itemColumnSpan(int index) const;
    /*! The rows it asks for. One unless it was told else. */
    int itemRowSpan(int index) const;
    BarPosition itemBarPosition(int index) const;
    /*! The widget standing at the bar's far end, or null. It is this list's
     *  child while it stands there. */
    QWidget *itemTrailingWidget(int index) const;
    /*!
     * Where picture \a index stands, in this widget's coordinates. Empty for an
     * index that is not there - and also while the grid has no width to divide.
     */
    QRect itemRect(int index) const;

    int columns() const;
    int rowHeight() const;
    /*! The room between two tiles, both across and down. */
    int gap() const;

    /*! The hint the grid answers with: square tiles at the columns asked for. */
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    /*! Append \a pixmap with \a title and \a subtitle under it, and answer
     *  where it landed. */
    int addItem(const QPixmap &pixmap, const QString &title = QString(), const QString &subtitle = QString());

public Q_SLOTS:
    void setItemPixmap(int index, const QPixmap &pixmap);
    void setItemTitle(int index, const QString &title);
    void setItemSubtitle(int index, const QString &subtitle);
    /*! Give picture \a index \a columns cells across and \a rows down. Less
     *  than one is read as one: a picture that takes no cell is not a tile. */
    void setItemSpan(int index, int columns, int rows);
    void setItemBarPosition(int index, BarPosition position);
    /*! Stand \a widget at the bar's far end, in place of any widget already
     *  there - which is handed back to the application, not deleted. Asking for
     *  the bar's place for the new widget is asking for a bar, so a picture that
     *  had no name has an empty one from then on. */
    void setItemTrailingWidget(int index, QWidget *widget);

    void removeItem(int index);
    void clear();

    void setColumns(int columns);
    void setRowHeight(int rowHeight);
    void setGap(int gap);

Q_SIGNALS:
    /*! Picture \a index was taken: pressed, or answered with the keyboard.
     *  Pressing the widget in its bar is that widget's, not the picture's. */
    void itemClicked(int index);
protected:
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    Q_DISABLE_COPY(MaterialImageList)
    QX_DECLARE_PRIVATE(MaterialImageList)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALIMAGELIST_H
