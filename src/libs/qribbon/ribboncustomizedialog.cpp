/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribboncustomizedialog.h"
#include "ribbon_def.h"
#include "ribbonbar_p.h"
#include "ribboncustomizedialog_p.h"
#include "ribboncustomizemanager.h"
#include "ribboncustomizepage.h"
#include <QApplication>
#include <QPainter>
#include <QVBoxLayout>

QRIBBON_USE_NAMESPACE

QRIBBON_BEGIN_NAMESPACE

static QWidget *qrn_WantTheFocus(QWidget *ancestor)
{
    const int MaxIterations = 100;

    QWidget *candidate = ancestor;
    for (int i = 0; i < MaxIterations; ++i) {
        candidate = candidate->nextInFocusChain();
        if (!candidate)
            break;

        if (candidate->focusPolicy() & Qt::TabFocus) {
            if (candidate != ancestor && ancestor->isAncestorOf(candidate))
                return candidate;
        }
    }
    return 0;
}

/* RibbonCustomizePageWidget */
class RibbonCustomizePageWidget : public QWidget
{
public:
    explicit RibbonCustomizePageWidget(QWidget *parent);
    virtual ~RibbonCustomizePageWidget();
public:
    QWidget *content() const;
    void setContent(QWidget *content);
protected:
    QHBoxLayout *m_hBoxLayout;
    QVBoxLayout *m_vBoxLayout;
    QLabel m_labelIcon;
    QLabel m_labelTitle;
    QWidget *m_content;
private:
    friend class RibbonCustomizeDialog;
    Q_DISABLE_COPY(RibbonCustomizePageWidget)
};

QRIBBON_END_NAMESPACE

/* RibbonCustomizePageWidget */
RibbonCustomizePageWidget::RibbonCustomizePageWidget(QWidget *parent) : QWidget(parent)
{
    m_content = Q_NULL;
    QFont font = m_labelTitle.font();
    font.setBold(true);
    m_labelTitle.setFont(font);
    m_hBoxLayout = new QHBoxLayout();
    m_hBoxLayout->addWidget(&m_labelIcon);
    m_hBoxLayout->addWidget(&m_labelTitle, 1);
    m_vBoxLayout = new QVBoxLayout(this);
    m_vBoxLayout->addLayout(m_hBoxLayout);
}

RibbonCustomizePageWidget::~RibbonCustomizePageWidget() {}

QWidget *RibbonCustomizePageWidget::content() const { return m_content; }

void RibbonCustomizePageWidget::setContent(QWidget *content)
{
    if (content == Q_NULL) {
        Q_ASSERT(false);
        return;
    }
    QIcon icon = content->windowIcon();
    m_labelIcon.setPixmap(icon.isNull() ? QPixmap(":/res/logo.png") : icon.pixmap(QSize(32, 32)));
    m_labelTitle.setText(content->statusTip());
    m_vBoxLayout->addWidget(content);
    m_content = content;
}

/* RibbonCustomizeListWidget */
RibbonCustomizeListWidget::RibbonCustomizeListWidget(QWidget *parent) : QListWidget(parent) {}

RibbonCustomizeListWidget::~RibbonCustomizeListWidget() {}

QSize RibbonCustomizeListWidget::sizeHint() const
{
    QSize sz = QListWidget::sizeHint();
    QStyleOption opt;
    opt.init(this);

    int maxWidth = 0;
    for (int row = 0, countRow = count(); countRow > row; row++) {
        if (QListWidgetItem *rbItem = item(row)) {
            QString str = rbItem->text() + QString("XX");
            int width = opt.fontMetrics.boundingRect(str).width();
            maxWidth = qMax(maxWidth, width);
        }
    }
    return QSize(maxWidth, sz.height());
}

QSize RibbonCustomizeListWidget::minimumSizeHint() const
{
    QSize sz = QListWidget::minimumSizeHint();
    sz.setWidth(sizeHint().width());
    return sz;
}

