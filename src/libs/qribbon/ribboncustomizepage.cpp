/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribboncustomizepage.h"
#include "officestyle_p.h"
#include "ribbon_def.h"
#include "ribbon_p.h"
#include "ribbonbar.h"
#include "ribbonbutton_p.h"
#include "ribboncustomizedialog_p.h"
#include "ribboncustomizemanager.h"
#include "ribboncustomizemanager_p.h"
#include "ribbongroup.h"
#include "ribbonpage.h"
#include "ribbonquickaccessbar.h"
#include "stylehelpers.h"
#include "ui_ribbonrenamedialog.h"
#include <QBitmap>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QRadioButton>
#include <QScrollBar>
#include <QSpinBox>
#include <QWidgetAction>

QRIBBON_USE_NAMESPACE

static const QColor clrTransparent = QColor(255, 0, 255);

/* RibbonCustomizeStyledItemDelegate */
RibbonCustomizeStyledItemDelegate::RibbonCustomizeStyledItemDelegate(QListWidget *listWidget)
    : QStyledItemDelegate(listWidget), m_listWidget(listWidget)
{
}

RibbonCustomizeStyledItemDelegate::~RibbonCustomizeStyledItemDelegate() {}

void RibbonCustomizeStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    QStyleOptionViewItemV4 opt = option;
#else
    QStyleOptionViewItem opt = option;
#endif
    initStyleOption(&opt, index);

    const QWidget *widget = m_listWidget;
    QStyle *style = widget ? widget->style() : QApplication::style();

    painter->save();
    painter->setClipRect(opt.rect);

    QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget);
    iconRect.setLeft(3);
    iconRect.setRight(19);
    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);
    int offset = iconRect.width() + 3;
    textRect.translate(offset - textRect.x(), 0);

    // draw the background
    style->proxy()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, widget);

    // draw the icon
    QIcon::Mode mode = QIcon::Normal;
    if (!(opt.state & QStyle::State_Enabled))
        mode = QIcon::Disabled;
    else if (opt.state & QStyle::State_Selected)
        mode = QIcon::Selected;
    QIcon::State state = opt.state & QStyle::State_Open ? QIcon::On : QIcon::Off;
    opt.icon.paint(painter, iconRect, opt.decorationAlignment, mode, state);

    // draw the text
    if (!opt.text.isEmpty()) {
        QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
        if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
            cg = QPalette::Inactive;

        if (opt.state & QStyle::State_Selected)
            painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
        else
            painter->setPen(opt.palette.color(cg, QPalette::Text));

        if (opt.state & QStyle::State_Editing) {
            painter->setPen(opt.palette.color(cg, QPalette::Text));
            painter->drawRect(textRect.adjusted(0, 0, -1, -1));
        }

        const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;
        textRect = textRect.adjusted(textMargin, 0, -textMargin, 0);   // remove width padding

        painter->setFont(opt.font);
        style->drawItemText(painter, textRect, QStyle::visualAlignment(opt.direction, opt.displayAlignment),
                            opt.palette, true /*enabled*/,
                            opt.text /*, QPalette::ColorRole textRole = QPalette::NoRole*/);
    }

    // draw the focus rect
    if (opt.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect o;
        o.QStyleOption::operator=(opt);
        o.rect = style->proxy()->subElementRect(QStyle::SE_ItemViewItemFocusRect, &opt, widget);
        o.state |= QStyle::State_KeyboardFocusChange;
        o.state |= QStyle::State_Item;
        QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;
        o.backgroundColor =
            opt.palette.color(cg, (opt.state & QStyle::State_Selected) ? QPalette::Highlight : QPalette::Window);
        style->proxy()->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, widget);
    }

    painter->restore();
}

/* RibbonQuickAccessBarCustomizePagePrivate */
RibbonQuickAccessBarCustomizePagePrivate::RibbonQuickAccessBarCustomizePagePrivate()
{
    m_ribbonBar = Q_NULL;
    m_separator = Q_NULL;
    m_currentAction = Q_NULL;
    m_separatorText = RibbonBar::tr_compatible(RibbonSeparatorString);
    m_heightRowItem = 16;
    m_widthColIconItem = 20;
    m_widthColViewItem = 35;
    m_wasDisplayed = false;
}

RibbonQuickAccessBarCustomizePagePrivate::~RibbonQuickAccessBarCustomizePagePrivate() { delete m_separator; }

void RibbonQuickAccessBarCustomizePagePrivate::init()
{
    QTC_Q(RibbonQuickAccessBarCustomizePage)
    Q_ASSERT(m_ribbonBar != Q_NULL);
    m_pageUI.setupUi(q);

    QAbstractItemDelegate *itemDelegate = m_pageUI.commandsList->itemDelegate();
    m_pageUI.commandsList->setItemDelegate(new RibbonCustomizeStyledItemDelegate(m_pageUI.commandsList));
    delete itemDelegate;

    QPixmap imagesSrc;
    imagesSrc = QPixmap(":/res/menuimg-pro24.png");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QImage img = imagesSrc.toImage();
    img.setAlphaChannel(img.createMaskFromColor(clrTransparent.rgb(), Qt::MaskOutColor));
    imagesSrc = QPixmap::fromImage(img);
#else
    imagesSrc.setAlphaChannel(imagesSrc.createMaskFromColor(clrTransparent, Qt::MaskOutColor));
#endif
    QSize sz = QSize(9, 9);
    QPixmap m_imagesUpSrc =
        imagesSrc.copy(QRect(QPoint((int)OfficePaintManager::Icon_ArowUpLarge * sz.width(), 0), sz));
    QPixmap m_imagesDownSrc =
        imagesSrc.copy(QRect(QPoint((int)OfficePaintManager::Icon_ArowDownLarge * sz.width(), 0), sz));

    m_pageUI.upButton->setText("");
    m_pageUI.upButton->setIcon(m_imagesUpSrc);
    m_pageUI.downButton->setText("");
    m_pageUI.downButton->setIcon(m_imagesDownSrc);

    m_separator = new QAction(m_separatorText, Q_NULL);
    m_separator->setSeparator(true);

    initListWidgetCommands();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    connect(m_pageUI.comboBoxSourceCategory, SIGNAL(currentTextChanged(const QString &)), this,
            SLOT(setCurrentCategoryText(const QString &)));
#else
    connect(m_pageUI.comboBoxSourceCategory, SIGNAL(currentIndexChanged(const QString &)), this,
            SLOT(setCurrentCategoryText(const QString &)));
#endif

    connect(m_pageUI.addButton, SIGNAL(released()), this, SLOT(addClicked()));
    connect(m_pageUI.removeButton, SIGNAL(released()), this, SLOT(removeClicked()));
    connect(m_pageUI.pushButtonReset, SIGNAL(released()), this, SLOT(resetClicked()));
    connect(m_pageUI.upButton, SIGNAL(released()), this, SLOT(upClicked()));
    connect(m_pageUI.downButton, SIGNAL(released()), this, SLOT(downClicked()));

    connect(m_pageUI.commandsList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this,
            SLOT(currentCommandChanged(QListWidgetItem *)));
    connect(m_pageUI.quickAccessBarList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this,
            SLOT(currentQTCommandChanged(QListWidgetItem *)));

    m_pageUI.checkBoxQATOn->setChecked(m_ribbonBar->quickAccessBarPosition() != RibbonBar::QATopPosition);
}

void RibbonQuickAccessBarCustomizePagePrivate::setupPage()
{
    Q_ASSERT(m_ribbonBar != Q_NULL);

    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    manager->setEditMode(true);

    fillActions();
    fillStateCommands();
    setButtons();
}

