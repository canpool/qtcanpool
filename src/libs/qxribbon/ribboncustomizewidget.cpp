/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribboncustomizewidget.h"
#include "ribboncustomizewidget_p.h"
#include "ribbonactionsmanager_p.h"
#include "ribbonbar.h"
#include "ribboncustomizedata.h"
#include "ribbonmainwindow.h"
#include "ribbongroup.h"
#include "ribbongroup_p.h"
#include "ribbongrouplayout.h"

#include <QAction>
#include <QButtonGroup>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QX_RIBBON_BEGIN_NAMESPACE

bool QxRibbonCustomizeDataSetToXml(QXmlStreamWriter *xml, const QList<RibbonCustomizeData> &cds, bool force = false)
{
    // If the xml previously recorded customization data that added a page, and that page is later removed,
    // after simplify the cds are cancelled out to empty. In that case, if we skip writing because cds is empty,
    // the xml would keep a stale add entry forever, so a force parameter is added: when cds is empty but the old
    // data is not, the write must be forced
    if (cds.size() == 0 && !force) {
        return false;
    }

    xml->writeStartElement("qx-ribbon-customize");
    for (const RibbonCustomizeData &d : cds) {
        xml->writeStartElement("customize-data");
        xml->writeAttribute("type", QString::number(d.actionType()));
        xml->writeAttribute("index", QString::number(d.indexValue));
        xml->writeAttribute("key", d.keyValue);
        xml->writeAttribute("page", d.pageObjNameValue);
        xml->writeAttribute("group", d.groupObjNameValue);
        xml->writeAttribute("row-prop", QString::number(d.actionRowProportionValue));

        xml->writeEndElement();
    }
    xml->writeEndElement();
    if (xml->hasError()) {
        qWarning() << "write has error";
    }
    return true;
}

QList<RibbonCustomizeData> QxRibbonCustomizeDataGetFromXml(QXmlStreamReader *xml, RibbonActionsManager *mgr)
{
    // First locate "qx-ribbon-customize"
    while (!xml->atEnd()) {
#ifdef QX_RIBBON_DEBUG
        qDebug() << "name:" << xml->name() << " qualifiedName:" << xml->qualifiedName();
#endif
        if (xml->isStartElement() && (xml->name().toString() == "qx-ribbon-customize")) {
            break;
        }
        xml->readNext();
    }
    QList<RibbonCustomizeData> res;

    // Start traversing "customize-data"
    while (!xml->atEnd()) {
        if (xml->isStartElement() && (xml->name().toString() == "customize-data")) {
            // First read the type attribute
            RibbonCustomizeData data;
            QXmlStreamAttributes attrs = xml->attributes();
            if (!attrs.hasAttribute("type")) {
                // Unexpected; skip this one
                xml->readNextStartElement();
                continue;
            }
            bool isOk = false;
            int v = xml->attributes().value("type").toInt(&isOk);
            if (!isOk) {
                // Unexpected; skip this one
                xml->readNextStartElement();
                continue;
            }
            data.setActionType(static_cast<RibbonCustomizeData::ActionType>(v));
            // Start reading the child fields
            if (attrs.hasAttribute("index")) {
                v = xml->attributes().value("index").toInt(&isOk);
                if (isOk) {
                    data.indexValue = v;
                }
            }
            if (attrs.hasAttribute("key")) {
                data.keyValue = attrs.value("key").toString();
            }
            if (attrs.hasAttribute("page")) {
                data.pageObjNameValue = attrs.value("page").toString();
            }
            if (attrs.hasAttribute("group")) {
                data.groupObjNameValue = attrs.value("group").toString();
            }
            if (attrs.hasAttribute("row-prop")) {
                v = xml->attributes().value("row-prop").toInt(&isOk);
                if (isOk) {
                    data.actionRowProportionValue = static_cast<RibbonGroup::RowProportion>(v);
                }
            }
            data.setActionsManager(mgr);
            res.append(data);
        }
        xml->readNext();
    }
    if (xml->hasError()) {
        qWarning() << xml->errorString();
    }
    return res;
}

int QxRibbonCustomizeDataApply(QList<RibbonCustomizeData> &cds, RibbonBar *bar)
{
    int c = 0;

    for (RibbonCustomizeData &d : cds) {
        if (d.apply(bar)) {
            ++c;
        }
    }
    return c;
}

/**
 * @brief Builds the Ui of RibbonCustomizeWidget
 */
class RibbonCustomizeWidgetUi
{
public:
    QHBoxLayout *horizontalLayoutMain;
    QVBoxLayout *verticalLayoutSelect;
    QLabel *labelSelectAction;
    QHBoxLayout *horizontalLayoutSearch;
    QComboBox *comboBoxActionIndex;
    QLineEdit *lineEditSearchAction;
    QListView *listViewSelect;
    QVBoxLayout *verticalLayoutMidButtons;
    QSpacerItem *verticalSpacerUp;
    QPushButton *pushButtonAdd;
    QPushButton *pushButtonDelete;
    QPushButton *pushButtonReset;
    QSpacerItem *verticalSpacerDown;
    QLabel *labelProportion;
    QComboBox *comboBoxActionProportion;
    QVBoxLayout *verticalLayoutResult;
    QLabel *labelCustomize;
    QHBoxLayout *horizontalLayoutPageSelect;
    QRadioButton *radioButtonMainPage;
    QRadioButton *radioButtonAllPage;
    QButtonGroup *radioButtonGroup;
    QTreeView *treeViewResult;
    QHBoxLayout *horizontalLayoutActionOptBtns;
    QPushButton *pushButtonNewPage;
    QPushButton *pushButtonNewGroup;
    QPushButton *pushButtonRename;
    QVBoxLayout *verticalLayoutRightButtons;
    QSpacerItem *verticalSpacerUp2;
    QToolButton *toolButtonUp;
    QToolButton *toolButtonDown;
    QSpacerItem *verticalSpacerDown2;

