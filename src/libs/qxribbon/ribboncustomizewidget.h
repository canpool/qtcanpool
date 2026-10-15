/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbonactionsmanager.h"
#include "ribboncustomizedata.h"
#include "ribbongroup.h"

#include <QWidget>

class QStandardItem;
class QAbstractButton;
class QXmlStreamWriter;
class QXmlStreamReader;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonCustomizeWidgetUi;
class RibbonCustomizeWidgetPrivate;
class RibbonBar;

/**
 * @brief Customize interface window
 */
class RibbonCustomizeWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Defines the display type of the ribbon tree
     */
    enum RibbonTreeShowType {
        ShowAllPage,   ///< Show all pages, including context pages
        ShowMainPage   ///< Show main pages only, excluding context pages
    };

    /**
     * @brief Role associated with QStandardItem
     */
    enum ItemRole {
        LevelRole = Qt::UserRole + 1,   ///< Indicates the level: 0 for page, 1 for group, 2 for item
        PointerRole,                    ///< Stores a pointer, cast according to LevelRole
        CanCustomizeRole,               ///< Indicates whether the item can be customized (bool)
        CustomizeRole,   ///< Indicates a customized item (bool), mainly for user-added tabs and groups; this role
                         ///< implies CanCustomizeRole
        CustomizeObjNameRole,   ///< Records the obj name of temporary customized content (QString)
    };
public:
    RibbonCustomizeWidget(RibbonBar *ribbonBar, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~RibbonCustomizeWidget();

    // Sets up the action manager
    void setupActionsManager(RibbonActionsManager *mgr);

    // Updates the model according to the current radio button option
    void updateModel();

    // Updates the model
    void updateModel(RibbonTreeShowType type);

    // Applies all settings
    bool apply();

    // Converts to xml
    bool toXml(QXmlStreamWriter *xml) const;
    bool toXml(const QString &xmlpath) const;

    // Loads QList<RibbonCustomizeData> from xml; for file-based settings, call this before showing the dialog to ensure
    // correct recording of stacked settings
    void fromXml(QXmlStreamReader *xml);
    void fromXml(const QString &xmlpath);

    // Applies xml configuration; can be combined with customize_datas_from_xml and customize_datas_apply
    static bool fromXml(QXmlStreamReader *xml, RibbonBar *bar, RibbonActionsManager *mgr);

    // Clears all actions; after calling apply(), call clear() before continuing, otherwise an exception may occur
    void clear();
protected:
    // Trims QList<RibbonCustomizeData> and merges some actions
    void simplify();

    RibbonGroup::RowProportion selectedRowProportion() const;

    QAction *selectedAction() const;
    QAction *itemToAction(QStandardItem *item) const;

    QStandardItem *selectedItem() const;

    // Gets the level of the selected ribbon tree
    int selectedRibbonLevel() const;

    // Determines based on the selected item
    int itemLevel(QStandardItem *item) const;

    // Selects the specified item
    void setSelectItem(QStandardItem *item, bool ensureVisible = true);

    // Checks whether the item can be modified; returns true if it can
    bool isItemCanCustomize(QStandardItem *item) const;
    bool isSelectedItemCanCustomize() const;

    // Checks whether the item is a customized item
    bool isCustomizeItem(QStandardItem *item) const;
    bool isSelectedItemIsCustomize() const;

    // Removes an item
    void removeItem(QStandardItem *item);

    // Moves the selected item up/down; delta of -1 moves up and 1 moves down
    void moveSelectedItem(int delta);

private Q_SLOTS:
    void onComboBoxActionIndexCurrentIndexChanged(int index);
    void onRadioButtonGroupButtonClicked(QAbstractButton *b);
    void onPushButtonNewPageClicked();
    void onPushButtonNewGroupClicked();
    void onPushButtonRenameClicked();
    void onPushButtonAddClicked();
    void onPushButtonDeleteClicked();
    void onListViewSelectClicked(const QModelIndex &index);
    void onTreeViewResultClicked(const QModelIndex &index);
    void onToolButtonUpClicked();
    void onToolButtonDownClicked();
    void onItemChanged(QStandardItem *item);
    void onLineEditSearchActionTextEdited(const QString &text);
    void onPushButtonResetClicked();
private:
    void initConnection();
private:
    RibbonCustomizeWidgetUi *ui;
    RibbonCustomizeWidgetPrivate *d;
};

QX_RIBBON_END_NAMESPACE
