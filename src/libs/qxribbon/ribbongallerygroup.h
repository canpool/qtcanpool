/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QListView>

class QActionGroup;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonGalleryGroupPrivate;

///
/// \brief Group of the Gallery
///
/// The group is responsible for displaying and managing Gallery Items
///
class QX_RIBBON_EXPORT RibbonGalleryGroup : public QListView
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

    void setGroupTitle(const QString &title);
    QString groupTitle() const;

    void selectByIndex(int i);

    /// Activates the current item exactly as a mouse click would; used by the keyboard
    /// navigation of RibbonGallery (B9).
    void triggerCurrentItem();

    void setDisplayRow(DisplayRow r);
    DisplayRow displayRow() const;

    void setGridMinimumWidth(int w);
    int gridMinimumWidth() const;

    void setGridMaximumWidth(int w);
    int gridMaximumWidth() const;

    QSize sizeHint() const override;

    QActionGroup *actionGroup() const;
private Q_SLOTS:
    void onItemClicked(const QModelIndex &index);
    void onItemEntered(const QModelIndex &index);
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
     * @note This property is activated via QAbstractItemView::entered(const QModelIndex
     * &index), so make sure setMouseTracking(true) is set
     * @param action
     */
    void hovered(QAction *action);
private:
    QX_DECLARE_PRIVATE(RibbonGalleryGroup)
};

QX_RIBBON_END_NAMESPACE