    void setupUi(QWidget *customizeWidget)
    {
        if (customizeWidget->objectName().isEmpty()) {
            customizeWidget->setObjectName(QStringLiteral("RibbonCustomizeWidget"));
        }
        customizeWidget->resize(800, 600);
        horizontalLayoutMain = new QHBoxLayout(customizeWidget);
        horizontalLayoutMain->setContentsMargins(0, 0, 0, 0);
        horizontalLayoutMain->setObjectName(QStringLiteral("horizontalLayoutMain"));
        verticalLayoutSelect = new QVBoxLayout();
        verticalLayoutSelect->setObjectName(QStringLiteral("verticalLayoutSelect"));
        labelSelectAction = new QLabel(customizeWidget);
        labelSelectAction->setObjectName(QStringLiteral("labelSelectAction"));

        verticalLayoutSelect->addWidget(labelSelectAction);

        horizontalLayoutSearch = new QHBoxLayout();
        horizontalLayoutSearch->setObjectName(QStringLiteral("horizontalLayoutSearch"));
        comboBoxActionIndex = new QComboBox(customizeWidget);
        comboBoxActionIndex->setObjectName(QStringLiteral("comboBoxActionIndex"));
        comboBoxActionIndex->setEditable(false);

        horizontalLayoutSearch->addWidget(comboBoxActionIndex);

        lineEditSearchAction = new QLineEdit(customizeWidget);
        lineEditSearchAction->setObjectName(QStringLiteral("lineEditSearchAction"));

        horizontalLayoutSearch->addWidget(lineEditSearchAction);

        verticalLayoutSelect->addLayout(horizontalLayoutSearch);

        listViewSelect = new QListView(customizeWidget);
        listViewSelect->setObjectName(QStringLiteral("listViewSelect"));

        verticalLayoutSelect->addWidget(listViewSelect);

        horizontalLayoutMain->addLayout(verticalLayoutSelect);

        verticalLayoutMidButtons = new QVBoxLayout();
        verticalLayoutMidButtons->setObjectName(QStringLiteral("verticalLayoutMidButtons"));
        verticalSpacerUp = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutMidButtons->addItem(verticalSpacerUp);

        pushButtonAdd = new QPushButton(customizeWidget);
        pushButtonAdd->setObjectName(QStringLiteral("pushButtonAdd"));
        pushButtonAdd->setEnabled(false);

        verticalLayoutMidButtons->addWidget(pushButtonAdd);

        pushButtonDelete = new QPushButton(customizeWidget);
        pushButtonDelete->setObjectName(QStringLiteral("pushButtonDelete"));
        pushButtonDelete->setEnabled(false);

        verticalLayoutMidButtons->addWidget(pushButtonDelete);

        verticalSpacerDown = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutMidButtons->addItem(verticalSpacerDown);

        labelProportion = new QLabel(customizeWidget);
        labelProportion->setObjectName(QStringLiteral("labelProportion"));
        verticalLayoutMidButtons->addWidget(labelProportion);

        comboBoxActionProportion = new QComboBox(customizeWidget);
        comboBoxActionProportion->setObjectName(QStringLiteral("comboBoxActionProportion"));
        comboBoxActionProportion->setEditable(false);
        verticalLayoutMidButtons->addWidget(comboBoxActionProportion);
        horizontalLayoutMain->addLayout(verticalLayoutMidButtons);

        verticalLayoutResult = new QVBoxLayout();
        verticalLayoutResult->setObjectName(QStringLiteral("verticalLayoutResult"));
        labelCustomize = new QLabel(customizeWidget);
        labelCustomize->setObjectName(QStringLiteral("labelCustomize"));

        verticalLayoutResult->addWidget(labelCustomize);

        horizontalLayoutPageSelect = new QHBoxLayout();
        horizontalLayoutPageSelect->setObjectName(QStringLiteral("horizontalLayoutPageSelect"));
        radioButtonMainPage = new QRadioButton(customizeWidget);
        radioButtonMainPage->setObjectName(QStringLiteral("radioButtonMainPage"));
        radioButtonMainPage->setChecked(false);

        horizontalLayoutPageSelect->addWidget(radioButtonMainPage);

        radioButtonAllPage = new QRadioButton(customizeWidget);
        radioButtonAllPage->setObjectName(QStringLiteral("radioButtonAllPage"));
        radioButtonAllPage->setChecked(true);

        horizontalLayoutPageSelect->addWidget(radioButtonAllPage);

        radioButtonGroup = new QButtonGroup(customizeWidget);
        radioButtonGroup->addButton(radioButtonMainPage);
        radioButtonGroup->addButton(radioButtonAllPage);

        verticalLayoutResult->addLayout(horizontalLayoutPageSelect);

        treeViewResult = new QTreeView(customizeWidget);
        treeViewResult->setObjectName(QStringLiteral("treeViewResult"));
        treeViewResult->setHeaderHidden(true);
        treeViewResult->setSelectionMode(QAbstractItemView::SingleSelection);
        treeViewResult->setAnimated(true);                                    // Enables animation
        treeViewResult->setEditTriggers(QAbstractItemView::NoEditTriggers);   // Disallows renaming directly on the item

        verticalLayoutResult->addWidget(treeViewResult);

        horizontalLayoutActionOptBtns = new QHBoxLayout();
        horizontalLayoutActionOptBtns->setObjectName(QStringLiteral("horizontalLayoutActionOptBtns"));
        pushButtonNewPage = new QPushButton(customizeWidget);
        pushButtonNewPage->setObjectName(QStringLiteral("pushButtonNewPage"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonNewPage);

        pushButtonNewGroup = new QPushButton(customizeWidget);
        pushButtonNewGroup->setObjectName(QStringLiteral("pushButtonNewGroup"));
        pushButtonNewGroup->setEnabled(false);

        horizontalLayoutActionOptBtns->addWidget(pushButtonNewGroup);

        pushButtonRename = new QPushButton(customizeWidget);
        pushButtonRename->setObjectName(QStringLiteral("pushButtonRename"));
        pushButtonRename->setEnabled(false);

        horizontalLayoutActionOptBtns->addWidget(pushButtonRename);

        pushButtonReset = new QPushButton(customizeWidget);
        pushButtonReset->setObjectName(QStringLiteral("pushButtonReset"));
        horizontalLayoutActionOptBtns->addWidget(pushButtonReset);

        verticalLayoutResult->addLayout(horizontalLayoutActionOptBtns);

        horizontalLayoutMain->addLayout(verticalLayoutResult);

        verticalLayoutRightButtons = new QVBoxLayout();
        verticalLayoutRightButtons->setObjectName(QStringLiteral("verticalLayoutRightButtons"));
        verticalSpacerUp2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutRightButtons->addItem(verticalSpacerUp2);

        toolButtonUp = new QToolButton(customizeWidget);
        toolButtonUp->setObjectName(QStringLiteral("pushButtonUp"));
        toolButtonUp->setArrowType(Qt::UpArrow);
        toolButtonUp->setAutoRaise(true);
        toolButtonUp->setEnabled(false);

        verticalLayoutRightButtons->addWidget(toolButtonUp);

        toolButtonDown = new QToolButton(customizeWidget);
        toolButtonDown->setObjectName(QStringLiteral("pushButtonDown"));
        toolButtonDown->setArrowType(Qt::DownArrow);
        toolButtonDown->setAutoRaise(true);
        toolButtonDown->setEnabled(false);

        verticalLayoutRightButtons->addWidget(toolButtonDown);

        verticalSpacerDown2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutRightButtons->addItem(verticalSpacerDown2);

        horizontalLayoutMain->addLayout(verticalLayoutRightButtons);

        retranslateUi(customizeWidget);
    }   // setupUi

    void retranslateUi(QWidget *customizeWidget)
    {
        customizeWidget->setWindowTitle(
            QApplication::translate("RibbonCustomizeWidget", "Customize Widget", Q_NULLPTR));
        labelSelectAction->setText(
            QApplication::translate("RibbonCustomizeWidget", "Please Select", Q_NULLPTR));   // cn: please select
        lineEditSearchAction->setInputMask(QString());
        lineEditSearchAction->setText(QString());
        lineEditSearchAction->setPlaceholderText(
            QApplication::translate("RibbonCustomizeWidget", "Find Command", Q_NULLPTR));   // cn: find command
        pushButtonAdd->setText(QApplication::translate("RibbonCustomizeWidget", "Add >>", Q_NULLPTR));   // cn: add >>
        pushButtonDelete->setText(
            QApplication::translate("RibbonCustomizeWidget", "<< Remove", Q_NULLPTR));   // cn: << remove
        labelCustomize->setText(QApplication::translate("RibbonCustomizeWidget", "Customize the Ribbon",
                                                        Q_NULLPTR));   // cn: customize the ribbon
        radioButtonMainPage->setText(
            QApplication::translate("RibbonCustomizeWidget", "Main Page", Q_NULLPTR));   // cn: main page
        radioButtonAllPage->setText(
            QApplication::translate("RibbonCustomizeWidget", "All Page", Q_NULLPTR));   // cn: all pages
        pushButtonNewPage->setText(
            QApplication::translate("RibbonCustomizeWidget", "New Page", Q_NULLPTR));   // cn: new page
        pushButtonNewGroup->setText(
            QApplication::translate("RibbonCustomizeWidget", "New Group", Q_NULLPTR));   // cn: new group
        pushButtonRename->setText(
            QApplication::translate("RibbonCustomizeWidget", "Rename", Q_NULLPTR));                       // cn: rename
        pushButtonReset->setText(QApplication::translate("RibbonCustomizeWidget", "reset", Q_NULLPTR));   // cn: reset
        labelProportion->setText(
            QApplication::translate("RibbonCustomizeWidget", "proportion:", Q_NULLPTR));   // cn: proportion
    }   // retranslateUi
};

/**
 * @brief Manages the business logic of RibbonCustomizeWidget
 */
class RibbonCustomizeWidgetPrivate
{
public:
    RibbonCustomizeWidgetPrivate(RibbonCustomizeWidget *p);

