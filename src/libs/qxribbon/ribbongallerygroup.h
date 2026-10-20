/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"

#include <QIcon>
#include <QWidget>

class QAction;
class QActionGroup;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonGalleryGroupPrivate;

///
/// \brief Group of the Gallery
///
/// The group holds the items a gallery offers and paints them itself. It is the
/// data a gallery scrolls through and the widget a gallery pops up, and both
/// uses share one set of metrics: an item's rectangle never depends on what is
/// being done to it, so pointing at one cannot move it.
///
/// The group paints from the palette - QPalette::Highlight for the emphasis and
/// QPalette::Text for the labels - and leaves its own background alone, so a
/// theme is followed without a style sheet having to describe the item states.
/// A state changes the wash and the border; it never changes the geometry, and
/// it never reverses the text out of its own colour.
///
class QX_RIBBON_EXPORT RibbonGalleryGroup : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief The display style of the GalleryGroup
     */
    enum GalleryGroupStyle {
        IconWithText,           ///< icon with text
        IconWithWordWrapText,   ///< icon with text, the text wraps; this mode only takes effect for DisplayOneRow,
                                ///< otherwise it is equivalent to IconWithText
        IconOnly                ///< icon only
    };

    /**
     * @brief Defines the number of icon rows the Gallery displays under a group
     *
     * The rows share the height the strip already has instead of asking for more:
     * the grid is (height - (rows + 1) * spacing) / rows tall, so an item in a
     * three-row group is about a third of the height of an item in a one-row
     * group. Icon-only groups are what this suits; a group with text needs a strip
     * tall enough to give every row a readable height, and the strip a ribbon
     * gives is about one row deep (see RibbonGallery's size hint).
     */
    enum DisplayRow {
        DisplayOneRow = 1,   ///< display 1 row, default
        DisplayTwoRow = 2,
        DisplayThreeRow = 3
    };
public:
    explicit RibbonGalleryGroup(QWidget *w = Q_NULLPTR);
    virtual ~RibbonGalleryGroup();

    void setRecalcGridSizeBlock(bool on = true);
    bool isRecalcGridSizeBlock() const;
    void recalcGridSize();
    void recalcGridSize(int galleryHeight);

    void setGalleryGroupStyle(GalleryGroupStyle style);
    GalleryGroupStyle galleryGroupStyle() const;

    void addItem(const QString &text, const QIcon &icon);
    void addItem(QAction *act);
    void addItems(const QList<QAction *> &acts);
    int itemCount() const;
    QAction *itemAction(int index) const;

    void setGroupTitle(const QString &title);
    QString groupTitle() const;

    void selectByIndex(int i);
    int selectedIndex() const;
    QAction *selectedAction() const;

    /// Activates the selected item exactly as a mouse click would; used by the keyboard
    /// navigation of RibbonGallery (B9).
    void triggerCurrentItem();

    void setDisplayRow(DisplayRow r);
    DisplayRow displayRow() const;

    void setGridMinimumWidth(int w);
    int gridMinimumWidth() const;

    void setGridMaximumWidth(int w);
    int gridMaximumWidth() const;

    void setSpacing(int spacing);
    int spacing() const;

    QSize gridSize() const;
    int rowCount() const;
    int columnCount() const;
    QRect itemRect(int index) const;
    int itemAt(const QPoint &pos) const;

    /// How many rows the group shows at once. Zero - the default, and what a group
    /// standing on its own wants - shows every row and never scrolls; a gallery sets
    /// it to the number of rows that fit and scrolls the rest through.
    void setViewportRows(int rows);
    int viewportRows() const;
    int scrollRow() const;
    void setScrollRow(int row);
    void scrollByRows(int rows);
    void scrollToItem(int index);
    int maxScrollRow() const;

    QSize sizeHint() const override;

    QActionGroup *actionGroup() const;
Q_SIGNALS:
    void groupTitleChanged(const QString &title);
    /**
     * @brief Equivalent to QActionGroup's triggered
     * All actions added to the RibbonGalleryGroup are managed by a QActionGroup; the corresponding action group can be
     * obtained via @sa actionGroup
     * @param action
     */
    void triggered(QAction *action);
    /**
     * @brief Equivalent to QActionGroup's hovered
     * All actions added to the RibbonGalleryGroup are managed by a QActionGroup; the corresponding action group can be
     * obtained via @sa actionGroup
     * @param action
     */
    void hovered(QAction *action);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    QX_DECLARE_PRIVATE(RibbonGalleryGroup)
};

QX_RIBBON_END_NAMESPACE