/* RibbonCustomizeDialogPrivate */
RibbonCustomizeDialogPrivate::RibbonCustomizeDialogPrivate()
{
    m_currentPage = 0;
    m_pageVBoxLayout = Q_NULL;
}

RibbonCustomizeDialogPrivate::~RibbonCustomizeDialogPrivate() {}

void RibbonCustomizeDialogPrivate::init()
{
    Q_Q(RibbonCustomizeDialog);
    q->setWindowTitle(RibbonBar::tr_compatible(RibbonCustomizeDialogOptionsString));

    q->resize(734, 465);
    m_verticalLayout = new QVBoxLayout(q);
    m_horizontalLayout = new QHBoxLayout();
    m_horizontalLayout->setSpacing(1);
    m_listPage = new RibbonCustomizeListWidget(q);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_listPage->sizePolicy().hasHeightForWidth());
    m_listPage->setSizePolicy(sizePolicy);
    m_listPage->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_horizontalLayout->addWidget(m_listPage, 0, Qt::AlignLeft);

    m_pageFrame = new QFrame(q);
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(m_pageFrame->sizePolicy().hasHeightForWidth());
    m_pageFrame->setSizePolicy(sizePolicy1);
    m_pageFrame->setAutoFillBackground(false);
    m_pageFrame->setFrameShape(QFrame::StyledPanel);
    m_pageFrame->setFrameShadow(QFrame::Sunken);

    m_horizontalLayout->addWidget(m_pageFrame);

    m_horizontalLayout->setStretch(1, 1);

    m_verticalLayout->addLayout(m_horizontalLayout);

    m_buttonBox = new QDialogButtonBox(q);
    m_buttonBox->setOrientation(Qt::Horizontal);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

    m_verticalLayout->addWidget(m_buttonBox);

    QObject::connect(m_buttonBox, SIGNAL(accepted()), q, SLOT(accept()), Qt::DirectConnection);
    QObject::connect(m_buttonBox, SIGNAL(rejected()), q, SLOT(reject()), Qt::DirectConnection);

    m_verticalLayout->setContentsMargins(2, 2, 2, 6);

    m_pageVBoxLayout = new QVBoxLayout(m_pageFrame);
    m_pageVBoxLayout->setSpacing(0);
    m_pageVBoxLayout->addSpacing(0);
    QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
    m_pageVBoxLayout->addItem(spacerItem);
    connect(m_listPage, SIGNAL(currentRowChanged(int)), this, SLOT(switchToPage(int)));
}

void RibbonCustomizeDialogPrivate::setCustomizeMode(bool edit)
{
    for (int index = 0, count = m_listWidget.count(); count > index; ++index) {
        if (RibbonBarCustomizePage *page = qobject_cast<RibbonBarCustomizePage *>(m_listWidget.at(index))) {
            page->ribbonBar()->customizeManager()->setEditMode(edit);
            break;
        } else if (RibbonQuickAccessBarCustomizePage *page =
                       qobject_cast<RibbonQuickAccessBarCustomizePage *>(m_listWidget.at(index))) {
            page->ribbonBar()->customizeManager()->setEditMode(edit);
            break;
        }
    }
}

void RibbonCustomizeDialogPrivate::addPage(RibbonCustomizePageWidget *page)
{
    if (!page) {
        qWarning("RibbonCustomizeDialogPrivate::addPage: Cannot insert null page");
        return;
    }
    page->setParent(m_pageFrame);

    int n = m_pageVBoxLayout->count();
    // disable layout to prevent layout updates while adding
    bool pageVBoxLayoutEnabled = m_pageVBoxLayout->isEnabled();
    m_pageVBoxLayout->setEnabled(false);

    m_pageVBoxLayout->insertWidget(n - 1, page);

    // hide new page and reset layout to old status
    page->hide();
    m_pageVBoxLayout->setEnabled(pageVBoxLayoutEnabled);
    RibbonCustomizePageItem *item = new RibbonCustomizePageItem(page->windowTitle(), page);
    m_listPage->addItem(item);
}

