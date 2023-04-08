/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribbongallerycontrols.h"
#include "ribbonbar_p.h"
#include "ribbonbutton_p.h"
#include "ribbongroup.h"

QRIBBON_USE_NAMESPACE

RibbonGalleryControl::RibbonGalleryControl(RibbonGroup *parentGroup)
    : RibbonWidgetControl(parentGroup, true /*ignoreActionSettings*/)
    , m_marginTop(0)
    , m_marginBottom(0)
{
    setContentsMargins(3, 3);
    // 6 3-5 2 6
    sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMinimumItemCount(6);
    sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMaximumItemCount(6);

    sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMinimumItemCount(3);
    sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMaximumItemCount(5);

    sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMinimumItemCount(2);
    sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMaximumItemCount(2);

    sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMinimumItemCount(6);
    sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMaximumItemCount(6);

    RibbonGallery *gallery = new RibbonGallery(this);
    gallery->setGeometry(0, 0, 0, 0);
    setContentWidget(gallery);
    widget()->updatelayout();
}

RibbonGalleryControl::~RibbonGalleryControl() {}

RibbonGallery *RibbonGalleryControl::widget() const { return qobject_cast<RibbonGallery *>(contentWidget()); }

void RibbonGalleryControl::setContentsMargins(int top, int bottom)
{
    m_marginTop = top;
    m_marginBottom = bottom;
}

QSize RibbonGalleryControl::sizeHint() const
{
    QSize sz = RibbonWidgetControl::sizeHint();

    if (widget() && parentGroup()) {
        if (RibbonBar *ribbonBar = parentGroup()->ribbonBar())
            sz.setHeight(ribbonBar->rowItemHeight() * ribbonBar->rowItemCount() - (m_marginTop + m_marginBottom));

        int count = widget()->columnCount();
        if (count > 0 && widget()->itemCount() > 0) {
            RibbonGalleryItem *item = widget()->item(0);
            QSize szItem = item->sizeHint();
            Q_ASSERT(!szItem.isNull());
            sz.setWidth(count * szItem.width() + widget()->borders().width() + 2);
        } else {
            sz.rwidth() = qMin(maximumWidth(), sz.width());
            sz.rheight() = qMin(maximumHeight(), sz.height());
            sz.rwidth() = qMax(minimumWidth(), sz.width());
            sz.rheight() = qMax(minimumHeight(), sz.height());
        }
    }
    return sz;
}

void RibbonGalleryControl::adjustCurrentSize(bool expand)
{
    int min = sizeDefinition(currentSize())->minimumItemCount();
    int max = sizeDefinition(currentSize())->maximumItemCount();
    int current = widget()->columnCount();

    if (expand)
        current++;
    else
        current--;
    current = qMin(max, current);
    current = qMax(min, current);
    widget()->setColumnCount(current);
}

void RibbonGalleryControl::sizeChanged(RibbonControlSizeDefinition::GroupSize size)
{
    int min = sizeDefinition(size)->minimumItemCount();
    int max = sizeDefinition(size)->maximumItemCount();

    if (size > currentSize())
        widget()->setColumnCount(max);
    else
        widget()->setColumnCount(min);

    RibbonWidgetControl::sizeChanged(size);
}

void RibbonGalleryControl::resizeEvent(QResizeEvent *event)
{
    RibbonWidgetControl::resizeEvent(event);
    widget()->updatelayout();
}