QIcon RibbonQuickAccessBarCustomizePagePrivate::createIconStyleWidget(QWidget *widget)
{
    return DrawHelpers::createIconStyleWidget(widget);
}

QPixmap RibbonQuickAccessBarCustomizePagePrivate::createIconExStyleWidget(QWidget *widget, bool &iconView)
{
    return DrawHelpers::createIconExStyleWidget(widget, m_widthColViewItem, m_heightRowItem, iconView);
}

void RibbonQuickAccessBarCustomizePagePrivate::initListWidgetCommands()
{
    QStyleOption opt;
    opt.init(m_pageUI.commandsList);

    QString str = QString("XX");
    int width = opt.fontMetrics.boundingRect(str).width();
    m_heightRowItem = qMax(m_heightRowItem, width);
}

void RibbonQuickAccessBarCustomizePagePrivate::fillActions()
{
    m_pageUI.comboBoxSourceCategory->clear();
    m_pageUI.comboBoxSourceCategory->addItems(m_sourceCategories);
}

void RibbonQuickAccessBarCustomizePagePrivate::fillStateCommands()
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    m_pageUI.quickAccessBarList->clear();

    setButtons();
    QListIterator<QAction *> itAction(manager->actions(m_ribbonBar->quickAccessBar()));
    QListWidgetItem *first = Q_NULL;

    while (itAction.hasNext()) {
        QAction *action = itAction.next();

        QString actionName = m_separatorText;
        if (action)
            actionName = action->text();

        actionName.remove(QChar::fromLatin1('&'));

        if (!actionName.isEmpty()) {
            QListWidgetItem *item = new QListWidgetItem(actionName, m_pageUI.quickAccessBarList);
            if (action) {
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                QIcon icon = action->property("icon").value<QIcon>();
                item->setIcon(icon);
            }
            if (!first)
                first = item;
        }
    }
    if (first)
        m_pageUI.quickAccessBarList->setCurrentItem(first);
}

void RibbonQuickAccessBarCustomizePagePrivate::fillListCategorieCommads(const QList<QAction *> &actions)
{
    m_pageUI.commandsList->clear();
    m_actionToItem.clear();
    m_itemToAction.clear();

    QListWidgetItem *item = new QListWidgetItem(m_separatorText, m_pageUI.commandsList);
    m_pageUI.commandsList->setCurrentItem(item);
    m_currentAction = item;
    m_actionToItem.insert(0, item);
    m_itemToAction.insert(item, 0);

    QListIterator<QAction *> itAction(actions);
    while (itAction.hasNext()) {
        if (QAction *action = itAction.next()) {
            QString actionName = action->text();
            actionName.remove(QChar::fromLatin1('&'));

            item = new QListWidgetItem(actionName, m_pageUI.commandsList);
            item->setIcon(action->icon());
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            m_actionToItem.insert(action, item);
            m_itemToAction.insert(item, action);
        }
    }
    m_pageUI.commandsList->setCurrentItem(m_currentAction);
}

void RibbonQuickAccessBarCustomizePagePrivate::setButtons()
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    bool addEnabled = false;
    bool removeEnabled = false;

    QListWidgetItem *itemCommand = m_pageUI.commandsList->currentItem();
    QListWidgetItem *itemCommandQT = m_pageUI.quickAccessBarList->currentItem();
    if (itemCommand && (itemCommandQT || m_pageUI.quickAccessBarList->count() == 0)) {
        if (itemCommand->text() != m_separatorText) {
            QAction *action = m_itemToAction.value(itemCommand);
            if (action && !manager->containsAction(m_ribbonBar->quickAccessBar(), action))
                addEnabled = true;
        } else
            addEnabled = true;
        removeEnabled = true;
    }
    m_pageUI.addButton->setEnabled(addEnabled);
    m_pageUI.removeButton->setEnabled(removeEnabled);

    int indexRow = m_pageUI.quickAccessBarList->currentRow();
    m_pageUI.upButton->setEnabled(indexRow != 0);
    m_pageUI.downButton->setEnabled(indexRow < m_pageUI.quickAccessBarList->count() - 1);
}

void RibbonQuickAccessBarCustomizePagePrivate::applyClicked()
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);
    manager->commit();

    if (m_ribbonBar->isQuickAccessVisible())
        m_ribbonBar->setQuickAccessBarPosition(m_pageUI.checkBoxQATOn->isChecked() ? RibbonBar::QABottomPosition
                                                                                   : RibbonBar::QATopPosition);
}

void RibbonQuickAccessBarCustomizePagePrivate::setCurrentCategoryText(const QString &strCategory)
{
    if (strCategory.isEmpty())
        return;

    if (m_strSeparator == strCategory) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        m_pageUI.comboBoxSourceCategory->setCurrentText(m_currentSourceCategory);
#else
        int index = m_pageUI.comboBoxSourceCategory->findText(m_currentSourceCategory);
        if (index != -1)
            m_pageUI.comboBoxSourceCategory->setCurrentIndex(index);
#endif
        return;
    }
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    QList<QAction *> actions = manager->actionsByCategory(strCategory);
    fillListCategorieCommads(actions);
    m_currentSourceCategory = strCategory;
}

void RibbonQuickAccessBarCustomizePagePrivate::currentCommandChanged(QListWidgetItem *current)
{
    if (m_itemToAction.contains(current))
        m_currentAction = current;
    else
        m_currentAction = Q_NULL;
    setButtons();
}

void RibbonQuickAccessBarCustomizePagePrivate::currentQTCommandChanged(QListWidgetItem *current)
{
    Q_UNUSED(current);
    setButtons();
}

void RibbonQuickAccessBarCustomizePagePrivate::addClicked()
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    if (!m_currentAction)
        return;

    QListWidgetItem *currentToolBarAction = m_pageUI.quickAccessBarList->currentItem();

    QAction *action = m_itemToAction.value(m_currentAction);
    QListWidgetItem *item = Q_NULL;
    if (action) {
        if (manager->containsAction(m_ribbonBar->quickAccessBar(), action)) {
            item = m_actionToCurrentItem.value(action);

            if (item == currentToolBarAction)
                return;

            int row = m_pageUI.quickAccessBarList->row(item);
            m_pageUI.quickAccessBarList->takeItem(row);
            manager->removeActionAt(m_ribbonBar->quickAccessBar(), row);
        } else {
            QString actionName = action->text();
            actionName.remove(QChar::fromLatin1('&'));
            item = new QListWidgetItem(actionName);
            item->setIcon(action->icon());
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            m_currentItemToAction.insert(item, action);
            m_actionToCurrentItem.insert(action, item);
        }
    } else {
        item = new QListWidgetItem(m_separatorText);
        m_currentItemToAction.insert(item, 0);
    }

    int row = m_pageUI.quickAccessBarList->count();
    if (currentToolBarAction)
        row = m_pageUI.quickAccessBarList->row(currentToolBarAction) + 1;

    m_pageUI.quickAccessBarList->insertItem(row, item);
    manager->insertAction(m_ribbonBar->quickAccessBar(), action, row);

    QListWidgetItem *itemCommand = m_pageUI.commandsList->currentItem();
    int index = m_pageUI.commandsList->row(itemCommand) + 1;
    m_pageUI.commandsList->setCurrentRow(index);

    m_pageUI.quickAccessBarList->setCurrentItem(item);

    setButtons();
}