    void updateModel();

    // Creates a random id in the form: pre_QDateTime::currentMSecsSinceEpoch_suf
    static QString makeRandomObjName(const QString &pre);

    int itemLevel(QStandardItem *item) const;

    bool isCustomizeItem(QStandardItem *item) const;

    // Converts the item to a page
    RibbonPage *itemToPage(QStandardItem *item) const;

    // Converts the item to a RibbonGroup
    RibbonGroup *itemToGroup(QStandardItem *item) const;

    // Gets the object name corresponding to the item
    QString itemObjectName(QStandardItem *item) const;

    // Checks whether customization is allowed
    bool isItemCanCustomize(QStandardItem *item) const;

    // Converts the item to an action
    QAction *itemToAction(QStandardItem *item) const;
public:
    RibbonCustomizeWidget *q;
    RibbonCustomizeWidget::RibbonTreeShowType m_showType;   ///< Display type
    RibbonBar *m_ribbonBar;                                 ///< Holds the RibbonBar pointer
    RibbonActionsManager *m_actionMgr;                      ///< Action manager
    RibbonActionsManagerModel *m_acionModel;                ///< Model corresponding to the action manager
    QStandardItemModel *m_ribbonModel;                      ///< Tree used to build the ribbon
    int m_customizePageCount;                               ///< Records the number of customized pages
    int m_customizeGroupCount;                              ///< Records the number of customized groups

