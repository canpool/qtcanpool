/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribboncustomizewidget.h"
#include "ribbonbar.h"
#include "ribboncustomizedata.h"
#include "ribbonwindow.h"
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
    // 如果xml中之前记录了添加页的定制数据，当再次删除该页时，经过simplify后，cds被抵消为空
    // 此时不如因为cds为空不写，那么xml中始终保留一份添加的定制数据，所以增加force参数，当cds为空，old数据非空时，应该强制写
    if (cds.size() <= 0 && !force) {
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
    // 先找到"qx-ribbon-customize"
    while (!xml->atEnd()) {
        qDebug() << "name:" << xml->name() << " qualifiedName:" << xml->qualifiedName();

        if (xml->isStartElement() && (xml->name().toString() == "qx-ribbon-customize")) {
            break;
        }
        xml->readNext();
    }
    QList<RibbonCustomizeData> res;

    // 开始遍历"customize-data"
    while (!xml->atEnd()) {
        if (xml->isStartElement() && (xml->name().toString() == "customize-data")) {
            // 首先读取属性type
            RibbonCustomizeData data;
            QXmlStreamAttributes attrs = xml->attributes();
            if (!attrs.hasAttribute("type")) {
                // 说明异常，跳过这个
                xml->readNextStartElement();
                continue;
            }
            bool isOk = false;
            int v = xml->attributes().value("type").toInt(&isOk);
            if (!isOk) {
                // 说明异常，跳过这个
                xml->readNextStartElement();
                continue;
            }
            data.setActionType(static_cast<RibbonCustomizeData::ActionType>(v));
            // 开始读取子对象
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
 * @brief 构建RibbonCustomizeWidget的Ui
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
        treeViewResult->setAnimated(true);                                    // 支持动画
        treeViewResult->setEditTriggers(QAbstractItemView::NoEditTriggers);   // 不允许直接在item上重命名

        verticalLayoutResult->addWidget(treeViewResult);

        horizontalLayoutActionOptBtns = new QHBoxLayout();
        horizontalLayoutActionOptBtns->setObjectName(QStringLiteral("horizontalLayoutActionOptBtns"));
        pushButtonNewPage = new QPushButton(customizeWidget);
        pushButtonNewPage->setObjectName(QStringLiteral("pushButtonNewPage"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonNewPage);

        pushButtonNewGroup = new QPushButton(customizeWidget);
        pushButtonNewGroup->setObjectName(QStringLiteral("pushButtonNewGroup"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonNewGroup);

        pushButtonRename = new QPushButton(customizeWidget);
        pushButtonRename->setObjectName(QStringLiteral("pushButtonRename"));

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

        verticalLayoutRightButtons->addWidget(toolButtonUp);

        toolButtonDown = new QToolButton(customizeWidget);
        toolButtonDown->setObjectName(QStringLiteral("pushButtonDown"));
        toolButtonDown->setArrowType(Qt::DownArrow);
        toolButtonDown->setAutoRaise(true);

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
            QApplication::translate("RibbonCustomizeWidget", "Please Select", Q_NULLPTR));   // cn:请选择
        lineEditSearchAction->setInputMask(QString());
        lineEditSearchAction->setText(QString());
        lineEditSearchAction->setPlaceholderText(
            QApplication::translate("RibbonCustomizeWidget", "Find Command", Q_NULLPTR));   // cn:查找命令
        pushButtonAdd->setText(QApplication::translate("RibbonCustomizeWidget", "Add >>", Q_NULLPTR));   // cn:添加 >>
        pushButtonDelete->setText(
            QApplication::translate("RibbonCustomizeWidget", "<< Remove", Q_NULLPTR));              // cn:<< 移除
        labelCustomize->setText(
            QApplication::translate("RibbonCustomizeWidget", "Customize the Ribbon", Q_NULLPTR));   // cn:自定义功能区
        radioButtonMainPage->setText(
            QApplication::translate("RibbonCustomizeWidget", "Main Page", Q_NULLPTR));          // cn:主要页
        radioButtonAllPage->setText(
            QApplication::translate("RibbonCustomizeWidget", "All Page", Q_NULLPTR));   // cn:所有页
        pushButtonNewPage->setText(
            QApplication::translate("RibbonCustomizeWidget", "New Page", Q_NULLPTR));   // cn:新建页
        pushButtonNewGroup->setText(
            QApplication::translate("RibbonCustomizeWidget", "New Group", Q_NULLPTR));      // cn:新建组
        pushButtonRename->setText(
            QApplication::translate("RibbonCustomizeWidget", "Rename", Q_NULLPTR));         // cn:重命名
        pushButtonReset->setText(QApplication::translate("RibbonCustomizeWidget", "reset", Q_NULLPTR));   // cn:重置
        labelProportion->setText(
            QApplication::translate("RibbonCustomizeWidget", "proportion:", Q_NULLPTR));                  // cn:比例
    }   // retranslateUi
};

/**
 * @brief 管理RibbonCustomizeWidget的业务逻辑
 */
class RibbonCustomizeWidgetPrivate
{
public:
    RibbonCustomizeWidgetPrivate(RibbonCustomizeWidget *p);

    void updateModel();

    // 创建一个随机id，形如：pre_QDateTime::currentMSecsSinceEpoch_suf
    static QString makeRandomObjName(const QString &pre);

    int itemLevel(QStandardItem *item) const;

    bool isCustomizeItem(QStandardItem *item) const;

    // 把item转换为page
    RibbonPage *itemToPage(QStandardItem *item) const;

    // 把item转换为RibbonGroup
    RibbonGroup *itemToGroup(QStandardItem *item) const;

    // 获取item对应的object name
    QString itemObjectName(QStandardItem *item) const;

    // 判断是否可以自定义
    bool isItemCanCustomize(QStandardItem *item) const;

    // 从item转为action
    QAction *itemToAction(QStandardItem *item) const;
public:
    RibbonCustomizeWidget *q;
    RibbonCustomizeWidget::RibbonTreeShowType m_showType;   ///< 显示类型
    RibbonBar *m_ribbonBar;                             ///< 保存RibbonBar的指针
    RibbonActionsManager *m_actionMgr;                  ///< action管理器
    RibbonActionsManagerModel *m_acionModel;            ///< action管理器对应的model
    QStandardItemModel *m_ribbonModel;                  ///< 用于生成ribbon的树
    int m_customizePageCount;                           ///< 记录自定义Page的个数
    int m_customizeGroupCount;                          ///< 记录自定义Group的个数

    QList<RibbonCustomizeData> m_customizeDatas;        ///< 记录所有的自定义动作
    QList<RibbonCustomizeData> m_oldCustomizeDatas;     ///< 记录旧的自定义动作
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

    for (RibbonPage *page : qAsConst(pages)) {
        if ((m_showType == RibbonCustomizeWidget::ShowMainPage) && page->isPageContext()) {
            // 如果是只显示主内容，如果是上下文标签就忽略
            continue;
        }
        QStandardItem *pageSI = new QStandardItem();
        if (page->isPageContext()) {
            pageSI->setText(QString("[%1]").arg(page->windowTitle()));
        } else {
            pageSI->setText(page->windowTitle());
        }
        if (page->isCanCustomize() && !page->isPageContext()) {
            // 上下文标签不做显示隐藏处理
            pageSI->setCheckable(true);
            pageSI->setCheckState(ribbonbar->isPageVisible(page) ? Qt::Checked : Qt::Unchecked);
            pageSI->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // 标记这个是可以自定义的
        }
        pageSI->setData(0, RibbonCustomizeWidget::LevelRole);
        pageSI->setData(QVariant::fromValue<qintptr>(qintptr(page)), RibbonCustomizeWidget::PointerRole);
        QList<RibbonGroup *> groups = page->groupList();
        for (RibbonGroup *grp : qAsConst(groups)) {
            QStandardItem *grpSI = new QStandardItem(grp->windowTitle());
            grpSI->setData(1, RibbonCustomizeWidget::LevelRole);
            grpSI->setData(QVariant::fromValue<qintptr>(qintptr(grp)), RibbonCustomizeWidget::PointerRole);
            if (grp->isCanCustomize()) {
                grpSI->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // 标记这个是可以自定义的
            }
            pageSI->appendRow(grpSI);
            const QList<RibbonGroupItem *> &goupItems = grp->d_ptr->ribbonGroupItems();
            for (RibbonGroupItem *grpItem : goupItems) {
                if (grpItem->action->isSeparator()) {
                    continue;
                }
                QStandardItem *stdItem = new QStandardItem();
                if (grpItem->customWidget) {
                    // 如果是自定义窗口
                    if (grpItem->widget()->windowTitle().isEmpty()/* && grpItem->widget()->windowIcon().isNull()*/) {
                        delete stdItem;
                        continue;   // 如果窗口啥也没有，就跳过
                    }
                    stdItem->setText(grpItem->widget()->windowTitle());
                    stdItem->setIcon(grpItem->widget()->windowIcon());
                    if (RibbonCustomizeData::isCanCustomize(grpItem->widget())) {
                        stdItem->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // 标记这个是可以自定义的
                    }
                } else {
                    // 不是自定义，说明是action
                    stdItem->setText(grpItem->action->text());
                    stdItem->setIcon(grpItem->action->icon());
                    if (RibbonCustomizeData::isCanCustomize(grpItem->action)) {
                        stdItem->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // 标记这个是可以自定义的
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
 * @brief 创建一个随机id，形如：pre_QDateTime::currentMSecsSinceEpoch
 * @param pre 前缀
 * @return
 */
QString RibbonCustomizeWidgetPrivate::makeRandomObjName(const QString &pre)
{
    return QString("%1_%2").arg(pre).arg(QDateTime::currentMSecsSinceEpoch());
}

/**
 * @brief 获取item的level
 * @param item
 * @return
 */
int RibbonCustomizeWidgetPrivate::itemLevel(QStandardItem *item) const
{
    return item->data(RibbonCustomizeWidget::LevelRole).toInt();
}

/**
 * @brief 判断itemn为自定义的item，自定义的item都带有CustomizeRole角色
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
 * @brief 把item转换为page
 * @param item
 * @return无法转换返回Q_NULLPTR
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
 * @brief 把item转换为group
 * @param item
 * @return 无法转换返回Q_NULLPTR
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
 * @brief 获取item对应的objectname
 * @param item
 * @return 如果无法获取，返回一个空的QString
 */
QString RibbonCustomizeWidgetPrivate::itemObjectName(QStandardItem *item) const
{
    QString objName;

    if (isCustomizeItem(item)) {
        // 说明是自定义的
        objName = item->data(RibbonCustomizeWidget::CustomizeObjNameRole).toString();
    } else {
        // 说明这个是非自定义的
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
 * @brief 判断item是否可自定义
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
 * @brief 从item 转为action
 * @param item
 * @return
 */
QAction *RibbonCustomizeWidgetPrivate::itemToAction(QStandardItem *item) const
{
    if (2 != itemLevel(item)) {
        return Q_NULLPTR;
    }
    // 这里要非常注意，RibbonCustomizeWidget::CustomizeRole为true时，说明这个是自定义的内容，
    // 这时PointerRole里存放的是action指针，不是RibbonGroupItem
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
 * @param ribbonBar 传入需要管理的ribbonBar指针
 * @param parent 用于界面生成的parent
 * @param f 同QWidget::QWidget的第二个参数
 */
RibbonCustomizeWidget::RibbonCustomizeWidget(RibbonBar *ribbonBar, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f), ui(new RibbonCustomizeWidgetUi), d(new RibbonCustomizeWidgetPrivate(this))
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    connect(ui->comboBoxActionIndex, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &RibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged);
    connect(ui->radioButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &RibbonCustomizeWidget::onRadioButtonGroupButtonClicked);
#else
    connect(ui->comboBoxActionIndex, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &RibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged);
    connect(ui->radioButtonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked),
            this, &RibbonCustomizeWidget::onRadioButtonGroupButtonClicked);
#endif
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
    connect(ui->lineEditSearchAction, &QLineEdit::textEdited,
            this, &RibbonCustomizeWidget::onLineEditSearchActionTextEdited);
    connect(ui->pushButtonReset, &QPushButton::clicked, this, &RibbonCustomizeWidget::onPushButtonResetClicked);
}

/**
 * @brief 设置action管理器
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
    // 更新左边复选框
    ui->comboBoxActionIndex->clear();
    const QList<int> tags = mgr->actionTags();
    for (int tag : tags) {
        ui->comboBoxActionIndex->addItem(mgr->tagName(tag), tag);
    }
}

bool RibbonCustomizeWidget::isChanged() const
{
    return (d->m_customizeDatas.size() > 0);
}

/**
 * @brief 获取model
 * @return
 */
const QStandardItemModel *RibbonCustomizeWidget::model() const
{
    return d->m_ribbonModel;
}

/**
 * @brief 根据当前的radiobutton选项来更新model
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
 * @brief 更新model
 */
void RibbonCustomizeWidget::updateModel(RibbonTreeShowType type)
{
    d->m_showType = type;
    d->updateModel();
}

/**
 * @brief 应用所有设定
 *
 * 先apply再simplify，保证操作都被应用到。
 *
 * 如果先simplify后apply会存在如下问题：
 * 比如：
 *   1.先添加一个元素，执行applys，元素被添加到ribbonWindow
 *   2.接着删除该元素，执行applys，由于先simplify，那么添加和删除相互抵消，
 *     再apply时，元素将无法从ribbonWindow中删除
 *
 * @return 应用成功返回true
 * @note 所有设定有一个应用成功都会返回true
 */
bool RibbonCustomizeWidget::applys()
{
    int res = QxRibbonCustomizeDataApply(d->m_customizeDatas, d->m_ribbonBar);
    simplify();
    return (res > 0);
}

/**
 * @brief 转换为xml
 *
 * 此函数仅会写element，不会写document相关内容，因此如果需要写document，
 * 需要在此函数前调用QXmlStreamWriter::writeStartDocument(),在此函数后调用QXmlStreamWriter::writeEndDocument()
 *
 * @note 注意，在传入QXmlStreamWriter之前，需要设置编码为utf-8:xml->setCodec("utf-8");
 * @note 由于QXmlStreamWriter在QString作为io时，是不支持编码的，而此又无法保证自定义过程不出现中文字符，
 * 因此，QXmlStreamWriter不应该通过QString进行构造，如果需要用到string，也需要通过QByteArray构造，如：
 * @code
 * RibbonCustomizeDialog dlg(this);//this为RibbonWindow的窗口
 * dlg.setupActionsManager(m_actMgr);
 * if (RibbonCustomizeDialog::Accepted == dlg.exec()) {
 *    dlg.applys();
 *    QByteArray str;
 *    QXmlStreamWriter xml(&str);//QXmlStreamWriter不建议通过QString构造，遇到中文会异常
 *    xml.setAutoFormatting(true);
 *    xml.setAutoFormattingIndent(2);
 *    xml.setCodec("utf-8");//在writeStartDocument之前指定编码
 *    xml.writeStartDocument();
 *    bool isok = dlg.toXml(&xml);
 *    xml.writeEndDocument();
 *    if (isok) {
 *        QFile f("customize.xml");
 *        if (f.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Truncate)) {
 *            QTextStream s(&f);
 *            s.setCodec("utf-8");//指定编码输出
 *            s << str;
 *            s.flush();
 *        }
 *        m_edit->append("write xml:");//m_edit的定义为：QTextEdit *m_edit;
 *        m_edit->append(str);
 *    }
 * }
 * @endcode
 * @return 如果出现异常，返回false,如果没有自定义数据也会返回false
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
 * @brief 把配置写入文件中
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
    xml.setCodec("utf-8");                   // 在writeStartDocument之前指定编码
#endif
    xml.writeStartDocument();
    bool isOK = toXml(&xml);
    xml.writeEndDocument();

    f.close();
    return isOK;
}

/**
 * @brief 从xml中加载QList<RibbonCustomizeData>
 *
 * 对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
 * @param xml
 * @return
 * @note 此函数要在@ref setupActionsManager 函数之后调用
 */
void RibbonCustomizeWidget::fromXml(QXmlStreamReader *xml)
{
    QList<RibbonCustomizeData> cds = QxRibbonCustomizeDataGetFromXml(xml, d->m_actionMgr);
    d->m_oldCustomizeDatas = cds;
}

/**
 * @brief 从xml中加载QList<RibbonCustomizeData>
 *
 * 对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
 * @param xmlpath
 * @note 此函数要在@ref setupActionsManager 函数之后调用
 * @note 如果程序启动后加载了自定义配置，再调用此窗口时需要调用此函数，把原来的配置加载进来，
 * 在生成新动作时会把旧动作保存，但在调用applys时不会调用此加载的动作
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
 * @brief 应用xml配置
 *
 * @note 重复加载一个配置文件会发生异常，为了避免此类事件发生，一般通过一个变量保证只加载一次，如：
 * @code
 * //只能调用一次
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
 * @return 所有设定有一个应用成功都会返回true
 * @see QxRibbonCustomizeDataGetFromXml QxRibbonCustomizeDataApply QxRibbonCustomizeApplyFromXmlFile
 */
bool RibbonCustomizeWidget::fromXml(QXmlStreamReader *xml, RibbonBar *bar, RibbonActionsManager *mgr)
{
    QList<RibbonCustomizeData> cds = QxRibbonCustomizeDataGetFromXml(xml, mgr);
    return (QxRibbonCustomizeDataApply(cds, bar) > 0);
}

/**
 * @brief 清除所有动作
 *
 * 在执行applys函数后，如果要继续调用，应该clear，否则会导致异常
 */
void RibbonCustomizeWidget::clear()
{
    d->m_customizeDatas.clear();
}

/**
 * @brief 精简
 */
void RibbonCustomizeWidget::simplify()
{
    d->m_customizeDatas = RibbonCustomizeData::simplify(d->m_customizeDatas);
}

/**
 * @brief 获取当前界面选中的行属性
 * @return
 */
RibbonGroup::RowProportion RibbonCustomizeWidget::selectedRowProportion() const
{
    return static_cast<RibbonGroup::RowProportion>(ui->comboBoxActionProportion->currentData().toInt());
}

/**
 * @brief 获取listview中选中的action
 * @return 如果没有选中action，返回Q_NULLPTR
 * @note 如果要获取treeview选中的action，使用@ref itemToAction 函数
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
 * @brief 把item转换为action
 * @param item
 * @return 如果没有action可转换，返回Q_NULLPTR
 */
QAction *RibbonCustomizeWidget::itemToAction(QStandardItem *item) const
{
    return d->itemToAction(item);
}

/**
 * @brief 获取ribbon tree选中的item
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
 * @brief 获取选中的ribbon tree 的level
 * @return -1为选中异常，0代表选中了page 1代表选中了group 2代表选中了action
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
 * @brief 获取StandardItem 的level
 * @param item
 * @return
 */
int RibbonCustomizeWidget::itemLevel(QStandardItem *item) const
{
    return d->itemLevel(item);
}

/**
 * @brief 设置某个item被选中
 * @param item
 */
void RibbonCustomizeWidget::setSelectItem(QStandardItem *item, bool ensureVisible)
{
    QItemSelectionModel *m = ui->treeViewResult->selectionModel();
    if (Q_NULLPTR == m) {
        return;
    }

    m->clearSelection();
    // 如果用select，看起来选中了，但是itemFromIndex却返回空
    // 比如：新建一个页，接着新建一个组，onPushButtonNewGroupClicked中的selectedItem返回空，导致无法新建组。
    // 备注：命令用SelectCurrent也没有用
    //m->select(item->index(), QItemSelectionModel::Select);
    m->setCurrentIndex(item->index(), QItemSelectionModel::Select);

    if (ensureVisible) {
        ui->treeViewResult->scrollTo(item->index());
    }
}

/**
 * @brief 判断itemn能否改动，可以改动返回true
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
 * @brief 判断itemn能否改动，可以改动返回true
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
        // 获取选中的最顶层item
        row = i.row() + 1;
    }
    QStandardItem *ni = new QStandardItem(tr("new page[customize]%1").arg(++(d->m_customizePageCount)));

    ni->setData(0, RibbonCustomizeWidget::LevelRole);
    d->m_ribbonModel->insertRow(row, ni);
    // 设置新增的为选中
    setSelectItem(ni);
    // 把动作插入动作列表中
    RibbonCustomizeData data = RibbonCustomizeData::makeAddPageCustomizeData(
        ni->text(), ni->row(), RibbonCustomizeWidgetPrivate::makeRandomObjName("page"));

    // FIXME: 操作数据无缓冲，导致操作无法废弃，且缺少apply按钮（当前设计，添加按钮意义并不大）
    // DO: RibbonCustomizeWidget 不再对外提供，而是通过 RibbonCustomizeDialog 进行定制，
    // 如果不应用定制数据，数据会随着 RibbonCustomizeDialog 销毁而废弃，所以不需要再增加缓冲
    d->m_customizeDatas.append(data);
    ni->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // 有CustomizeRole，必有CanCustomizeRole
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
        // 说明是page,插入到最后
        item->appendRow(ni);
    } else if (1 == level) {
        // 说明选择的是group，插入到此group之后
        QStandardItem *pageItem = item->parent();
        if (Q_NULLPTR == pageItem) {
            return;
        }
        pageItem->insertRow(item->row() + 1, ni);
    } else {
        // 不符合就删除退出
        delete ni;
        ni = Q_NULLPTR;
        return;
    }
    // 查找page的object name
    QStandardItem *pageItem = ni->parent();
    QString pageObjName = "";

    pageObjName = d->itemObjectName(pageItem);
    RibbonCustomizeData data = RibbonCustomizeData::makeAddGroupCustomizeData(
        ni->text(), ni->row(), pageObjName, RibbonCustomizeWidgetPrivate::makeRandomObjName("group"));

    d->m_customizeDatas.append(data);
    ni->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // 有CustomizeRole，必有CanCustomizeRole
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
        // 改Page名
        QString cateObjName = d->itemObjectName(item);
        RibbonCustomizeData data = RibbonCustomizeData::makeRenamePageCustomizeData(text, cateObjName);
        d->m_customizeDatas.append(data);
    } else if (1 == level) {
        QString cateObjName = d->itemObjectName(item->parent());
        QString groupObjName = d->itemObjectName(item);
        RibbonCustomizeData data = RibbonCustomizeData::makeRenameGroupCustomizeData(text, cateObjName, groupObjName);
        d->m_customizeDatas.append(data);
    } else {
        // action 不允许改名
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
        // 选中page不进行操作
        return;
    } else if (2 == level) {
        // 选中action，添加到这个action之后,把item设置为group
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
    actItem->setData(true, RibbonCustomizeWidget::CanCustomizeRole);   // 有CustomizeRole，必有CanCustomizeRole
    actItem->setData(true, RibbonCustomizeWidget::CustomizeRole);
    actItem->setData(act->objectName(), RibbonCustomizeWidget::CustomizeObjNameRole);
    actItem->setData(qintptr(act), RibbonCustomizeWidget::PointerRole);   // 把action指针传入
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
        // 删除page
        RibbonCustomizeData data = RibbonCustomizeData::makeRemovePageCustomizeData(d->itemObjectName(item));
        d->m_customizeDatas.append(data);
    } else if (1 == level) {
        // 删除group
        QString catObjName = d->itemObjectName(item->parent());
        QString groupObjName = d->itemObjectName(item);
        RibbonCustomizeData data = RibbonCustomizeData::makeRemoveGroupCustomizeData(catObjName, groupObjName);
        d->m_customizeDatas.append(data);
    } else if (2 == level) {
        // 删除Action
        QString catObjName = d->itemObjectName(item->parent()->parent());
        QString groupObjName = d->itemObjectName(item->parent());
        QAction *act = itemToAction(item);
        QString key = d->m_actionMgr->key(act);
        if (key.isEmpty() || catObjName.isEmpty() || groupObjName.isEmpty()) {
            return;
        }

        RibbonCustomizeData data =
            RibbonCustomizeData::makeRemoveActionCustomizeData(catObjName, groupObjName, key, d->m_actionMgr);
        d->m_customizeDatas.append(data);
    }
    // 执行删除操作
    removeItem(item);
    // 删除后重新识别
    ui->pushButtonAdd->setEnabled(selectedAction() && isSelectedItemIsCustomize() && selectedRibbonLevel() > 0);
    ui->pushButtonDelete->setEnabled(isSelectedItemIsCustomize());
}

void RibbonCustomizeWidget::onListViewSelectClicked(const QModelIndex &index)
{
    // 每次点击，判断是否可以进行操作，决定pushButtonAdd和pushButtonDelete的显示状态
    // 点击了listview，判断treeview的状态
    Q_UNUSED(index);
    ui->pushButtonAdd->setEnabled(isSelectedItemCanCustomize() && selectedRibbonLevel() > 0);
    ui->pushButtonDelete->setEnabled(isSelectedItemCanCustomize());
}

void RibbonCustomizeWidget::onTreeViewResultClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    // 每次点击，判断是否可以进行操作，决定pushButtonAdd和pushButtonDelete的显示状态
    QStandardItem *item = selectedItem();

    if (Q_NULLPTR == item) {
        return;
    }
    int level = itemLevel(item);

    ui->pushButtonAdd->setEnabled(selectedAction() && (level > 0) && isItemCanCustomize(item));
    ui->pushButtonDelete->setEnabled(isItemCanCustomize(item));   // 有CustomizeRole，必有CanCustomizeRole
    ui->pushButtonRename->setEnabled(
        level != 2 || isItemCanCustomize(item));   // QAction 不能改名 ， 有CustomizeRole，必有CanCustomizeRole
}

void RibbonCustomizeWidget::onToolButtonUpClicked()
{
    QStandardItem *item = selectedItem();

    if ((Q_NULLPTR == item) || (0 == item->row())) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 移动page
        RibbonCustomizeData data =
            RibbonCustomizeData::makeChangePageOrderCustomizeData(d->itemObjectName(item), -1);
        d->m_customizeDatas.append(data);
        int r = item->row();
        item = d->m_ribbonModel->takeItem(r);
        d->m_ribbonModel->removeRow(r);
        d->m_ribbonModel->insertRow(r - 1, item);
    } else if (1 == level) {
        QStandardItem *paritem = item->parent();
        RibbonCustomizeData data = RibbonCustomizeData::makeChangeGroupOrderCustomizeData(
            d->itemObjectName(paritem), d->itemObjectName(item), -1);
        d->m_customizeDatas.append(data);
        int r = item->row();
        item = paritem->takeChild(r);
        paritem->removeRow(r);
        paritem->insertRow(r - 1, item);
    } else if (2 == level) {
        QStandardItem *groupItem = item->parent();
        QStandardItem *pageItem = groupItem->parent();
        QAction *act = itemToAction(item);
        if (!act) {
            return;
        }
        QString key = d->m_actionMgr->key(act);
        RibbonCustomizeData data = RibbonCustomizeData::makeChangeActionOrderCustomizeData(
            d->itemObjectName(pageItem), d->itemObjectName(groupItem), key, d->m_actionMgr, -1);
        d->m_customizeDatas.append(data);
        int r = item->row();
        item = groupItem->takeChild(r);
        groupItem->removeRow(r);
        groupItem->insertRow(r - 1, item);
    }
}

void RibbonCustomizeWidget::onToolButtonDownClicked()
{
    QStandardItem *item = selectedItem();

    if (item == Q_NULLPTR) {
        return;
    }
    int count = 0;

    if (item->parent()) {
        count = item->parent()->rowCount();
    } else {
        count = d->m_ribbonModel->rowCount();
    }
    if ((Q_NULLPTR == item) || ((count - 1) == item->row())) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 移动page
        RibbonCustomizeData data =
            RibbonCustomizeData::makeChangePageOrderCustomizeData(d->itemObjectName(item), 1);
        d->m_customizeDatas.append(data);
        int r = item->row();
        item = d->m_ribbonModel->takeItem(item->row());
        d->m_ribbonModel->removeRow(r);
        d->m_ribbonModel->insertRow(r + 1, item);
    } else if (1 == level) {
        QStandardItem *paritem = item->parent();
        RibbonCustomizeData data = RibbonCustomizeData::makeChangeGroupOrderCustomizeData(d->itemObjectName(paritem),
                                                                                          d->itemObjectName(item), 1);
        d->m_customizeDatas.append(data);
        int r = item->row();
        item = paritem->takeChild(r);
        paritem->removeRow(r);
        paritem->insertRow(r + 1, item);
    } else if (2 == level) {
        QStandardItem *groupItem = item->parent();
        QStandardItem *pageItem = groupItem->parent();
        QAction *act = itemToAction(item);
        if (!act) {
            return;
        }
        QString key = d->m_actionMgr->key(act);
        RibbonCustomizeData data = RibbonCustomizeData::makeChangeActionOrderCustomizeData(
            d->itemObjectName(pageItem), d->itemObjectName(groupItem), key, d->m_actionMgr, -1);
        d->m_customizeDatas.append(data);
        int r = item->row();
        item = groupItem->takeChild(r);
        groupItem->removeRow(r);
        groupItem->insertRow(r + 1, item);
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
    }
}

QX_RIBBON_END_NAMESPACE