void RibbonQuickAccessBarCustomizePagePrivate::removeClicked()
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    QListWidgetItem *currentToolBarAction = m_pageUI.quickAccessBarList->currentItem();
    if (!currentToolBarAction)
        return;

    int row = m_pageUI.quickAccessBarList->row(currentToolBarAction);
    manager->removeActionAt(m_ribbonBar->quickAccessBar(), row);

    QAction *action = m_currentItemToAction.value(currentToolBarAction);

    if (action)
        m_actionToCurrentItem.remove(action);

    m_currentItemToAction.remove(currentToolBarAction);
    delete currentToolBarAction;

    if (row == m_pageUI.quickAccessBarList->count())
        row--;

    if (row >= 0) {
        QListWidgetItem *item = m_pageUI.quickAccessBarList->item(row);
        m_pageUI.quickAccessBarList->setCurrentItem(item);
    }
    setButtons();
}

void RibbonQuickAccessBarCustomizePagePrivate::resetClicked()
{
    if (m_ribbonBar->quickAccessBar()) {
        RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
        Q_ASSERT(manager != Q_NULL);
        manager->reset(m_ribbonBar->quickAccessBar());
        fillStateCommands();
    }
}

void RibbonQuickAccessBarCustomizePagePrivate::upClicked()
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    QListWidgetItem *currentToolBarAction = m_pageUI.quickAccessBarList->currentItem();
    if (!currentToolBarAction)
        return;
    int row = m_pageUI.quickAccessBarList->row(currentToolBarAction);
    if (row == 0)
        return;
    m_pageUI.quickAccessBarList->takeItem(row);
    int newRow = row - 1;
    m_pageUI.quickAccessBarList->insertItem(newRow, currentToolBarAction);

    QAction *action = manager->actions(m_ribbonBar->quickAccessBar()).at(row);
    manager->removeActionAt(m_ribbonBar->quickAccessBar(), row);
    manager->insertAction(m_ribbonBar->quickAccessBar(), action, newRow);
    m_pageUI.quickAccessBarList->setCurrentItem(currentToolBarAction);
    setButtons();
}

void RibbonQuickAccessBarCustomizePagePrivate::downClicked()
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    QListWidgetItem *currentToolBarAction = m_pageUI.quickAccessBarList->currentItem();
    if (!currentToolBarAction)
        return;
    int row = m_pageUI.quickAccessBarList->row(currentToolBarAction);
    if (row == m_pageUI.quickAccessBarList->count() - 1)
        return;
    m_pageUI.quickAccessBarList->takeItem(row);
    int newRow = row + 1;
    m_pageUI.quickAccessBarList->insertItem(newRow, currentToolBarAction);
    QAction *action = manager->actions(m_ribbonBar->quickAccessBar()).at(row);
    manager->removeActionAt(m_ribbonBar->quickAccessBar(), row);
    manager->insertAction(m_ribbonBar->quickAccessBar(), action, newRow);
    m_pageUI.quickAccessBarList->setCurrentItem(currentToolBarAction);
    setButtons();
}

/*RibbonQuickAccessBarCustomizePage*/
RibbonQuickAccessBarCustomizePage::RibbonQuickAccessBarCustomizePage(RibbonBar *ribbonBar) : QWidget(Q_NULL)
{
    QTC_INIT_PRIVATE(RibbonQuickAccessBarCustomizePage);
    QTC_D(RibbonQuickAccessBarCustomizePage)
    d->m_ribbonBar = ribbonBar;
    d->init();
}

RibbonQuickAccessBarCustomizePage::~RibbonQuickAccessBarCustomizePage() { QTC_FINI_PRIVATE(); }

RibbonBar *RibbonQuickAccessBarCustomizePage::ribbonBar() const
{
    QTC_D(const RibbonQuickAccessBarCustomizePage)
    return d->m_ribbonBar;
}

void RibbonQuickAccessBarCustomizePage::addCustomCategory(const QString &strCategory)
{
    QTC_D(RibbonQuickAccessBarCustomizePage)
    QString str = strCategory;
    str.remove(QChar::fromLatin1('&'));
    d->m_sourceCategories.append(str);
}

void RibbonQuickAccessBarCustomizePage::addSeparatorCategory(const QString &strCategory)
{
    QTC_D(RibbonQuickAccessBarCustomizePage)

    QString separator = strCategory;
    if (separator.isEmpty())
        separator = QString("----------");

    d->m_strSeparator = separator;
    d->m_sourceCategories.append(separator);
}

void RibbonQuickAccessBarCustomizePage::accepted()
{
    QTC_D(RibbonQuickAccessBarCustomizePage)
    if (d->m_wasDisplayed)
        d->applyClicked();
}

void RibbonQuickAccessBarCustomizePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    QTC_D(RibbonQuickAccessBarCustomizePage)
    d->m_wasDisplayed = true;
    d->setupPage();
}

///////////////////////////////////////////////////////////////////////////////
/* RibbonBarCustomizePagePrivate */
RibbonBarCustomizePagePrivate::RibbonBarCustomizePagePrivate()
{
    m_ribbonBar = Q_NULL;
    m_currentSourceItemPage = Q_NULL;
    m_currentSourceItemGroup = Q_NULL;
    m_currentSourceAction = Q_NULL;
    m_currentItemPage = Q_NULL;
    m_currentItemGroup = Q_NULL;
    m_currentItemAction = Q_NULL;
    m_sufNameCustom = QString(" (Custom)");
    m_wasDisplayed = false;
}

RibbonBarCustomizePagePrivate::~RibbonBarCustomizePagePrivate() {}

/* RibbonCustomizeRibbonStyledItemDelegate */
class RibbonCustomizeRibbonStyledItemDelegate : public QStyledItemDelegate
{
public:
    explicit RibbonCustomizeRibbonStyledItemDelegate(QObject *parent = Q_NULL) : QStyledItemDelegate(parent) {}
    virtual ~RibbonCustomizeRibbonStyledItemDelegate() {}
    void addCaption(const QString &strCategory)
    {
        Q_ASSERT(!strCategory.isEmpty());
        m_lstCaption.append(strCategory);
    }
    void clearCaptions() { m_lstCaption.clear(); }
protected:
    virtual void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
    {
        QStyledItemDelegate::initStyleOption(option, index);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (m_lstCaption.contains(option->text))
            option->rect.setLeft(0);
#else
        if (const QStyleOptionViewItemV4 *vopt = qstyleoption_cast<QStyleOptionViewItemV4 *>(option))
            if (m_lstCaption.contains(vopt->text))
                option->rect.setLeft(0);
#endif   // QT_VERSION_CHECK(4, 0, 0)
    }
protected:
    QList<QString> m_lstCaption;
};