    QList<RibbonCustomizeData> m_customizeDatas;      ///< Records all customization actions
    QList<RibbonCustomizeData> m_oldCustomizeDatas;   ///< Records the old customization actions
};

RibbonCustomizeWidgetPrivate::RibbonCustomizeWidgetPrivate(RibbonCustomizeWidget *p)
    : q(p)
    , m_showType(RibbonCustomizeWidget::ShowAllPage)
    , m_ribbonBar(Q_NULLPTR)
    , m_actionMgr(Q_NULLPTR)
    , m_acionModel(new RibbonActionsManagerModel(p))
    , m_ribbonModel(new QStandardItemModel(p))
    , m_customizePageCount(0)
    , m_customizeGroupCount(0)
{
}

void RibbonCustomizeWidgetPrivate::updateModel()
{
    if (m_ribbonBar == Q_NULLPTR) {
        return;
    }
    m_ribbonModel->clear();
    RibbonBar *ribbonbar = m_ribbonBar;
    QList<RibbonPage *> pages = ribbonbar->pages();

    for (RibbonPage *page : std::as_const(pages)) {
        if ((m_showType == RibbonCustomizeWidget::ShowMainPage) && page->isPageContext()) {
            // If only the main content is shown, ignore context tabs
            continue;
        }
        if (page->objectName().isEmpty()) {
            continue;
        }
        QStandardItem *pageSI = new QStandardItem();
        if (page->isPageContext()) {
            pageSI->setText(QString("[%1]").arg(page->windowTitle()));
        } else {
            pageSI->setText(page->windowTitle());
        }
        if (page->isCanCustomize() && !page->isPageContext()) {
            // Context tabs are not handled for show/hide
            pageSI->setCheckable(true);
            pageSI->setCheckState(ribbonbar->isPageVisible(page) ? Qt::Checked : Qt::Unchecked);
            pageSI->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // Marks this as customizable
        }
        pageSI->setData(0, RibbonCustomizeWidget::LevelRole);
        pageSI->setData(QVariant::fromValue<qintptr>(qintptr(page)), RibbonCustomizeWidget::PointerRole);
        QList<RibbonGroup *> groups = page->groupList();
        for (RibbonGroup *grp : std::as_const(groups)) {
            if (grp->objectName().isEmpty()) {
                continue;
            }
            QStandardItem *grpSI = new QStandardItem(grp->windowTitle());
            grpSI->setData(1, RibbonCustomizeWidget::LevelRole);
            grpSI->setData(QVariant::fromValue<qintptr>(qintptr(grp)), RibbonCustomizeWidget::PointerRole);
            if (grp->isCanCustomize()) {
                grpSI->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // Marks this as customizable
            }
            pageSI->appendRow(grpSI);
            const QList<RibbonGroupItem *> &goupItems = grp->d_ptr->ribbonGroupItems();
            for (RibbonGroupItem *grpItem : goupItems) {
                if (grpItem->action->isSeparator()) {
                    continue;
                }
                QStandardItem *stdItem = new QStandardItem();
                if (grpItem->customWidget) {
                    // Custom widget
                    if (grpItem->widget()->windowTitle().isEmpty() /* && grpItem->widget()->windowIcon().isNull()*/) {
                        delete stdItem;
                        continue;   // Skip if the widget has nothing
                    }
                    stdItem->setText(grpItem->widget()->windowTitle());
                    stdItem->setIcon(grpItem->widget()->windowIcon());
                    if (RibbonCustomizeData::isCanCustomize(grpItem->widget())) {
                        stdItem->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // Marks this as customizable
                    }
                } else {
                    // Not customized, so it is an action
                    stdItem->setText(grpItem->action->text());
                    stdItem->setIcon(grpItem->action->icon());
                    if (RibbonCustomizeData::isCanCustomize(grpItem->action)) {
                        stdItem->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // Marks this as customizable
                    }
                }
                stdItem->setData(2, RibbonCustomizeWidget::LevelRole);
                stdItem->setData(QVariant::fromValue<qintptr>(qintptr(grpItem)), RibbonCustomizeWidget::PointerRole);
                grpSI->appendRow(stdItem);
            }
        }
        m_ribbonModel->appendRow(pageSI);
    }
}

/**
 * @brief Creates a random id in the form: pre_QDateTime::currentMSecsSinceEpoch
 * @param pre Prefix
 * @return
 */
QString RibbonCustomizeWidgetPrivate::makeRandomObjName(const QString &pre)
{
    return QString("%1_%2").arg(pre).arg(QDateTime::currentMSecsSinceEpoch());
}

/**
 * @brief Gets the level of the item
 * @param item
 * @return
 */
int RibbonCustomizeWidgetPrivate::itemLevel(QStandardItem *item) const
{
    return item->data(RibbonCustomizeWidget::LevelRole).toInt();
}

/**
 * @brief Checks whether the item is a customized item; customized items all carry the CustomizeRole role
 * @param item
 * @return
 */
bool RibbonCustomizeWidgetPrivate::isCustomizeItem(QStandardItem *item) const
{
    if (Q_NULLPTR == item) {
        return false;
    }
    return item->data(RibbonCustomizeWidget::CustomizeRole).isValid();
}

/**
 * @brief Converts the item to a page
 * @param item
 * @return Returns Q_NULLPTR if it cannot be converted
 */
RibbonPage *RibbonCustomizeWidgetPrivate::itemToPage(QStandardItem *item) const
{
    int level = item->data(RibbonCustomizeWidget::LevelRole).toInt();

    if (level != 0) {
        return Q_NULLPTR;
    }
    qintptr p = item->data(RibbonCustomizeWidget::PointerRole).value<qintptr>();

    return reinterpret_cast<RibbonPage *>(p);
}

/**
 * @brief Converts the item to a group
 * @param item
 * @return Returns Q_NULLPTR if it cannot be converted
 */
RibbonGroup *RibbonCustomizeWidgetPrivate::itemToGroup(QStandardItem *item) const
{
    int level = item->data(RibbonCustomizeWidget::LevelRole).toInt();

    if (level != 1) {
        return Q_NULLPTR;
    }
    qintptr p = item->data(RibbonCustomizeWidget::PointerRole).value<qintptr>();

    return reinterpret_cast<RibbonGroup *>(p);
}

/**
 * @brief Gets the object name corresponding to the item
 * @param item
 * @return Returns an empty QString if it cannot be obtained
 */
QString RibbonCustomizeWidgetPrivate::itemObjectName(QStandardItem *item) const
{
    QString objName;

    if (isCustomizeItem(item)) {
        // Indicates a customized item
        objName = item->data(RibbonCustomizeWidget::CustomizeObjNameRole).toString();
    } else {
        // Indicates a non-customized item
        int level = itemLevel(item);
        if (0 == level) {
            RibbonPage *page = itemToPage(item);
            if (page) {
                objName = page->objectName();
            }
        } else if (1 == level) {
            RibbonGroup *group = itemToGroup(item);
            if (group) {
                objName = group->objectName();
            }
        }
    }
    return objName;
}

/**
 * @brief Checks whether the item can be customized
 * @param item
 * @return
 */
bool RibbonCustomizeWidgetPrivate::isItemCanCustomize(QStandardItem *item) const
{
    if (Q_NULLPTR == item) {
        return false;
    }
    QVariant v = item->data(RibbonCustomizeWidget::CanCustomizeRole);

    if (v.isValid()) {
        return v.toBool();
    }
    return false;
}

/**
 * @brief Converts the item to an action
 * @param item
 * @return
 */
QAction *RibbonCustomizeWidgetPrivate::itemToAction(QStandardItem *item) const
{
    if (2 != itemLevel(item)) {
        return Q_NULLPTR;
    }
    // Note carefully: when RibbonCustomizeWidget::CustomizeRole is true, this is customized content,
    // and PointerRole then stores an action pointer, not a RibbonGroupItem
    QAction *act = Q_NULLPTR;
    if (item->data(RibbonCustomizeWidget::CustomizeRole).toBool()) {
        act = reinterpret_cast<QAction *>(item->data(RibbonCustomizeWidget::PointerRole).value<qintptr>());
    } else {
        RibbonGroupItem *grpItem =
            reinterpret_cast<RibbonGroupItem *>(item->data(RibbonCustomizeWidget::PointerRole).value<qintptr>());
        if (grpItem) {
            act = grpItem->action;
        }
    }
    return act;
}

/**
 * @brief RibbonCustomizeWidget::RibbonCustomizeWidget
 * @param ribbonBar The ribbonBar pointer to be managed
 * @param parent The parent used for UI generation
 * @param f Same as the second parameter of QWidget::QWidget
 */
RibbonCustomizeWidget::RibbonCustomizeWidget(RibbonBar *ribbonBar, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , ui(new RibbonCustomizeWidgetUi)
    , d(new RibbonCustomizeWidgetPrivate(this))
{
    d->m_ribbonBar = ribbonBar;

    ui->setupUi(this);
    ui->listViewSelect->setModel(d->m_acionModel);
    ui->treeViewResult->setModel(d->m_ribbonModel);
    initConnection();
    updateModel();
}

RibbonCustomizeWidget::~RibbonCustomizeWidget()
{
    delete ui;
    delete d;
}

void RibbonCustomizeWidget::initConnection()
{
    connect(ui->comboBoxActionIndex, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &RibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged);
    connect(ui->radioButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this,
            &RibbonCustomizeWidget::onRadioButtonGroupButtonClicked);
    connect(ui->pushButtonNewPage, &QPushButton::clicked, this, &RibbonCustomizeWidget::onPushButtonNewPageClicked);
    connect(ui->pushButtonNewGroup, &QPushButton::clicked, this, &RibbonCustomizeWidget::onPushButtonNewGroupClicked);
    connect(ui->pushButtonRename, &QPushButton::clicked, this, &RibbonCustomizeWidget::onPushButtonRenameClicked);
    connect(ui->pushButtonAdd, &QPushButton::clicked, this, &RibbonCustomizeWidget::onPushButtonAddClicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &RibbonCustomizeWidget::onPushButtonDeleteClicked);
    connect(ui->listViewSelect, &QAbstractItemView::clicked, this, &RibbonCustomizeWidget::onListViewSelectClicked);
    connect(ui->treeViewResult, &QAbstractItemView::clicked, this, &RibbonCustomizeWidget::onTreeViewResultClicked);
    connect(ui->toolButtonUp, &QToolButton::clicked, this, &RibbonCustomizeWidget::onToolButtonUpClicked);
    connect(ui->toolButtonDown, &QToolButton::clicked, this, &RibbonCustomizeWidget::onToolButtonDownClicked);
    connect(d->m_ribbonModel, &QStandardItemModel::itemChanged, this, &RibbonCustomizeWidget::onItemChanged);
    connect(ui->lineEditSearchAction, &QLineEdit::textEdited, this,
            &RibbonCustomizeWidget::onLineEditSearchActionTextEdited);
    connect(ui->pushButtonReset, &QPushButton::clicked, this, &RibbonCustomizeWidget::onPushButtonResetClicked);
}

/**
 * @brief Sets up the action manager
 * @param mgr
 */
void RibbonCustomizeWidget::setupActionsManager(RibbonActionsManager *mgr)
{
    if (mgr == Q_NULLPTR) {
        return;
    }
    if (d->m_actionMgr) {
        d->m_acionModel->uninstallActionsManager();
    }
    d->m_actionMgr = mgr;
    d->m_acionModel->setupActionsManager(mgr);
    // Updates the combo box on the left
    ui->comboBoxActionIndex->clear();
    const QList<int> tags = mgr->actionTags();
    for (int tag : tags) {
        ui->comboBoxActionIndex->addItem(mgr->tagName(tag), tag);
    }
}

/**
 * @brief Updates the model according to the current radio button option
 */
void RibbonCustomizeWidget::updateModel()
{
    updateModel(ui->radioButtonAllPage->isChecked() ? ShowAllPage : ShowMainPage);
    if (d->m_ribbonBar) {
        ui->comboBoxActionProportion->clear();
        if (d->m_ribbonBar->isTwoRowStyle()) {
            ui->comboBoxActionProportion->addItem(tr("large"), RibbonGroup::Large);
            ui->comboBoxActionProportion->addItem(tr("small"), RibbonGroup::Small);
        } else {
            ui->comboBoxActionProportion->addItem(tr("large"), RibbonGroup::Large);
            ui->comboBoxActionProportion->addItem(tr("medium"), RibbonGroup::Medium);
            ui->comboBoxActionProportion->addItem(tr("small"), RibbonGroup::Small);
        }
    }
}

/**
 * @brief Updates the model
 */
void RibbonCustomizeWidget::updateModel(RibbonTreeShowType type)
{
    d->m_showType = type;
    d->updateModel();
}

/**
 * @brief Applies all settings
 *
 * Apply first, then simplify, to ensure all operations are applied.
 *
 * If simplify runs before apply, the following problem occurs:
 * for example:
 *   1. First add an element and apply; the element is added to the ribbonBar
 *   2. Then remove that element and apply; since simplify runs first, the add and remove cancel each other out,
 *      so on apply the element cannot be removed from the ribbonBar
 *
 * @return Returns true on success
 * @note Returns true if any one of the settings is applied successfully
 */
bool RibbonCustomizeWidget::apply()
{
    int res = QxRibbonCustomizeDataApply(d->m_customizeDatas, d->m_ribbonBar);
    simplify();
    return (res > 0);
}

/**
 * @brief Converts to xml
 *
 * This function only writes elements and does not write document-related content.
 * Therefore, to write a document, call QXmlStreamWriter::writeStartDocument() before this
 * function and QXmlStreamWriter::writeEndDocument() after it.
 *
 * @note Before passing in the QXmlStreamWriter, set the encoding to utf-8: xml->setCodec("utf-8");
 * @note Since QXmlStreamWriter does not support encoding when a QString is used as the IO device,
 * and the customization process cannot be guaranteed to be free of Chinese characters,
 * QXmlStreamWriter should not be constructed from a QString; if a string is needed, construct it from a QByteArray,
 * e.g.:
 * @code
 * RibbonCustomizeDialog dlg(this);//this is the window of RibbonMainWindow
 * dlg.setupActionsManager(m_actMgr);
 * if (RibbonCustomizeDialog::Accepted == dlg.exec()) {
 *    dlg.apply();
 *    QByteArray str;
 *    QXmlStreamWriter xml(&str);//QXmlStreamWriter should not be constructed from a QString; it fails with Chinese
 * characters xml.setAutoFormatting(true); xml.setAutoFormattingIndent(2); xml.setCodec("utf-8");//Specify the encoding
 * before writeStartDocument xml.writeStartDocument(); bool isOk = dlg.toXml(&xml); xml.writeEndDocument(); if (isOk) {
 *        QFile f("customize.xml");
 *        if (f.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Truncate)) {
 *            QTextStream s(&f);
 *            s.setCodec("utf-8");//Specify the encoding for output
 *            s << str;
 *            s.flush();
 *        }
 *        m_edit->append("write xml:");//m_edit is defined as: QTextEdit *m_edit;
 *        m_edit->append(str);
 *    }
 * }
 * @endcode
 * @return Returns false on exception; also returns false when there is no customization data
 * @see QxRibbonCustomizeDataSetToXml
 */
bool RibbonCustomizeWidget::toXml(QXmlStreamWriter *xml) const
{
    QList<RibbonCustomizeData> res;

    res = d->m_oldCustomizeDatas + d->m_customizeDatas;
    res = RibbonCustomizeData::simplify(res);
    return QxRibbonCustomizeDataSetToXml(xml, res, d->m_oldCustomizeDatas.count() > 0);
}

/**
 * @brief Writes the configuration to a file
 * @param xmlpath
 * @return
 */
bool RibbonCustomizeWidget::toXml(const QString &xmlpath) const
{
    QFile f(xmlpath);
    if (!f.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        return false;
    }

    QXmlStreamWriter xml(&f);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)   // QXmlStreamWriter always encodes XML in UTF-8.
    xml.setCodec("utf-8");                   // Specify the encoding before writeStartDocument
#endif
    xml.writeStartDocument();
    bool isOK = toXml(&xml);
    xml.writeEndDocument();

    f.close();
    return isOK;
}

/**
 * @brief Loads QList<RibbonCustomizeData> from xml
 *
 * For file-based settings, call this before showing the dialog to ensure correct recording of stacked settings
 * @param xml
 * @note This function must be called after the @ref setupActionsManager function
 */
void RibbonCustomizeWidget::fromXml(QXmlStreamReader *xml)
{
    QList<RibbonCustomizeData> cds = QxRibbonCustomizeDataGetFromXml(xml, d->m_actionMgr);
    d->m_oldCustomizeDatas = cds;
}

/**
 * @brief Loads QList<RibbonCustomizeData> from xml
 *
 * For file-based settings, call this before showing the dialog to ensure correct recording of stacked settings
 * @param xmlpath
 * @note This function must be called after the @ref setupActionsManager function
 * @note If the application has loaded a custom configuration at startup, this function must be called when this
 * window is opened again to load the previous configuration; when new actions are generated the old actions are
 * preserved, but the actions loaded by this call are not applied when apply() is called
 */
void RibbonCustomizeWidget::fromXml(const QString &xmlpath)
{
    QFile f(xmlpath);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    f.seek(0);
    QXmlStreamReader xml(&f);

    fromXml(&xml);
}

/**
 * @brief Applies the xml configuration
 *
 * @note Loading the same configuration file twice causes an exception; to avoid this, typically use a variable to
 * ensure it is loaded only once, e.g.:
 * @code
 * // Can be called only once
 * static bool has_call = false;
 * if (!has_call) {
 *     QFile f("customize.xml");
 *     if (!f.open(QIODevice::ReadWrite|QIODevice::Text)) {
 *         return;
 *     }
 *     f.seek(0);
 *     QXmlStreamReader xml(&f);
 *     has_call = RibbonCustomizeWidget::fromXml(&xml, this, m_actMgr);
 * }
 * @endcode
 * @param xml
 * @param bar
 * @param mgr
 * @return Returns true if any one of the settings is applied successfully
 * @see QxRibbonCustomizeDataGetFromXml QxRibbonCustomizeDataApply QxRibbonCustomizeApplyFromXmlFile
 */
bool RibbonCustomizeWidget::fromXml(QXmlStreamReader *xml, RibbonBar *bar, RibbonActionsManager *mgr)
{
    QList<RibbonCustomizeData> cds = QxRibbonCustomizeDataGetFromXml(xml, mgr);
    return (QxRibbonCustomizeDataApply(cds, bar) > 0);
}

/**
 * @brief Clears all actions
 *
 * After calling apply(), if you want to continue, call clear(), otherwise an exception may occur
 */
void RibbonCustomizeWidget::clear()
{
    d->m_customizeDatas.clear();
}

/**
 * @brief Simplifies
 */
void RibbonCustomizeWidget::simplify()
{
    d->m_customizeDatas = RibbonCustomizeData::simplify(d->m_customizeDatas);
}

/**
 * @brief Gets the row proportion selected in the current interface
 * @return
 */
RibbonGroup::RowProportion RibbonCustomizeWidget::selectedRowProportion() const
{
    return static_cast<RibbonGroup::RowProportion>(ui->comboBoxActionProportion->currentData().toInt());
}

/**
 * @brief Gets the action selected in the listview
 * @return Returns Q_NULLPTR if no action is selected
 * @note To get the action selected in the treeview, use the @ref itemToAction function
 */
QAction *RibbonCustomizeWidget::selectedAction() const
{
    QItemSelectionModel *m = ui->listViewSelect->selectionModel();

    if ((Q_NULLPTR == m) || !m->hasSelection()) {
        return Q_NULLPTR;
    }
    QModelIndex i = m->currentIndex();

    return d->m_acionModel->indexToAction(i);
}

/**
 * @brief Converts the item to an action
 * @param item
 * @return Returns Q_NULLPTR if there is no action to convert
 */
QAction *RibbonCustomizeWidget::itemToAction(QStandardItem *item) const
{
    return d->itemToAction(item);
}

/**
 * @brief Gets the item selected in the ribbon tree
 * @return
 */
QStandardItem *RibbonCustomizeWidget::selectedItem() const
{
    QItemSelectionModel *m = ui->treeViewResult->selectionModel();

    if ((Q_NULLPTR == m) || !m->hasSelection()) {
        return Q_NULLPTR;
    }
    QModelIndex i = m->currentIndex();

    return d->m_ribbonModel->itemFromIndex(i);
}

/**
 * @brief Gets the level of the selected ribbon tree
 * @return -1 for an invalid selection, 0 for a selected page, 1 for a selected group, 2 for a selected action
 */
int RibbonCustomizeWidget::selectedRibbonLevel() const
{
    QStandardItem *item = selectedItem();

    if (item) {
        return itemLevel(item);
    }
    return -1;
}

/**
 * @brief Gets the level of the StandardItem
 * @param item
 * @return
 */
int RibbonCustomizeWidget::itemLevel(QStandardItem *item) const
{
    return d->itemLevel(item);
}

/**
 * @brief Selects the specified item
 * @param item
 * @param ensureVisible Whether to ensure the item is visible
 */
void RibbonCustomizeWidget::setSelectItem(QStandardItem *item, bool ensureVisible)
{
    QItemSelectionModel *m = ui->treeViewResult->selectionModel();
    if (Q_NULLPTR == m) {
        return;
    }

    m->clearSelection();
    // If select is used, it looks selected but itemFromIndex returns null.
    // For example, after creating a page and then creating a group, selectedItem in onPushButtonNewGroupClicked
    // returns null, so the group cannot be created.
    // Note: using SelectCurrent does not help either
    // m->select(item->index(), QItemSelectionModel::Select);
    m->setCurrentIndex(item->index(), QItemSelectionModel::Select);

    if (ensureVisible) {
        ui->treeViewResult->scrollTo(item->index());
    }
}

/**
 * @brief Checks whether the item can be modified; returns true if it can
 * @param item
 * @return
 */
bool RibbonCustomizeWidget::isItemCanCustomize(QStandardItem *item) const
{
    return d->isItemCanCustomize(item);
}

bool RibbonCustomizeWidget::isSelectedItemCanCustomize() const
{
    return isItemCanCustomize(selectedItem());
}

/**
 * @brief Checks whether the item can be modified; returns true if it can
 * @param item
 * @return
 */
bool RibbonCustomizeWidget::isCustomizeItem(QStandardItem *item) const
{
    return d->isCustomizeItem(item);
}

bool RibbonCustomizeWidget::isSelectedItemIsCustomize() const
{
    return isCustomizeItem(selectedItem());
}

void RibbonCustomizeWidget::removeItem(QStandardItem *item)
{
    if (item->parent()) {
        item->parent()->removeRow(item->row());
    } else {
        d->m_ribbonModel->removeRow(item->row());
    }
}

void RibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged(int index)
{
    int tag = ui->comboBoxActionIndex->itemData(index).toInt();

    d->m_acionModel->setFilter(tag);
}

void RibbonCustomizeWidget::onRadioButtonGroupButtonClicked(QAbstractButton *b)
{
    updateModel(b == ui->radioButtonAllPage ? ShowAllPage : ShowMainPage);
}

void RibbonCustomizeWidget::onPushButtonNewPageClicked()
{
    int row = d->m_ribbonModel->rowCount();
    QItemSelectionModel *m = ui->treeViewResult->selectionModel();

    if (m && m->hasSelection()) {
        QModelIndex i = m->currentIndex();
        while (i.parent().isValid()) {
            i = i.parent();
        }
        // Gets the topmost selected item
        row = i.row() + 1;
    }
    QStandardItem *ni = new QStandardItem(tr("new page[customize]%1").arg(++(d->m_customizePageCount)));

    ni->setData(0, RibbonCustomizeWidget::LevelRole);
    d->m_ribbonModel->insertRow(row, ni);
    // Sets the newly added item as selected
    setSelectItem(ni);
    // Inserts the action into the action list
    RibbonCustomizeData data = RibbonCustomizeData::makeAddPageCustomizeData(
        ni->text(), ni->row(), RibbonCustomizeWidgetPrivate::makeRandomObjName("page"));

    // No draft state, on purpose. Edits land in m_customizeDatas as they are
    // made, so they cannot be discarded one by one - discarding means dropping
    // the widget, and that is what the dialog does on Cancel. Since this class
    // is not exported (RibbonCustomizeDialog owns it and is the only way in),
    // an extra buffer would be machinery the lifetime already provides.
    d->m_customizeDatas.append(data);
    ni->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // Having CustomizeRole implies CanCustomizeRole
    ni->setData(true, RibbonCustomizeWidget::CustomizeRole);
    ni->setData(data.pageObjNameValue, RibbonCustomizeWidget::CustomizeObjNameRole);
}

void RibbonCustomizeWidget::onPushButtonNewGroupClicked()
{
    QStandardItem *item = selectedItem();

    if (Q_NULLPTR == item) {
        return;
    }
    int level = selectedRibbonLevel();

    QStandardItem *ni = new QStandardItem(tr("new group[customize]%1").arg(++(d->m_customizeGroupCount)));

    ni->setData(1, RibbonCustomizeWidget::LevelRole);

    if (0 == level) {
        // It is a page; insert at the end
        item->appendRow(ni);
    } else if (1 == level) {
        // A group is selected; insert after this group
        QStandardItem *pageItem = item->parent();
        if (Q_NULLPTR == pageItem) {
            return;
        }
        pageItem->insertRow(item->row() + 1, ni);
    } else {
        // Otherwise remove and exit
        delete ni;
        ni = Q_NULLPTR;
        return;
    }
    // Finds the object name of the page
    QStandardItem *pageItem = ni->parent();
    QString pageObjName = "";

    pageObjName = d->itemObjectName(pageItem);
    RibbonCustomizeData data = RibbonCustomizeData::makeAddGroupCustomizeData(
        ni->text(), ni->row(), pageObjName, RibbonCustomizeWidgetPrivate::makeRandomObjName("group"));

    // Pages are addressed by objectName when the customization is applied, so
    // two pages sharing one - an empty name in particular - would both resolve
    // to the first of them. That cannot happen from here: the model is built
    // only from pages whose objectName is set (see the loop in
    // setCustomizeMode()), groups with an empty objectName are skipped the same
    // way, and a page created here is given a random name. Giving every page a
    // unique id would be the alternative, and is more machinery than the
    // problem needs.
    d->m_customizeDatas.append(data);
    ni->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // Having CustomizeRole implies CanCustomizeRole
    ni->setData(true, RibbonCustomizeWidget::CustomizeRole);
    ni->setData(data.groupObjNameValue, RibbonCustomizeWidget::CustomizeObjNameRole);
    setSelectItem(ni);
}

void RibbonCustomizeWidget::onPushButtonRenameClicked()
{
    QStandardItem *item = selectedItem();

    if (Q_NULLPTR == item) {
        return;
    }
    bool ok;
    QString text = "";

    text = QInputDialog::getText(this, tr("rename"), tr("name:"), QLineEdit::Normal, item->text(), &ok);

    if (!ok || text.isEmpty()) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // Rename the page
        QString pageObjName = d->itemObjectName(item);
        RibbonCustomizeData data = RibbonCustomizeData::makeRenamePageCustomizeData(text, pageObjName);
        d->m_customizeDatas.append(data);
#if QX_RIBBON_DEBUG
        qDebug() << QString("rename page %1 from %2 to %3").arg(pageObjName, item->text(), text);
#endif
    } else if (1 == level) {
        QString pageObjName = d->itemObjectName(item->parent());
        QString groupObjName = d->itemObjectName(item);
        RibbonCustomizeData data = RibbonCustomizeData::makeRenameGroupCustomizeData(text, pageObjName, groupObjName);
        d->m_customizeDatas.append(data);
#if QX_RIBBON_DEBUG
        qDebug() << QString("rename group %1 from %2 to %3").arg(groupObjName, item->text(), text);
#endif
    } else {
        // Actions cannot be renamed
        return;
    }
    item->setText(text);
}

