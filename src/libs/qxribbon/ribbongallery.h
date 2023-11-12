/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"
#include "ribbongallerygroup.h"

#include <QFrame>
#include <QSizeGrip>

class QLabel;
class QVBoxLayout;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonGalleryPrivate;
class RibbonGalleryViewport;

/**
 * @brief Gallery 控件
 *
 * Gallery 控件是由一个当前激活的 @sa RibbonGalleryGroup 和弹出的 @sa RibbonGalleryViewport 组成
 *
 * @sa RibbonGalleryGroup 是继承 @sa QListView actions 通过 icon 展示出来，相关的属性可以按照 QListView 设置
 *
 * @sa RibbonGalleryViewport 是一个内部有垂直布局的窗体，在弹出激活时，把管理的 RibbonGalleryGroup 都展示出来
 *
 * 示例如下：
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

    RibbonGalleryViewport *getPopupViewPort() const;
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    static void setGalleryButtonMaximumWidth(int w = 15);
signals:
    void triggered(QAction *action);
    void hovered(QAction *action);
public slots:
    virtual void pageUp();
    virtual void pageDown();
    virtual void showMoreDetail();
protected slots:
    void onItemClicked(const QModelIndex &index);
    virtual void onTriggered(QAction *action);
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    RibbonGalleryViewport *getPopupViewPort();
private:
    QX_DECLARE_PRIVATE(RibbonGallery)
};

class RibbonGalleryViewportPrivate;
/* RibbonGalleryViewport */
class QX_RIBBON_EXPORT RibbonGalleryViewport : public QWidget
{
    Q_OBJECT
public:
    RibbonGalleryViewport(QWidget *parent);
    ~RibbonGalleryViewport();
public:
    void addWidget(QWidget *w);
    void addWidget(QWidget *w, const QString &title);
    void removeWidget(QWidget *w);
    QLabel *getWidgetTitleLabel(QWidget *w);
public slots:
    void widgetTitleChanged(QWidget *w, const QString &title);
private:
    QX_DECLARE_PRIVATE(RibbonGalleryViewport)
};

QX_RIBBON_END_NAMESPACE