void RibbonBarCustomizePagePrivate::init()
{
    QTC_Q(RibbonBarCustomizePage)
    Q_ASSERT(m_ribbonBar != Q_NULL);
    m_pageUI.setupUi(q);

    QPixmap imagesSrc;
    imagesSrc = QPixmap(":/res/menuimg-pro24.png");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QImage img = imagesSrc.toImage();
    img.setAlphaChannel(img.createMaskFromColor(clrTransparent.rgb(), Qt::MaskOutColor));
    imagesSrc = QPixmap::fromImage(img);
#else
    imagesSrc.setAlphaChannel(imagesSrc.createMaskFromColor(clrTransparent, Qt::MaskOutColor));
#endif   // QT_VERSION_CHECK(4, 0, 0)
    m_pageUI.treeCommands->setItemDelegate(new RibbonCustomizeRibbonStyledItemDelegate(m_pageUI.treeCommands));
    m_pageUI.treeRibbon->setItemDelegate(new RibbonCustomizeRibbonStyledItemDelegate(m_pageUI.treeRibbon));
    m_pageUI.switchTabsBox->setVisible(false);

    QSize sz = QSize(9, 9);
    QPixmap m_imagesUpSrc =
        imagesSrc.copy(QRect(QPoint((int)OfficePaintManager::Icon_ArowUpLarge * sz.width(), 0), sz));
    QPixmap m_imagesDownSrc =
        imagesSrc.copy(QRect(QPoint((int)OfficePaintManager::Icon_ArowDownLarge * sz.width(), 0), sz));

    m_pageUI.upButton->setText("");
    m_pageUI.upButton->setIcon(m_imagesUpSrc);
    m_pageUI.downButton->setText("");
    m_pageUI.downButton->setIcon(m_imagesDownSrc);

    connect(m_pageUI.addButton, SIGNAL(released()), this, SLOT(addClicked()));
    connect(m_pageUI.newTabButton, SIGNAL(released()), this, SLOT(newTabClicked()));
    connect(m_pageUI.newGroupButton, SIGNAL(released()), this, SLOT(newGroupClicked()));
    connect(m_pageUI.removeButton, SIGNAL(released()), this, SLOT(removeClicked()));
    connect(m_pageUI.resetButton, SIGNAL(released()), this, SLOT(resetClicked()));
    connect(m_pageUI.upButton, SIGNAL(released()), this, SLOT(upClicked()));
    connect(m_pageUI.downButton, SIGNAL(released()), this, SLOT(downClicked()));
    connect(m_pageUI.renameButton, SIGNAL(released()), this, SLOT(renameClicked()));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    connect(m_pageUI.comboBoxSourceCategory, SIGNAL(currentTextChanged(const QString &)), this,
            SLOT(setCurrentCategoryText(const QString &)));
#else
    connect(m_pageUI.comboBoxSourceCategory, SIGNAL(currentIndexChanged(const QString &)), this,
            SLOT(setCurrentCategoryText(const QString &)));
#endif

    connect(m_pageUI.treeCommands, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this,
            SLOT(currentSourceChanged(QTreeWidgetItem *)));

    connect(m_pageUI.treeRibbon, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this,
            SLOT(currentPageChanged(QTreeWidgetItem *)));
    connect(m_pageUI.treeRibbon, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this,
            SLOT(itemPageChanged(QTreeWidgetItem *, int)));
}

void RibbonBarCustomizePagePrivate::setupPage()
{
    Q_ASSERT(m_ribbonBar != Q_NULL);

    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    if (!manager->isEditMode())
        manager->setEditMode();
    fillSourceCategories();
    setCurrentTabsText(QString());
    fillStateCategories();
}

void RibbonBarCustomizePagePrivate::setCategoryCaptionTree(QTreeWidget *tree, const QString &strCategory)
{
    QTC_Q(RibbonBarCustomizePage)
    if (!strCategory.isEmpty()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(tree, QStringList(strCategory));
        item->setDisabled(true);
        QFont font = q->font();
        font.setBold(true);
        item->setFont(0, font);
        item->setBackground(0, QColor(238, 238, 238));
        RibbonCustomizeRibbonStyledItemDelegate *ribbonStyledItemDelegate =
            dynamic_cast<RibbonCustomizeRibbonStyledItemDelegate *>(tree->itemDelegate());
        Q_ASSERT(ribbonStyledItemDelegate != Q_NULL);
        ribbonStyledItemDelegate->addCaption(strCategory);
    }
}

void RibbonBarCustomizePagePrivate::clearCategoryCaptionTree(QTreeWidget *tree)
{
    RibbonCustomizeRibbonStyledItemDelegate *ribbonStyledItemDelegate =
        dynamic_cast<RibbonCustomizeRibbonStyledItemDelegate *>(tree->itemDelegate());
    Q_ASSERT(ribbonStyledItemDelegate != Q_NULL);
    ribbonStyledItemDelegate->clearCaptions();
}

void RibbonBarCustomizePagePrivate::fillSourceCategories()
{
    m_pageUI.comboBoxSourceCategory->clear();
    m_pageUI.comboBoxSourceCategory->addItems(m_sourceCategories);
    if (m_sourceCategories.size() > 0)
        m_pageUI.comboBoxSourceCategory->setCurrentIndex(0);
}

void RibbonBarCustomizePagePrivate::fillSourceActions(QList<QAction *> &actions, QTreeWidgetItem *parentItem)
{
    QListIterator<QAction *> itAction(actions);
    QTreeWidgetItem *first = Q_NULL;

    while (itAction.hasNext()) {
        QAction *action = itAction.next();
        if (action == Q_NULL)
            continue;

        QString actionName = action->text();
        actionName.remove(QChar::fromLatin1('&'));

        if (!actionName.isEmpty()) {
            QTreeWidgetItem *item = Q_NULL;
            if (parentItem)
                item = new QTreeWidgetItem(parentItem, QStringList(actionName));
            else
                item = new QTreeWidgetItem(m_pageUI.treeCommands, QStringList(actionName));

            item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            QIcon icon = action->property("icon").value<QIcon>();
            item->setIcon(0, icon);
            m_currentSourceItemToAction.insert(item, action);

            if (!first)
                first = item;
        }
    }

    if (first)
        m_pageUI.treeCommands->setCurrentItem(first);
}

void RibbonBarCustomizePagePrivate::fillSourcePages(QList<RibbonPage *> &pages, const QString &strCategory)
{
    Q_UNUSED(strCategory);
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    QListIterator<RibbonPage *> itPage(pages);
    while (itPage.hasNext()) {
        RibbonPage *page = itPage.next();

        if (!manager->qtc_d()->m_ribbonManager->pageDefaultName(page).isEmpty()) {
            QString title = manager->qtc_d()->m_ribbonManager->pageDefaultName(page);
            title.remove(QChar::fromLatin1('&'));

            QTreeWidgetItem *item = new QTreeWidgetItem(m_pageUI.treeCommands, QStringList(title));
            fillSourceGroups(page, item);

            m_currentSourceItemToPage.insert(item, page);
        }
    }
}

void RibbonBarCustomizePagePrivate::fillSourceGroups(RibbonPage *page, QTreeWidgetItem *parentItem)
{
    Q_ASSERT(page != Q_NULL);
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    QList<RibbonGroup *> listGroup = manager->qtc_d()->m_ribbonManager->pageDefaultGroups(page);
    for (int i = 0, count = listGroup.count(); count > i; ++i) {
        QString groupTitle = manager->qtc_d()->m_ribbonManager->groupDefaultName(listGroup[i]);
        groupTitle.remove(QChar::fromLatin1('&'));
        if (!groupTitle.isEmpty()) {
            QTreeWidgetItem *newItem = new QTreeWidgetItem(parentItem, QStringList(groupTitle));
            fillSourceGroupActions(listGroup[i], newItem);
            m_currentSourceItemToGroup.insert(newItem, listGroup[i]);
        }
    }
}

void RibbonBarCustomizePagePrivate::fillSourceGroupActions(RibbonGroup *group, QTreeWidgetItem *parentItem)
{
    Q_ASSERT(group != Q_NULL);
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    QListIterator<QAction *> itAction(manager->actionsGroup(group));
    while (itAction.hasNext()) {
        QAction *action = itAction.next();

        QString actionName = action->text();
        actionName.remove(QChar::fromLatin1('&'));

        if (!actionName.isEmpty()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(parentItem, QStringList(actionName));
            item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            QIcon icon = action->property("icon").value<QIcon>();
            item->setIcon(0, icon);
            m_currentSourceItemToAction.insert(item, action);
        }
    }
}