void RibbonCustomizeWidget::onPushButtonAddClicked()
{
    QAction *act = selectedAction();
    QStandardItem *item = selectedItem();

    if ((Q_NULLPTR == act) || (Q_NULLPTR == item)) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // Do nothing when a page is selected
        return;
    } else if (2 == level) {
        // An action is selected; add after this action by setting item to its group
        item = item->parent();
    }
    QString groupObjName = d->itemObjectName(item);
    QString pageObjName = d->itemObjectName(item->parent());
    QString key = d->m_actionMgr->key(act);

    RibbonCustomizeData data = RibbonCustomizeData::makeAddActionCustomizeData(
        key, d->m_actionMgr, selectedRowProportion(), pageObjName, groupObjName);

    d->m_customizeDatas.append(data);

    QStandardItem *actItem = new QStandardItem(act->icon(), act->text());

    actItem->setData(2, RibbonCustomizeWidget::LevelRole);
    actItem->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // Having CustomizeRole implies CanCustomizeRole
    actItem->setData(true, RibbonCustomizeWidget::CustomizeRole);
    actItem->setData(act->objectName(), RibbonCustomizeWidget::CustomizeObjNameRole);
    actItem->setData(qintptr(act), RibbonCustomizeWidget::PointerRole);   // Stores the action pointer
    item->appendRow(actItem);
}

