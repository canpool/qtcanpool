/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbongallerygroup.h"

#include <QFrame>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonGalleryPrivate;

/**
 * @brief Gallery widget
 *
 * The Gallery widget consists of a currently active @sa RibbonGalleryGroup and a popup @sa RibbonGalleryViewport
 *
 * @sa RibbonGalleryGroup inherits @sa QListView and displays actions as icons; its related properties can be set as for
 * a QListView
 *
 * @sa RibbonGalleryViewport is a widget with an internal vertical layout; when the popup is activated, it displays all
 * managed RibbonGalleryGroups
 *
 * Example as follows:
 * @code
 * RibbonGallery *gallery = ribbonGroup1->addGallery();
 * QList< QAction* > galleryActions;
 * ...create many actions ...
 * RibbonGalleryGroup *group1 = gallery->addCategoryActions(tr("Files"), galleryActions);
 * galleryActions.clear();
 * ...create many actions ...
 * gallery->addCategoryActions(tr("Apps"), galleryActions);
 * gallery->setCurrentViewGroup(group1);
 * @endcode
 */
class QX_RIBBON_EXPORT RibbonGallery : public QFrame
{
    Q_OBJECT
public:
    RibbonGallery(QWidget *parent = Q_NULLPTR);
    virtual ~RibbonGallery();

    RibbonGalleryGroup *addGalleryGroup();
    RibbonGalleryGroup *addGalleryGroup(const QString &title, QList<QAction *> actions);
    void addGalleryGroup(RibbonGalleryGroup *group);
    RibbonGalleryGroup *currentViewGroup() const;
    void setCurrentViewGroup(RibbonGalleryGroup *group);

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    static void setGalleryButtonMaximumWidth(int w = 15);
Q_SIGNALS:
    void triggered(QAction *action);
    void hovered(QAction *action);
public Q_SLOTS:
    virtual void pageUp();
    virtual void pageDown();
    virtual void showMoreDetail();
protected Q_SLOTS:
    void onItemClicked(const QModelIndex &index);
    virtual void onTriggered(QAction *action);
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
private:
    void moveCurrentItem(int step);
private:
    QX_DECLARE_PRIVATE(RibbonGallery)
};

QX_RIBBON_END_NAMESPACE