void RibbonBarCustomizePagePrivate::fillStateCategories()
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    QStringList listCommands = manager->categories();

    QListIterator<QString> itAction(listCommands);
    QStringList list;
    while (itAction.hasNext()) {
        QString str = itAction.next();
        str.remove(QChar::fromLatin1('&'));
        list << str;
    }

    m_pageUI.switchTabsBox->addItems(list);
    if (listCommands.size() > 0)
        m_pageUI.switchTabsBox->setCurrentIndex(0);
}

void RibbonBarCustomizePagePrivate::fillStateRibbon(const QString &strCategory)
{
    Q_UNUSED(strCategory);
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    m_pageUI.treeRibbon->clear();
    m_pageToCurrentItem.clear();
    m_currentItemToPage.clear();
    m_currentItemToGroup.clear();

    QList<RibbonPage *> pages = manager->pages();
    QListIterator<RibbonPage *> itPages(pages);
    QTreeWidgetItem *first = Q_NULL;

    while (itPages.hasNext()) {
        if (RibbonPage *page = itPages.next()) {
            QString pageTitle = !manager->pageName(page).isEmpty() ? manager->pageName(page) : page->title();
            pageTitle.remove(QChar::fromLatin1('&'));

            if (!pageTitle.isEmpty()) {
                if (page->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Page_Custom"))
                    pageTitle += m_sufNameCustom;

                QStringList stringsPage;
                stringsPage.append(pageTitle);
                QTreeWidgetItem *item = new QTreeWidgetItem(m_pageUI.treeRibbon, stringsPage);

                if (manager->isPageHidden(page))
                    item->setCheckState(0, Qt::Unchecked);
                else
                    item->setCheckState(0, Qt::Checked);

                m_pageToCurrentItem.insert(page, item);
                m_currentItemToPage.insert(item, page);

                fillStateGroups(page, item);

                if (!first)
                    first = item;
            }
        }
    }

    if (first) {
        m_pageUI.treeRibbon->setCurrentItem(first);
        m_pageUI.treeRibbon->expandItem(first);
    }
}

void RibbonBarCustomizePagePrivate::fillStateGroups(RibbonPage *page, QTreeWidgetItem *itemParent)
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    QList<RibbonGroup *> groups = manager->pageGroups(page);
    QListIterator<RibbonGroup *> itGroups(groups);

    while (itGroups.hasNext()) {
        RibbonGroup *group = itGroups.next();
        QString groupTitle = !manager->groupName(group).isEmpty() ? manager->groupName(group) : group->title();

        groupTitle.remove(QChar::fromLatin1('&'));

        if (!groupTitle.isEmpty()) {
            if (group->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom"))
                groupTitle += m_sufNameCustom;

            QStringList stringsGroup;
            stringsGroup.append(groupTitle);
            QTreeWidgetItem *item = new QTreeWidgetItem(itemParent, stringsGroup);
            m_currentItemToGroup.insert(item, group);
            fillStateActions(group, item, false);
        }
    }
}

void RibbonBarCustomizePagePrivate::fillStateActions(RibbonGroup *group, QTreeWidgetItem *itemParent, bool newGroup)
{
    Q_ASSERT(group != Q_NULL);
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    for (int i = itemParent->childCount() - 1; i >= 0; --i) {
        QTreeWidgetItem *item = itemParent->takeChild(i);
        QAction *action = m_currentItemToAction.value(item);
        m_currentItemToAction.remove(item);
        m_actionToCurrentItem.remove(action);
        delete item;
    }

    bool isCustomGroup = group->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom");
    QListIterator<QAction *> itAction(newGroup ? group->actions() : manager->actionsGroup(group));
    while (itAction.hasNext()) {
        QAction *action = itAction.next();

        QString actionName = action->text();
        actionName.remove(QChar::fromLatin1('&'));

        if (!actionName.isEmpty()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(itemParent, QStringList(actionName));
            if (!isCustomGroup)
                item->setForeground(0, QColor(Qt::gray));

            item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            QIcon icon = action->property("icon").value<QIcon>();
            QPixmap px = icon.pixmap(QSize(16, 16), isCustomGroup ? QIcon::Normal : QIcon::Disabled);
            item->setIcon(0, QIcon(px));

            if (isCustomGroup) {
                m_currentItemToAction.insert(item, action);
                m_actionToCurrentItem.insert(action, item);
            }
        }
    }
    if (isCustomGroup)
        manager->appendActions(group, manager->actionsGroup(group));
}

RibbonPage *RibbonBarCustomizePagePrivate::addPageCustom(RibbonPage *srcPage)
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    QString strNewPage(RibbonBar::tr_compatible(RibbonNewPageString));
    if (srcPage) {
        strNewPage = srcPage->title();
        strNewPage.remove('&');
    }

    RibbonPage *findPage = m_currentItemToPage.value(m_currentItemPage);
    int index = manager->pageIndex(findPage) + 1;
    RibbonPage *newPage = manager->createPage(strNewPage, index);

    QTreeWidgetItem *item = new QTreeWidgetItem(m_pageUI.treeRibbon, m_currentItemPage);
    item->setCheckState(0, Qt::Checked);
    strNewPage += m_sufNameCustom;
    item->setText(0, strNewPage);

    m_pageToCurrentItem.insert(newPage, item);
    m_currentItemToPage.insert(item, newPage);
    m_pageUI.treeRibbon->setCurrentItem(item);
    return newPage;
}

void RibbonBarCustomizePagePrivate::addGroupCopy(RibbonPage *srcPage, RibbonPage *copyPage)
{
    if (!m_currentItemPage)
        return;

    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    QList<RibbonGroup *> srcGroups = srcPage->groups();
    for (int i = 0, count = srcGroups.count(); count > i; ++i) {
        RibbonGroup *srcGroup = srcGroups.at(i);
        if (srcGroup->property(__qtc_Widget_Copy).toString() != QLatin1String("__qtc_Group_Copy")) {
            QString titleGroup(srcGroup->title());
            RibbonGroup *newGroup = manager->createGroup(copyPage, titleGroup, i);
            newGroup->setIcon(srcGroup->icon());
            newGroup->setProperty(__qtc_Widget_Copy, QLatin1String("__qtc_Group_Copy"));

            titleGroup.remove(QChar::fromLatin1('&'));
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(titleGroup));
            m_currentItemPage->insertChild(i, item);

            m_currentItemToGroup.insert(item, newGroup);

            QListIterator<QAction *> itAction(srcGroup->actions());
            while (itAction.hasNext()) {
                QAction *action = itAction.next();
                newGroup->addAction(action);
            }
            fillStateActions(newGroup, item, true);
        }
    }
}

void RibbonBarCustomizePagePrivate::addActionGroupCustom(RibbonGroup *group, QAction *action, int index)
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    QString actionName = action->text();
    actionName.remove(QChar::fromLatin1('&'));

    if (!actionName.isEmpty()) {
        QTreeWidgetItem *item = Q_NULL;
        if (index == -1)
            item = new QTreeWidgetItem(m_currentItemGroup, QStringList(actionName));
        else {
            item = new QTreeWidgetItem(QStringList(actionName));
            m_currentItemGroup->insertChild(index, item);
        }
        item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
        QIcon icon = action->property("icon").value<QIcon>();
        QPixmap px = icon.pixmap(QSize(16, 16), QIcon::Normal);
        item->setIcon(0, QIcon(px));

        m_currentItemToAction.insert(item, action);
        m_actionToCurrentItem.insert(action, item);
        manager->insertAction(group, action, index);
    }
}