void RibbonCustomizeWidget::onPushButtonDeleteClicked()
{
    QStandardItem *item = selectedItem();

    if (Q_NULLPTR == item) {
        return;
    }
    if (!isItemCanCustomize(item)) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // Remove the page
        RibbonCustomizeData data = RibbonCustomizeData::makeRemovePageCustomizeData(d->itemObjectName(item));
        d->m_customizeDatas.append(data);
    } else if (1 == level) {
        // Remove the group
        QString pageObjName = d->itemObjectName(item->parent());
        QString groupObjName = d->itemObjectName(item);
        RibbonCustomizeData data = RibbonCustomizeData::makeRemoveGroupCustomizeData(pageObjName, groupObjName);
        d->m_customizeDatas.append(data);
    } else if (2 == level) {
        // Remove the action
        QString pageObjName = d->itemObjectName(item->parent()->parent());
        QString groupObjName = d->itemObjectName(item->parent());
        QAction *act = itemToAction(item);
        QString key = d->m_actionMgr->key(act);
        if (key.isEmpty() || pageObjName.isEmpty() || groupObjName.isEmpty()) {
            return;
        }

        RibbonCustomizeData data =
            RibbonCustomizeData::makeRemoveActionCustomizeData(pageObjName, groupObjName, key, d->m_actionMgr);
        d->m_customizeDatas.append(data);
    }
    // Performs the removal
    removeItem(item);
    // Re-evaluates after removal
    ui->pushButtonAdd->setEnabled(selectedAction() && isSelectedItemIsCustomize() && selectedRibbonLevel() > 0);
    ui->pushButtonDelete->setEnabled(isSelectedItemIsCustomize());
}