void RibbonCustomizeDialogPrivate::insertPage(int index, RibbonCustomizePageWidget *page)
{
    if (!page) {
        qWarning("RibbonCustomizeDialogPrivate::insertPage: Cannot insert null page");
        return;
    }

    RibbonCustomizePageItem *posItem = dynamic_cast<RibbonCustomizePageItem *>(m_listPage->item(index));
    int indexItem = m_listPage->row(posItem);
    if (indexItem == -1)
        return;

    page->setParent(m_pageFrame);

    int n = m_pageVBoxLayout->count();
    // disable layout to prevent layout updates while adding
    bool pageVBoxLayoutEnabled = m_pageVBoxLayout->isEnabled();
    m_pageVBoxLayout->setEnabled(false);

    m_pageVBoxLayout->insertWidget(n - 1, page);

    // hide new page and reset layout to old status
    page->hide();
    m_pageVBoxLayout->setEnabled(pageVBoxLayoutEnabled);

    RibbonCustomizePageItem *item = new RibbonCustomizePageItem(page->windowTitle(), page);
    m_listPage->insertItem(indexItem, item);
}

void RibbonCustomizeDialogPrivate::switchToPage(int currentRow)
{
    if (m_currentPage != currentRow) {
        if (RibbonCustomizePageItem *item = dynamic_cast<RibbonCustomizePageItem *>(m_listPage->item(m_currentPage))) {
            RibbonCustomizePageWidget *page = item->m_page;
            page->setUpdatesEnabled(false);
            page->hide();
            page->parentWidget()->repaint();
        }
    }

    m_currentPage = currentRow;

    if (RibbonCustomizePageItem *item = dynamic_cast<RibbonCustomizePageItem *>(m_listPage->item(m_currentPage))) {
        RibbonCustomizePageWidget *page = item->m_page;
        page->setUpdatesEnabled(true);
        page->show();

        if (QWidget *candidate = qrn_WantTheFocus(page))
            candidate->setFocus();

        bool expandPage = !page->layout();
        if (!expandPage) {
            const QLayoutItem *pageItem = m_pageVBoxLayout->itemAt(m_pageVBoxLayout->indexOf(page));
            expandPage = pageItem->expandingDirections() & Qt::Vertical;
        }
        QSpacerItem *bottomSpacer = m_pageVBoxLayout->itemAt(m_pageVBoxLayout->count() - 1)->spacerItem();
        Q_ASSERT(bottomSpacer);
        bottomSpacer->changeSize(0, 0, QSizePolicy::Ignored,
                                 expandPage ? QSizePolicy::Ignored : QSizePolicy::MinimumExpanding);
        m_pageVBoxLayout->invalidate();
        page->parentWidget()->repaint();
    }
}

/*!
\class RibbonCustomizeDialog
\inmodule ribbon
RibbonCustomizeDialog it a widget to configure actions on the RibbonBar
*/
RibbonCustomizeDialog::RibbonCustomizeDialog(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QRN_INIT_PRIVATE(RibbonCustomizeDialog);
    Q_D(RibbonCustomizeDialog);
    d->init();
}

RibbonCustomizeDialog::~RibbonCustomizeDialog() { QRN_FINI_PRIVATE(); }

/*!
Adds a widget to the dialogue as a setting \a page.
*/
void RibbonCustomizeDialog::addPage(QWidget *page)
{
    Q_D(RibbonCustomizeDialog);
    Q_ASSERT(page != Q_NULL);
    RibbonCustomizePageWidget *pageWidget = new RibbonCustomizePageWidget(this);
    pageWidget->setContent(page);
    pageWidget->setWindowTitle(page->windowTitle());

    if (const QMetaObject *mo = page->metaObject()) {
        if (mo->indexOfSlot("accepted()") != -1)
            connect(this, SIGNAL(accepted()), page, SLOT(accepted()), Qt::DirectConnection);
        if (mo->indexOfSlot("rejected()") != -1)
            connect(this, SIGNAL(rejected()), page, SLOT(rejected()), Qt::DirectConnection);
    }

    d->m_listWidget.append(page);
    d->addPage(pageWidget);
}