void RibbonBarCustomizePagePrivate::setButtons()
{
    bool addEnabled = false;
    bool removeEnabled = false;
    bool renameEnabled = false;

    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    if (m_currentSourceAction) {
        if (RibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup)) {
            if (QAction *action = m_currentSourceItemToAction.value(m_currentSourceAction))
                addEnabled = !manager->containsAction(group, action);
        }
    } else if (!m_currentSourceItemGroup)
        addEnabled = true;
    else if (m_currentSourceItemGroup) {
        RibbonPage *currentSourcePage = m_currentSourceItemToPage.value(m_currentSourceItemPage);
        RibbonPage *currentPage = m_currentItemToPage.value(m_currentItemPage);
        if (currentSourcePage && currentPage) {
            if (currentSourcePage != currentPage) {
                RibbonGroup *group = m_currentSourceItemToGroup.value(m_currentSourceItemGroup);
                QList<QAction *> listActions = manager->actionsGroup(group);   // group->actions();

                QList<RibbonGroup *> listGroup = manager->pageGroups(currentPage);
                QListIterator<RibbonGroup *> itGroup(listGroup);
                bool find = false;
                while (itGroup.hasNext()) {
                    RibbonGroup *currentGroup = itGroup.next();
                    QList<QAction *> currentListActions = currentGroup->actions();

                    if (listActions == currentListActions) {
                        find = true;
                        break;
                    }
                }
                if (!find)
                    addEnabled = true;
            }
        }
    }

    if (m_currentItemAction && m_currentItemGroup) {
        RibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup);
        if (group && group->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom"))
            removeEnabled = true;
    } else if (m_currentItemGroup) {
        renameEnabled = true;
        removeEnabled = true;
    } else if (m_currentItemPage) {
        renameEnabled = true;
        RibbonPage *currentPage = m_currentItemToPage.value(m_currentItemPage);
        if (currentPage && currentPage->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Page_Custom"))
            removeEnabled = true;
    }

    m_pageUI.addButton->setEnabled(addEnabled);
    m_pageUI.removeButton->setEnabled(removeEnabled);
    m_pageUI.renameButton->setEnabled(renameEnabled);

    bool upEnabled = false;
    bool downEnabled = false;

    if (m_currentItemPage && m_currentItemGroup) {
        RibbonPage *currentPage = m_currentItemToPage.value(m_currentItemPage);
        Q_ASSERT(currentPage != Q_NULL);
        RibbonGroup *currentGroup = m_currentItemToGroup.value(m_currentItemGroup);
        Q_ASSERT(currentGroup != Q_NULL);

        QList<RibbonGroup *> groups = manager->pageGroups(currentPage);

        int index = groups.indexOf(currentGroup);
        upEnabled = index != 0;
        downEnabled = index < (groups.size() - 1);

        if (m_currentItemAction) {
            if (QAction *action = m_currentItemToAction.value(m_currentItemAction)) {
                QList<QAction *> actions = manager->actionsGroup(currentGroup);
                index = actions.indexOf(action);
                upEnabled = index != 0;
                downEnabled = index < (actions.size() - 1);
            }
        }
    } else if (m_currentItemPage && !m_currentItemGroup) {
        int index = m_pageUI.treeRibbon->indexOfTopLevelItem(m_pageUI.treeRibbon->currentItem());
        upEnabled = index != 0;
        downEnabled = index < m_pageUI.treeRibbon->topLevelItemCount() - 1;
    }

    m_pageUI.upButton->setEnabled(upEnabled);
    m_pageUI.downButton->setEnabled(downEnabled);
}

void RibbonBarCustomizePagePrivate::applyClicked()
{
    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);
    manager->commit();
}

void RibbonBarCustomizePagePrivate::cancelClicked() { m_ribbonBar->customizeManager()->cancel(); }

void RibbonBarCustomizePagePrivate::addClicked()
{
    QTC_Q(RibbonBarCustomizePage)

    if (m_currentSourceAction && m_currentItemGroup) {
        RibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup);
        if (group && group->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom")) {
            if (QAction *action = m_currentSourceItemToAction.value(m_currentSourceAction)) {
                int index = m_currentItemGroup->indexOfChild(m_currentItemAction);
                addActionGroupCustom(group, action, index + 1);

                m_currentItemAction = m_currentItemGroup->child(index + 1);

                if (m_currentItemAction)
                    m_pageUI.treeRibbon->setCurrentItem(m_currentItemAction);
                else {
                    QTreeWidgetItem *currentItem = m_currentItemGroup->child(m_currentItemGroup->childCount() - 1);
                    m_pageUI.treeRibbon->setCurrentItem(currentItem);
                }

                if (m_currentSourceItemGroup && m_currentSourceAction) {
                    int indexNext = m_currentSourceItemGroup->indexOfChild(m_currentSourceAction) + 1;
                    QTreeWidgetItem *nextItem = m_currentSourceItemGroup->child(indexNext);
                    nextItem = nextItem ? nextItem : m_currentSourceAction;
                    m_pageUI.treeCommands->setCurrentItem(nextItem);
                } else if (m_currentSourceAction) {
                    int index = m_pageUI.treeCommands->indexOfTopLevelItem(m_currentSourceAction);
                    QTreeWidgetItem *item = m_pageUI.treeCommands->topLevelItem(index + 1);
                    item = item ? item : m_currentSourceAction;
                    m_pageUI.treeCommands->setCurrentItem(item);
                }
                m_pageUI.treeRibbon->expandItem(m_currentItemGroup);
            }
        } else {
            QMessageBox::warning(q, q->windowTitle(), RibbonBar::tr_compatible(RibbonAddCommandWarningString));
        }
    } else if (!m_currentSourceItemGroup) {
        if (RibbonPage *page = m_currentSourceItemToPage.value(m_currentSourceItemPage)) {
            int index = m_pageUI.treeCommands->indexOfTopLevelItem(m_currentSourceItemPage);
            QTreeWidgetItem *item = m_pageUI.treeCommands->topLevelItem(index + 1);
            item = item ? item : m_currentSourceItemPage;
            m_pageUI.treeCommands->setCurrentItem(item);

            RibbonPage *newPage = addPageCustom(page);
            if (newPage)
                addGroupCopy(page, newPage);
        }
    } else if (m_currentSourceItemGroup) {
        RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
        Q_ASSERT(manager != Q_NULL);

        RibbonGroup *group = m_currentSourceItemToGroup.value(m_currentSourceItemGroup);
        RibbonPage *page = m_currentItemToPage.value(m_currentItemPage);

        if (group != Q_NULL && page != Q_NULL) {
            QTreeWidgetItem *itemParent = m_currentItemPage;
            int index = itemParent->childCount();
            QString titleGroup(group->title());
            RibbonGroup *newGroup = manager->createGroup(page, titleGroup, index);
            newGroup->setIcon(group->icon());
            newGroup->setProperty(__qtc_Widget_Copy, QLatin1String("__qtc_Group_Copy"));

            titleGroup.remove(QChar::fromLatin1('&'));
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(titleGroup));
            m_currentItemPage->insertChild(index, item);

            m_currentItemToGroup.insert(item, newGroup);

            QListIterator<QAction *> itAction(manager->actionsGroup(group));
            while (itAction.hasNext()) {
                QAction *action = itAction.next();
                newGroup->addAction(action);
            }
            fillStateActions(newGroup, item, true);

            int indexNext = m_currentSourceItemPage->indexOfChild(m_currentSourceItemGroup) + 1;
            QTreeWidgetItem *nextItem = m_currentSourceItemPage->child(indexNext);
            nextItem = nextItem ? nextItem : m_currentSourceItemGroup;
            m_pageUI.treeCommands->setCurrentItem(nextItem);
            m_pageUI.treeRibbon->setCurrentItem(item);
        }
    }
    setButtons();
}

