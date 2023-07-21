/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include "ribboncustomizepage.h"
#include "ui_ribbonbarcustomizepage.h"
#include "ui_ribbonquickaccessbarcustomizepage.h"
#include <QDialogButtonBox>
#include <QLayout>
#include <QListWidget>
#include <QStyledItemDelegate>

QRIBBON_BEGIN_NAMESPACE

class RibbonCustomizePageWidget;
/* RibbonCustomizePageItem */
class RibbonCustomizePageItem : public QListWidgetItem
{
public:
    RibbonCustomizePageWidget *m_page;
public:
    explicit RibbonCustomizePageItem(const QString &text, RibbonCustomizePageWidget *page)
        : QListWidgetItem(text), m_page(page)
    {
    }
};

/* RibbonCustomizeListWidget */
class RibbonCustomizeListWidget : public QListWidget
{
public:
    explicit RibbonCustomizeListWidget(QWidget *parent = Q_NULL);
    virtual ~RibbonCustomizeListWidget();
public:
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
};

/* RibbonCustomizeDialogPrivate */
class RibbonCustomizeDialogPrivate : public QObject
{
    Q_OBJECT
public:
    QTC_DECLARE_PUBLIC(RibbonCustomizeDialog)
public:
    explicit RibbonCustomizeDialogPrivate();
    virtual ~RibbonCustomizeDialogPrivate();
public:
    void init();
    void setCustomizeMode(bool edit);
    void addPage(RibbonCustomizePageWidget *page);
    void insertPage(int index, RibbonCustomizePageWidget *page);

public Q_SLOTS:
    void switchToPage(int currentRow);
public:
    QList<QWidget *> m_listWidget;
    QVBoxLayout *m_verticalLayout;
    QHBoxLayout *m_horizontalLayout;
    QListWidget *m_listPage;
    QFrame *m_pageFrame;
    QDialogButtonBox *m_buttonBox;

    int m_currentPage;
    QVBoxLayout *m_pageVBoxLayout;
private:
    Q_DISABLE_COPY(RibbonCustomizeDialogPrivate)
};

/* RibbonCustomizeStyledItemDelegate */
class RibbonCustomizeStyledItemDelegate : public QStyledItemDelegate
{
public:
    explicit RibbonCustomizeStyledItemDelegate(QListWidget *listWidget);
    virtual ~RibbonCustomizeStyledItemDelegate();
public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
protected:
    QListWidget *m_listWidget;
private:
    Q_DISABLE_COPY(RibbonCustomizeStyledItemDelegate)
};

class RibbonBar;
class RibbonPage;
/* RibbonQuickAccessBarCustomizePagePrivate */
class RibbonQuickAccessBarCustomizePagePrivate : public QObject
{
    Q_OBJECT
public:
    QTC_DECLARE_PUBLIC(RibbonQuickAccessBarCustomizePage)
    enum ColumnCommand {
        nIconCommand = 0,
        nNameCommand,
        nViewCommand
    };
public:
    explicit RibbonQuickAccessBarCustomizePagePrivate();
    virtual ~RibbonQuickAccessBarCustomizePagePrivate();
public:
    void init();
    void setupPage();
    void initListWidgetCommands();
    QIcon createIconStyleWidget(QWidget *widget);
    QPixmap createIconExStyleWidget(QWidget *widget, bool &iconView);

    void fillActions();
    void fillStateCommands();
    void fillListCategorieCommads(const QList<QAction *> &actions);

    void setButtons();

    void applyClicked();

public Q_SLOTS:
    void setCurrentCategoryText(const QString &);
    void currentCommandChanged(QListWidgetItem *current);
    void currentQTCommandChanged(QListWidgetItem *current);

    void addClicked();
    void removeClicked();
    void resetClicked();
    void upClicked();
    void downClicked();
public:
    Ui::RibbonQuickAccessBarCustomizePage m_pageUI;
    RibbonBar *m_ribbonBar;

    QStringList m_sourceCategories;
    QAction *m_separator;
    QString m_separatorText;
    QString m_strSeparator;
    QString m_currentSourceCategory;

