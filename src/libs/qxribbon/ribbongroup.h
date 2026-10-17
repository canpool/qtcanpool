/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbonbutton.h"

#include <QWidget>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonGallery;
class RibbonGroupPrivate;

/**
 * @brief A group is used to group the controls in a ribbon page window
 *
 * Ribbon groups come in two-row and three-row modes. The ribbon asks a group for three rows in the
 * styles without the TwoRow suffix and for two rows in the ones with it, through @ref
 * RibbonBar::setRibbonStyle.
 *
 * Within a group, @ref setExpanding can be used to make the group expand horizontally; if the
 * group has no horizontally expandable controls, whitespace is left, so it is recommended to call
 * this function only when the group contains such a control (e.g. @ref RibbonGallery)
 *
 * The group layout is implemented through @ref RibbonGroupLayout
 */
class QX_RIBBON_EXPORT RibbonGroup : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(bool isExpanding READ isExpanding WRITE setExpanding)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName)
public:
    enum GroupLayoutMode {
        ThreeRowMode,   ///< Three-row layout: a group holds 3 rows of small toolbuttons; the default
        TwoRowMode      ///< Two-row layout: a group holds 2 rows of small toolbuttons; one row shorter
    };
    enum RowProportion {
        Auto,     ///< Undefined proportion; determined from expandingDirections: Qt::Vertical means Large, otherwise
                  ///< Small
        Large,    ///< Large proportion; a widget's height fills the whole group
        Medium,   ///< Medium proportion; only takes effect when @ref RibbonGroup::groupLayoutMode is @ref
                  ///< RibbonGroup::ThreeRowMode, and occupies two of the three rows when two Mediums are in the same
                  ///< column
        Small,    ///< Small proportion; occupies one row of the RibbonGroup; Medium becomes Small when its condition is
                  ///< not met, but never Large
    };
public:
    // make the addAction of QWidget visible in the subclass
    using QWidget::addAction;
public:
    explicit RibbonGroup(QWidget *parent = Q_NULLPTR);
    explicit RibbonGroup(const QString &name, QWidget *parent = Q_NULLPTR);
    ~RibbonGroup() Q_DECL_OVERRIDE;

    QString groupName() const;
    void setGroupName(const QString &title);
    QString title() const;
    void setTitle(const QString &title);

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

    int ribbonButtonCount() const;
    RibbonButton *ribbonButtonByIndex(int index) const;
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

    bool isReduced() const;
    void setReduced(bool reduced = true);

    bool isCanCustomize() const;
    void setCanCustomize(bool b);

    int largeHeight() const;
    int titleHeight() const;

    static bool titleVisible();
    static void setTitleVisible(bool visible);

    static int groupTitleHeight();
    static void setGroupTitleHeight(int h);
Q_SIGNALS:
    void actionTriggered(QAction *action);
protected:
    void setGroupLayoutMode(GroupLayoutMode mode);
    void updateItemGeometry();
protected:
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent *event) Q_DECL_OVERRIDE;
private:
    // B11: switch the presentation without telling the page. The page's own layout
    // and the width measurement both run through here; neither may post a
    // LayoutRequest, or the page would be asked to lay out again while it is
    // laying out.
    void setReducedInternal(bool reduced);
    // B11: the width the group needs in each of its two presentations
    int normalWidth();
    int reducedWidth();
private:
    QX_DECLARE_PRIVATE(RibbonGroup)
    friend class RibbonPage;
    friend class RibbonPagePrivate;
    friend class RibbonCustomizeWidgetPrivate;
};

QX_RIBBON_END_NAMESPACE