void RibbonBarCustomizePagePrivate::newTabClicked()
{
    if (!m_currentItemPage)
        return;

    addPageCustom(Q_NULL);
    newGroupClicked();
    setButtons();
}

void RibbonBarCustomizePagePrivate::newGroupClicked()
{
    if (!m_currentItemPage)
        return;

    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    QString strNewGroup(RibbonBar::tr_compatible(RibbonNewGroupString));
    RibbonPage *currentPage = m_currentItemToPage.value(m_currentItemPage);
    Q_ASSERT(currentPage != Q_NULL);

    int index = 0;
    if (m_currentItemGroup) {
        RibbonGroup *currentGroup = m_currentItemToGroup.value(m_currentItemGroup);
        Q_ASSERT(currentGroup != Q_NULL);

        QList<RibbonGroup *> groups = manager->pageGroups(currentPage);
        index = groups.indexOf(currentGroup) + 1;
    }

    RibbonGroup *newGroup = manager->createGroup(currentPage, strNewGroup, index);

    QStringList stringsGroup;
    strNewGroup += m_sufNameCustom;
    stringsGroup.append(strNewGroup);
    QTreeWidgetItem *item = new QTreeWidgetItem(stringsGroup);
    m_currentItemPage->insertChild(index, item);
    newGroup->setProperty(__qtc_Widget_Custom, QLatin1String("__qtc_Group_Custom"));
    m_currentItemToGroup.insert(item, newGroup);
    m_pageUI.treeRibbon->setCurrentItem(item);
    setButtons();
}

void RibbonBarCustomizePagePrivate::removeClicked()
{
    QTreeWidgetItem *currentItem = m_pageUI.treeRibbon->currentItem();
    if (!currentItem)
        return;

    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    if (m_currentItemGroup && m_currentItemAction) {
        if (RibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup)) {
            if (group && group->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom")) {
                QTreeWidgetItem *currentParent = currentItem->parent();
                int index = currentParent->indexOfChild(currentItem);
                if (index == -1)
                    return;

                if (QAction *action = m_currentItemToAction.value(m_currentItemAction)) {
                    m_actionToCurrentItem.remove(action);
                    m_currentItemToAction.remove(currentItem);
                    manager->removeActionAt(group, index);
                }
                delete currentItem;
            }
        }
    } else if (RibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup)) {
        QTreeWidgetItem *currentParent = currentItem->parent();
        int index = currentParent->indexOfChild(currentItem);
        if (index == -1)
            return;
        RibbonPage *page = m_currentItemToPage.value(m_currentItemPage);
        Q_ASSERT(page != Q_NULL);

        manager->clearActions(group);
        manager->deleteGroup(page, index);
        m_currentItemToGroup.remove(currentItem);
        delete currentItem;
    } else if (RibbonPage *page = m_currentItemToPage.value(m_currentItemPage)) {
        int index = m_pageUI.treeRibbon->indexOfTopLevelItem(currentItem);
        //        manager->removePage(page);
        manager->deletePage(page);
        m_pageToCurrentItem.remove(page);
        QListIterator<RibbonGroup *> itGroup(manager->pageGroups(page));
        while (itGroup.hasNext())
            manager->clearActions(itGroup.next());

        m_currentItemToPage.remove(currentItem);
        delete currentItem;

        if (index == m_pageUI.treeRibbon->topLevelItemCount())
            index--;

        if (index >= 0) {
            QTreeWidgetItem *item = m_pageUI.treeRibbon->topLevelItem(index);
            m_pageUI.treeRibbon->setCurrentItem(item);
        }
    }
    setButtons();
}

void RibbonBarCustomizePagePrivate::resetClicked()
{
    if (m_ribbonBar) {
        RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
        Q_ASSERT(manager != NULL);
        manager->reset();
        fillStateRibbon(m_pageUI.switchTabsBox->currentText());
    }
}

template <class T>
QTreeWidgetItem *currentItem(const QMap<QTreeWidgetItem *, T *> &currentItemToObject, QTreeWidgetItem *current)
{
    if (current == Q_NULL)
        return Q_NULL;

    if (currentItemToObject.contains(current))
        return current;

    return currentItem(currentItemToObject, current->parent());
}

void RibbonBarCustomizePagePrivate::upClicked()
{
    QTreeWidgetItem *currentItem = m_pageUI.treeRibbon->currentItem();
    if (!currentItem)
        return;

    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    if (m_currentItemGroup && m_currentItemAction) {
        QTreeWidgetItem *currentParent = currentItem->parent();
        int index = currentParent->indexOfChild(currentItem);
        if (!(index == -1 || index == 0)) {
            currentParent->takeChild(index);
            int newIndex = index - 1;
            currentParent->insertChild(newIndex, currentItem);
            Q_ASSERT(m_currentItemPage != Q_NULL);
            QAction *action = m_currentItemToAction.value(currentItem);
            Q_ASSERT(action != Q_NULL);

            RibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup);
            Q_ASSERT(group != Q_NULL);
            manager->removeActionAt(group, index);
            manager->insertAction(group, action, newIndex);

            m_pageUI.treeRibbon->setCurrentItem(currentItem);
        }
    } else if (m_currentItemToPage.contains(currentItem)) {
        int index = m_pageUI.treeRibbon->indexOfTopLevelItem(currentItem);
        if (index != -1) {
            m_pageUI.treeRibbon->takeTopLevelItem(index);
            int newIndex = index - 1;
            m_pageUI.treeRibbon->insertTopLevelItem(newIndex, currentItem);
            manager->movePage(index, newIndex);
            m_pageUI.treeRibbon->setCurrentItem(currentItem);
        }
    } else if (m_currentItemToGroup.contains(currentItem)) {
        QTreeWidgetItem *currentParent = currentItem->parent();
        int index = currentParent->indexOfChild(currentItem);
        if (!(index == -1 || index == 0)) {
            currentParent->takeChild(index);
            int newIndex = index - 1;
            currentParent->insertChild(newIndex, currentItem);
            Q_ASSERT(m_currentItemPage != Q_NULL);
            RibbonPage *page = m_currentItemToPage.value(m_currentItemPage);
            Q_ASSERT(page != Q_NULL);
            manager->moveGroup(page, index, newIndex);
            m_pageUI.treeRibbon->setCurrentItem(currentItem);
        }
    }

    setButtons();
}

void RibbonBarCustomizePagePrivate::downClicked()
{
    QTreeWidgetItem *currentItem = m_pageUI.treeRibbon->currentItem();
    if (!currentItem)
        return;

    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    if (m_currentItemGroup && m_currentItemAction) {
        QTreeWidgetItem *currentParent = currentItem->parent();
        int index = currentParent->indexOfChild(currentItem);
        if (index != currentParent->childCount() - 1) {
            currentParent->takeChild(index);
            int newIndex = index + 1;
            currentParent->insertChild(newIndex, currentItem);
            Q_ASSERT(m_currentItemPage != Q_NULL);
            QAction *action = m_currentItemToAction.value(currentItem);
            Q_ASSERT(action != Q_NULL);

            RibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup);
            Q_ASSERT(group != Q_NULL);
            manager->removeActionAt(group, index);
            manager->insertAction(group, action, newIndex);
        }
    } else if (m_currentItemToPage.contains(currentItem)) {
        int index = m_pageUI.treeRibbon->indexOfTopLevelItem(currentItem);
        if (index != m_pageUI.treeRibbon->topLevelItemCount() - 1) {
            m_pageUI.treeRibbon->takeTopLevelItem(index);
            int newIndex = index + 1;
            m_pageUI.treeRibbon->insertTopLevelItem(newIndex, currentItem);
            manager->movePage(index, newIndex);
        }
    } else if (m_currentItemToGroup.contains(currentItem)) {
        QTreeWidgetItem *currentParent = currentItem->parent();
        int index = currentParent->indexOfChild(currentItem);
        if (index != currentParent->childCount() - 1) {
            currentParent->takeChild(index);
            int newIndex = index + 1;
            currentParent->insertChild(newIndex, currentItem);
            Q_ASSERT(m_currentItemPage != Q_NULL);
            RibbonPage *page = m_currentItemToPage.value(m_currentItemPage);
            Q_ASSERT(page != Q_NULL);
            manager->moveGroup(page, index, newIndex);
        }
    }
    m_pageUI.treeRibbon->setCurrentItem(currentItem);
    setButtons();
}