    QListWidgetItem *m_currentAction;
    QMap<QAction *, QListWidgetItem *> m_actionToItem;
    QMap<QListWidgetItem *, QAction *> m_itemToAction;

    QMap<QAction *, QListWidgetItem *> m_actionToCurrentItem;
    QMap<QListWidgetItem *, QAction *> m_currentItemToAction;

    int m_heightRowItem;
    int m_widthColIconItem;
    int m_widthColViewItem;
    bool m_wasDisplayed;
private:
    Q_DISABLE_COPY(RibbonQuickAccessBarCustomizePagePrivate)
};

class RibbonGroup;
/* RibbonBarCustomizePagePrivate */
class RibbonBarCustomizePagePrivate : public QObject
{
    Q_OBJECT
public:
    QTC_DECLARE_PUBLIC(RibbonBarCustomizePage)
public:
    explicit RibbonBarCustomizePagePrivate();
    virtual ~RibbonBarCustomizePagePrivate();
public:
    void init();
    void setupPage();

    void setCategoryCaptionTree(QTreeWidget *tree, const QString &strCategory);
    void clearCategoryCaptionTree(QTreeWidget *tree);
    void fillSourceCategories();
    void fillSourceActions(QList<QAction *> &actions, QTreeWidgetItem *itemParent = Q_NULL);
    void fillSourcePages(QList<RibbonPage *> &pages, const QString &strCategory);
    void fillSourceGroups(RibbonPage *page, QTreeWidgetItem *itemParent);
    void fillSourceGroupActions(RibbonGroup *group, QTreeWidgetItem *itemParent);

    void fillStateCategories();
    void fillStateRibbon(const QString &strCategory);
    void fillStateGroups(RibbonPage *page, QTreeWidgetItem *itemParent);
    void fillStateActions(RibbonGroup *group, QTreeWidgetItem *itemParent, bool newGroup);

    RibbonPage *addPageCustom(RibbonPage *srcPage);
    void addGroupCopy(RibbonPage *srcPage, RibbonPage *copyPage);
    void addActionGroupCustom(RibbonGroup *group, QAction *action, int index);
    void setButtons();

    void applyClicked();
    void cancelClicked();

public Q_SLOTS:
    void addClicked();
    void newTabClicked();
    void newGroupClicked();
    void removeClicked();
    void resetClicked();
    void upClicked();
    void downClicked();
    void renameClicked();
    void setCurrentCategoryText(const QString &);
    void setCurrentTabsText(const QString &);

    void currentSourceChanged(QTreeWidgetItem *itemPage);

    void currentPageChanged(QTreeWidgetItem *itemPage);
    void itemPageChanged(QTreeWidgetItem *itemPage, int);
public:
    RibbonBar *m_ribbonBar;
    Ui::RibbonBarCustomizePage m_pageUI;

    QString m_strSeparator;
    QString m_currentSourceCategory;

    QStringList m_sourceCategories;
    QTreeWidgetItem *m_currentSourceItemPage;
    QTreeWidgetItem *m_currentSourceItemGroup;
    QTreeWidgetItem *m_currentSourceAction;
    QMap<QTreeWidgetItem *, RibbonPage *> m_currentSourceItemToPage;
    QMap<QTreeWidgetItem *, RibbonGroup *> m_currentSourceItemToGroup;
    QMap<QTreeWidgetItem *, QAction *> m_currentSourceItemToAction;

    QTreeWidgetItem *m_currentItemPage;
    QTreeWidgetItem *m_currentItemGroup;
    QTreeWidgetItem *m_currentItemAction;
    QMap<RibbonPage *, QTreeWidgetItem *> m_pageToCurrentItem;
    QMap<QTreeWidgetItem *, RibbonPage *> m_currentItemToPage;

    QMap<QAction *, QTreeWidgetItem *> m_actionToCurrentItem;
    QMap<QTreeWidgetItem *, QAction *> m_currentItemToAction;
    QMap<QTreeWidgetItem *, RibbonGroup *> m_currentItemToGroup;

    QString m_sufNameCustom;
    bool m_wasDisplayed;
private:
    Q_DISABLE_COPY(RibbonBarCustomizePagePrivate)
};

QRIBBON_END_NAMESPACE