/*!
Inserts a widget to the dialogue in a position \a index as a setting \a page.
*/
void RibbonCustomizeDialog::insertPage(int index, QWidget *page)
{
    Q_D(RibbonCustomizeDialog);
    Q_ASSERT(page != Q_NULL);
    RibbonCustomizePageWidget *pageWidget = new RibbonCustomizePageWidget(this);
    pageWidget->setContent(page);
    pageWidget->setWindowTitle(page->windowTitle());
    connect(this, SIGNAL(accepted()), page, SLOT(accepted()));

    d->m_listWidget.insert(index, page);
    d->insertPage(index, pageWidget);
}

/*!
Returns index of the \a page.
*/
int RibbonCustomizeDialog::indexOf(QWidget *page) const
{
    Q_D(const RibbonCustomizeDialog);
    return d->m_listWidget.indexOf(page);
}

/*!
Returns a \a page that is active at the moment.
*/
QWidget *RibbonCustomizeDialog::currentPage() const
{
    Q_D(const RibbonCustomizeDialog);
    return d->m_listWidget.at(d->m_currentPage);
}

/*!
Makes the \a page an active.
*/
void RibbonCustomizeDialog::setCurrentPage(QWidget *page)
{
    int index = indexOf(page);
    if (index != -1)
        setCurrentPageIndex(index);
}

/*!
Returns an index of the page that is active at the moment.
*/
int RibbonCustomizeDialog::currentPageIndex() const
{
    Q_D(const RibbonCustomizeDialog);
    return d->m_currentPage;
}

/*!
Makes the page with the \a index an active.
*/
void RibbonCustomizeDialog::setCurrentPageIndex(int index)
{
    Q_D(RibbonCustomizeDialog);

    if (index == -1) {
        Q_ASSERT(false);
        return;
    }
    d->m_currentPage = index;
    d->m_listPage->setCurrentRow(index);

    if (RibbonCustomizePageItem *posItem = dynamic_cast<RibbonCustomizePageItem *>(d->m_listPage->item(index))) {
        if (posItem->m_page)
            posItem->m_page->setVisible(true);
    }
}

/*!
Returns a count of the pages in the dialogue.
*/
int RibbonCustomizeDialog::pageCount() const
{
    Q_D(const RibbonCustomizeDialog);
    return d->m_listWidget.count();
}

/*!
Returns a page by its \a index.
*/
QWidget *RibbonCustomizeDialog::pageByIndex(int index) const
{
    Q_D(const RibbonCustomizeDialog);
    return d->m_listWidget[index];
}

/*! \reimp */
void RibbonCustomizeDialog::accept()
{
    Q_D(RibbonCustomizeDialog);
    QDialog::accept();
    d->setCustomizeMode(false);
}

/*! \reimp */
void RibbonCustomizeDialog::reject()
{
    Q_D(RibbonCustomizeDialog);
    QDialog::reject();
    d->setCustomizeMode(false);
}

/*! \reimp */
void RibbonCustomizeDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    Q_D(RibbonCustomizeDialog);
    d->setCustomizeMode(true);
    if (d->m_currentPage != -1)
        setCurrentPageIndex(d->m_currentPage);
}

/*! \reimp */
void RibbonCustomizeDialog::hideEvent(QHideEvent *event)
{
    Q_D(RibbonCustomizeDialog);
    QDialog::hideEvent(event);
    for (int index = 0, count = d->m_listPage->count(); count > index; ++index) {
        if (RibbonCustomizePageItem *posItem = dynamic_cast<RibbonCustomizePageItem *>(d->m_listPage->item(index))) {
            if (posItem->m_page)
                posItem->m_page->setVisible(false);
        }
    }
}