void RibbonBarCustomizePagePrivate::renameClicked()
{
    QTC_Q(RibbonBarCustomizePage);
    class RenameDialog : public QDialog
    {
    public:
        RenameDialog(QWidget *parent, const QString &currentName) : QDialog(parent)
        {
            setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
            m_renameDialogUI.setupUi(this);
            m_renameDialogUI.lineEditName->setText(currentName);
        }
        QString currentName() const { return m_renameDialogUI.lineEditName->text(); }
    protected:
        Ui::RibbonRenameDialog m_renameDialogUI;
    };

    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    QTreeWidgetItem *currentItem = m_pageUI.treeRibbon->currentItem();
    if (!currentItem)
        return;

    QString currentName;
    RibbonPage *page = m_currentItemToPage.value(currentItem);
    RibbonGroup *group = m_currentItemToGroup.value(currentItem);
    if (page) {
        if (!manager->pageName(page).isEmpty())
            currentName = manager->pageName(page);
        else
            currentName = page->title();
    } else if (group) {
        if (!manager->groupName(group).isEmpty())
            currentName = manager->groupName(group);
        else
            currentName = group->title();
    }

    if (!currentName.isEmpty()) {
        RenameDialog renameDialog(q, currentName);
        if (renameDialog.exec() == QDialog::Accepted) {
            QString currentText = renameDialog.currentName();
            if (page != Q_NULL) {
                manager->setPageName(page, currentText);
                currentText.remove(QChar::fromLatin1('&'));
                if (page->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Page_Custom"))
                    currentText += m_sufNameCustom;
                currentItem->setText(0, currentText);
            } else if (group != Q_NULL) {
                manager->setGroupName(group, currentText);
                currentText.remove(QChar::fromLatin1('&'));
                if (group->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom"))
                    currentText += m_sufNameCustom;
                currentItem->setText(0, currentText);
            }
        }
    }
}

void RibbonBarCustomizePagePrivate::setCurrentCategoryText(const QString &strCategory)
{
    if (strCategory.isEmpty())
        return;

    if (m_strSeparator == strCategory) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        m_pageUI.comboBoxSourceCategory->setCurrentText(m_currentSourceCategory);
#else
        int index = m_pageUI.comboBoxSourceCategory->findText(m_currentSourceCategory);
        if (index != -1)
            m_pageUI.comboBoxSourceCategory->setCurrentIndex(index);
#endif
        return;
    }

    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    m_currentSourceItemToPage.clear();
    m_currentSourceItemToGroup.clear();
    m_currentSourceItemToAction.clear();

    m_pageUI.treeCommands->clear();
    clearCategoryCaptionTree(m_pageUI.treeCommands);

    QList<QAction *> actions = manager->actionsByCategory(strCategory);
    if (actions.size() > 0) {
        m_pageUI.treeCommands->setRootIsDecorated(false);
        fillSourceActions(actions);
    } else {
        QList<RibbonPage *> pages = manager->pagesByCategory(strCategory);
        if (pages.size() > 0) {
            m_pageUI.treeCommands->setRootIsDecorated(true);
            fillSourcePages(pages, strCategory);
        }
    }
    m_currentSourceCategory = strCategory;
    setButtons();
}

void RibbonBarCustomizePagePrivate::setCurrentTabsText(const QString &strCategory)
{
    // if (strCategory.isEmpty())
    //     return;
    fillStateRibbon(strCategory);
}

void RibbonBarCustomizePagePrivate::currentSourceChanged(QTreeWidgetItem *current)
{
    m_currentSourceItemPage = ::currentItem(m_currentSourceItemToPage, current);
    m_currentSourceItemGroup = ::currentItem(m_currentSourceItemToGroup, current);
    m_currentSourceAction = ::currentItem(m_currentSourceItemToAction, current);
    setButtons();
}

void RibbonBarCustomizePagePrivate::currentPageChanged(QTreeWidgetItem *current)
{
    m_currentItemPage = ::currentItem(m_currentItemToPage, current);
    m_currentItemGroup = ::currentItem(m_currentItemToGroup, current);
    m_currentItemAction = (m_currentItemGroup && m_currentItemGroup != current) ? current : Q_NULL;
    setButtons();
}

void RibbonBarCustomizePagePrivate::itemPageChanged(QTreeWidgetItem *itemPage, int column)
{
    if (m_currentItemToPage.empty())
        return;

    RibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != Q_NULL);

    RibbonPage *page = m_currentItemToPage.value(itemPage);
    if (page == Q_NULL)
        return;

    if (itemPage->checkState(column) == Qt::Checked) {
        if (manager->isPageHidden(page))
            m_ribbonBar->customizeManager()->setPageHidden(page, false);
    } else if (itemPage->checkState(column) == Qt::Unchecked) {
        if (!manager->isPageHidden(page))
            m_ribbonBar->customizeManager()->setPageHidden(page, true);
    }
}

/* RibbonBarCustomizePage */
RibbonBarCustomizePage::RibbonBarCustomizePage(RibbonBar *ribbonBar) : QWidget(Q_NULL)
{
    QTC_INIT_PRIVATE(RibbonBarCustomizePage);
    QTC_D(RibbonBarCustomizePage)
    d->m_ribbonBar = ribbonBar;
    d->init();
}

RibbonBarCustomizePage::~RibbonBarCustomizePage() { QTC_FINI_PRIVATE(); }

RibbonBar *RibbonBarCustomizePage::ribbonBar() const
{
    QTC_D(const RibbonBarCustomizePage)
    return d->m_ribbonBar;
}

void RibbonBarCustomizePage::addCustomCategory(const QString &strCategory)
{
    QTC_D(RibbonBarCustomizePage)
    d->m_sourceCategories.append(strCategory);
}

void RibbonBarCustomizePage::addSeparatorCategory(const QString &strCategory)
{
    QTC_D(RibbonBarCustomizePage)

    QString separator = strCategory;
    if (separator.isEmpty())
        separator = QString("----------");

    d->m_strSeparator = separator;
    d->m_sourceCategories.append(separator);
}

void RibbonBarCustomizePage::accepted()
{
    QTC_D(RibbonBarCustomizePage)
    if (d->m_wasDisplayed)
        d->applyClicked();
}

void RibbonBarCustomizePage::rejected()
{
    QTC_D(RibbonBarCustomizePage)
    d->cancelClicked();
}

void RibbonBarCustomizePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    QTC_D(RibbonBarCustomizePage)
    d->m_wasDisplayed = true;
    d->setupPage();
}

void RibbonBarCustomizePage::hideEvent(QHideEvent *event) { QWidget::hideEvent(event); }