void RibbonCustomizeWidget::onListViewSelectClicked(const QModelIndex &index)
{
    // On each click, decide whether the operation is available and set the display state of pushButtonAdd and
    // pushButtonDelete The listview was clicked, so check the treeview state
    Q_UNUSED(index);
    ui->pushButtonAdd->setEnabled(isSelectedItemCanCustomize() && selectedRibbonLevel() > 0);
    ui->pushButtonDelete->setEnabled(isSelectedItemCanCustomize());
}

void RibbonCustomizeWidget::onTreeViewResultClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    // On each click, decide whether the operation is available and set the display state of pushButtonAdd and
    // pushButtonDelete
    QStandardItem *item = selectedItem();

    if (Q_NULLPTR == item) {
        return;
    }
    int level = itemLevel(item);

    ui->pushButtonAdd->setEnabled(selectedAction() && (level > 0) && isItemCanCustomize(item));
    ui->pushButtonDelete->setEnabled(isItemCanCustomize(item));   // Having CustomizeRole implies CanCustomizeRole
    ui->pushButtonRename->setEnabled(level != 2 || isItemCanCustomize(item));   // QAction cannot be renamed
    ui->pushButtonNewGroup->setEnabled(level != 2);
    ui->toolButtonUp->setEnabled(item->index().row() != 0);

    QStandardItem *parItem = item->parent();
    if (parItem) {
        ui->toolButtonDown->setEnabled(item->index().row() + 1 < parItem->rowCount());
    } else {
        ui->toolButtonDown->setEnabled(item->index().row() + 1 < ui->treeViewResult->model()->rowCount());
    }
}

