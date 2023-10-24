/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxglobal.h"
#include "ribbonbutton.h"

#include <QWidget>

QX_BEGIN_NAMESPACE

class RibbonGallery;
class RibbonGroupPrivate;

/**
 * @brief group 用于对 ribbon 页窗口中的控件进行分组
 *
 * ribbon 的 group 分为两行模式和三行模式，以 office 为代表的 ribbon 为 3 行模式，以 WPS 为代表的“紧凑派”就是 2 行模式，
 * Ribbon 可通过 RibbonBar 的 @ref RibbonBar::RibbonStyle 来指定模式 (通过函数 @ref RibbonBar::setRibbonStyle )
 *
 * 在 group 中，可以通过 @ref setExpanding 函数指定 group 水平扩展，如果 group 里面没有能水平扩展的控件，将会留白，
 * 因此，建议在 group 里面有水平扩展的控件如（ @ref RibbonGallery ）才指定这个函数
 *
 * group 的布局通过 @ref RibbonGroupLayout 来实现
 */
class QX_WIDGET_EXPORT RibbonGroup : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(bool isExpanding READ isExpanding WRITE setExpanding)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName)
public:
    enum GroupLayoutMode {
        ThreeRowMode,   ///< 三行布局模式，office就是三行布局模式，group能布置3行小toolbutton，默认模式
        TwoRowMode      ///< 两行布局模式，wps的后续布局模式就是两行布局模式，group能布置2行小toolbutton
    };
    enum RowProportion {
        Auto,   ///< 未定义占比，这时候将会依据expandingDirections来判断，如果能有Qt::Vertical，就等同于Large，否则就是Small
        Large,  ///< 大占比，一个widget的高度会充满整个group
        Medium, ///< 中占比，在 @ref RibbonGroup::groupLayoutMode 为 @ref RibbonGroup::ThreeRowMode 时才会起作用，
                ///< 且要同一列里两个都是 Medium 时，会在三行中占据两行
        Small,  ///< 小占比，占RibbonGroup的一行，Medium在不满足条件时也会变为Small，但不会变为Large
    };
public:
    explicit RibbonGroup(QWidget *parent = Q_NULLPTR);
    explicit RibbonGroup(const QString &name, QWidget *parent = Q_NULLPTR);
    ~RibbonGroup() Q_DECL_OVERRIDE;

    QString groupName() const;
    void setGroupName(const QString &title);

    static RibbonGroup::RowProportion getActionRowProportionProperty(QAction *action);
    static void setActionRowProportionProperty(QAction *action, RibbonGroup::RowProportion rp);

    void setActionRowProportion(QAction *action, RibbonGroup::RowProportion rp);

    RibbonButton *addAction(QAction *action, RibbonGroup::RowProportion rp);
    RibbonButton *addLargeAction(QAction *action);
    RibbonButton *addMediumAction(QAction *action);
    RibbonButton *addSmallAction(QAction *action);

    void addAction(QAction *act, QToolButton::ToolButtonPopupMode popMode,
                   RibbonGroup::RowProportion rp = RibbonGroup::Large);
    QAction *addAction(const QString &text, const QIcon &icon, QToolButton::ToolButtonPopupMode popMode,
                       RibbonGroup::RowProportion rp = RibbonGroup::Large);

    RibbonButton *addMenu(QMenu *menu, RibbonGroup::RowProportion rp,
                          QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    RibbonButton *addLargeMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    RibbonButton *addSmallMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    RibbonButton *addActionMenu(QAction *action, QMenu *menu, RibbonGroup::RowProportion rp);
    RibbonButton *addLargeActionMenu(QAction *action, QMenu *menu);

    QAction *addWidget(QWidget *w, RibbonGroup::RowProportion rp);
    QAction *addLargeWidget(QWidget *w);
    QAction *addMediumWidget(QWidget *w);
    QAction *addSmallWidget(QWidget *w);

    RibbonGallery *addGallery();

    QAction *addSeparator(int top = 6, int bottom = 6);

    RibbonButton *ribbonButtonForAction(QAction *action) const;
    QList<RibbonButton *> ribbonButtons() const;

    bool hasOptionAction() const;
    void addOptionAction(QAction *action);

    int actionIndex(QAction *act) const;
    void moveAction(int from, int to);

    GroupLayoutMode groupLayoutMode() const;

    bool isTwoRow() const;

    virtual QSize optionActionButtonSize() const;
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    bool isExpanding() const;
    void setExpanding(bool expanding = true);

    bool isCanCustomize() const;
    void setCanCustomize(bool b);

    int largeHeight() const;
    int titleHeight() const;

    static bool titleVisible();
    static void setTitleVisible(bool visible);

    static int groupTitleHeight();
    static void setGroupTitleHeight(int h);
signals:
    void actionTriggered(QAction *action);
protected:
    void setGroupLayoutMode(GroupLayoutMode mode);
    void updateItemGeometry();
protected:
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent *event) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(RibbonGroup)
    friend class RibbonPage;
    friend class RibbonPagePrivate;
    friend class RibbonCustomizeWidgetPrivate;
};

QX_END_NAMESPACE