void RibbonCustomizeWidget::onToolButtonUpClicked()
{
    moveSelectedItem(-1);
}

void RibbonCustomizeWidget::onToolButtonDownClicked()
{
    moveSelectedItem(1);
}

/**
 * @brief Moves the selected item up/down
 * @param delta -1 moves up, 1 moves down
 * @note The two directions differ only by a sign: the recorded order-change value, and the target row within the parent
 * (row + delta)
 */
void RibbonCustomizeWidget::moveSelectedItem(int delta)
{
    QStandardItem *item = selectedItem();

    if (Q_NULLPTR == item) {
        return;
    }
    const int row = item->row();
    const int count = item->parent() ? item->parent()->rowCount() : d->m_ribbonModel->rowCount();

    // The first item cannot move up and the last item cannot move down
    if (((delta < 0) && (0 == row)) || ((delta > 0) && ((row + 1) >= count))) {
        return;
    }
    const int level = itemLevel(item);

    if (0 == level) {
        // Move the page
        RibbonCustomizeData data =
            RibbonCustomizeData::makeChangePageOrderCustomizeData(d->itemObjectName(item), delta);
        d->m_customizeDatas.append(data);
        item = d->m_ribbonModel->takeItem(row);
        d->m_ribbonModel->removeRow(row);
        d->m_ribbonModel->insertRow(row + delta, item);
    } else if (1 == level) {
        // Move the group
        QStandardItem *paritem = item->parent();
        RibbonCustomizeData data = RibbonCustomizeData::makeChangeGroupOrderCustomizeData(
            d->itemObjectName(paritem), d->itemObjectName(item), delta);
        d->m_customizeDatas.append(data);
        item = paritem->takeChild(row);
        paritem->removeRow(row);
        paritem->insertRow(row + delta, item);
    } else if (2 == level) {
        // Move the action
        QStandardItem *groupItem = item->parent();
        QStandardItem *pageItem = groupItem->parent();
        QAction *act = itemToAction(item);
        if (!act) {
            return;
        }
        QString key = d->m_actionMgr->key(act);
        RibbonCustomizeData data = RibbonCustomizeData::makeChangeActionOrderCustomizeData(
            d->itemObjectName(pageItem), d->itemObjectName(groupItem), key, d->m_actionMgr, delta);
        d->m_customizeDatas.append(data);
        item = groupItem->takeChild(row);
        groupItem->removeRow(row);
        groupItem->insertRow(row + delta, item);
    }
}

void RibbonCustomizeWidget::onItemChanged(QStandardItem *item)
{
    if (item == Q_NULLPTR) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        if (item->isCheckable()) {
            QString objname = d->itemObjectName(item);
            RibbonCustomizeData data =
                RibbonCustomizeData::makeVisiblePageCustomizeData(objname, item->checkState() == Qt::Checked);
            d->m_customizeDatas.append(data);
        }
    }
}

void RibbonCustomizeWidget::onLineEditSearchActionTextEdited(const QString &text)
{
    d->m_acionModel->search(text);
}

void RibbonCustomizeWidget::onPushButtonResetClicked()
{
    int btn = QMessageBox::warning(this, tr("Warning"), tr("Are you sure reset all customize setting?"),
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (btn == QMessageBox::Yes) {
        clear();
        d->updateModel();
    }
}

QX_RIBBON_END_NAMESPACE
