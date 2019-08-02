/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
** Copyright (c) 2019 MaMinJie <canpool@163.com>
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#include <QApplication>
#include <QWidgetAction>
#include <QStyleOption>
#include <QDesktopWidget>
#include <QResizeEvent>
#include <QPainter>
#include <QAction>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QListView>

#include "QtnRibbonGroup.h"
#include "QtnRibbonGallery.h"
#include "QtnRibbonPrivate.h"
#include "QtnOfficePopupMenu.h"
#include "QtnRibbonGroupPrivate.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonButton.h"

QTITAN_USE_NAMESPACE

#define ADDITIVE_WIDTH 20

/*!
\property Qtitan::RibbonGroup::isControlsGrouping
*/
/*!
\property Qtitan::RibbonGroup::isControlsCentering
*/
/*!
\property Qtitan::RibbonGroup::title
*/
/*!
\property Qtitan::RibbonGroup::isOptionButtonVisible
*/

/*!
\class Qtitan::GroupItem
\internal
*/
GroupItem::GroupItem(QWidget* widget, bool begin, bool group)
    : QWidgetItem(widget)
    , action(Q_NULL)
    , tailWidth(0)
    , customWidget(false)
    , beginGroup(begin)
    , wrapGroup(false)
    , wrap(false)
{
    wrapGroup = beginGroup;
    widget->setProperty(qtn_Prop_Begin_Group, beginGroup);
    widget->setProperty(qtn_Prop_Wrap, wrap);
    widget->setProperty(qtn_Prop_Group, group);
}

bool GroupItem::isEmpty() const
{
    return action == 0 || !action->isVisible();
}

/*!
\class Qtitan::GroupDefaultLayout
\internal
*/
GroupDefaultLayout::GroupDefaultLayout(QWidget* parent)
    : GroupLayout(parent)
    , dirty(true)
    , hasSeparator(false)
{
    width_ = 0;
    rowCount_ = 2;
    if (!qobject_cast<RibbonGroup*>(parent))
        return;
}

GroupDefaultLayout::~GroupDefaultLayout()
{
    while (!items.isEmpty())
    {
        GroupItem* item = items.takeFirst();
        if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(item->action)) 
        {
            if (item->customWidget)
                widgetAction->releaseWidget(item->widget());
        }
        delete item;
    }
}

void GroupDefaultLayout::updateMarginAndSpacing()
{
    if (!qobject_cast<RibbonGroup*>(parentWidget()))
        return;
    setMargin( 2 );
    setSpacing( 0 );
}

QSize GroupDefaultLayout::sizeRealHint() const
{
    if ( dirty)
        const_cast<GroupDefaultLayout*>(this)->layoutActions();
    return hint;
}

int GroupDefaultLayout::getMinimumWidth() const
{
    int width = 0;
    RibbonGroup* rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return width;

    const QString& txt = rg->title();
    if (!txt.isEmpty())
    {
        QFontMetrics fm = rg->fontMetrics();
        QSize textSize = fm.size(Qt::TextShowMnemonic, txt);
        textSize.setWidth(textSize.width() + fm.width(QLatin1Char(' '))*2);
        width = textSize.width() + (rg->isOptionButtonVisible() ? 0 : ADDITIVE_WIDTH-4);
    }

    if (rg->isOptionButtonVisible())
        width += rg->qtn_d().m_butOption->width();

    return width;
}

QWidget* GroupDefaultLayout::getNextWidget(const QWidget* w) const
{
    int index = getNextIndex(w);
    return index != -1 ? items.at(index)->widget() : Q_NULL;
}

QWidget* GroupDefaultLayout::getWidget(int index) const
{
    if (index < 0 || index >= items.count())
        return Q_NULL;
    GroupItem* item = items.at(index);
    return item->widget();
}

int GroupDefaultLayout::getIndexWidget(const QWidget* w) const
{
    for (int i = 0; i < items.count(); ++i) 
    {
        GroupItem * item = (GroupItem*)items.at(i);
        if (item->widget() == w)
            return i;
    }
    return -1;
}

int GroupDefaultLayout::getNextIndex(const QWidget* w) const
{
    int nIndex = indexOf((QWidget*)w);
    int nNextIndex = nIndex + 1;
    if (nNextIndex >= 0 && nNextIndex < items.count())
    {
        GroupItem* item = (GroupItem*)itemAt(nNextIndex);
        if (item && !item->action->isSeparator())
            return nNextIndex;
    }
    return nIndex;
}


void GroupDefaultLayout::addItem(QLayoutItem*)
{
    Q_ASSERT_X(false, "addItem", "GroupDefaultLayout::addItem(): please use addAction() instead");
    return;
}

QLayoutItem* GroupDefaultLayout::itemAt(int index) const
{
    if (index < 0 || index >= items.count())
        return 0;
    return items.at(index);
}

QLayoutItem* GroupDefaultLayout::takeAt(int index)
{
    if (index < 0 || index >= items.count())
        return 0;

    GroupItem* item = items.takeAt(index);
    QWidgetAction *widgetAction = qobject_cast<QWidgetAction*>(item->action);
    if (widgetAction != 0 && item->customWidget) 
    {
        widgetAction->releaseWidget(item->widget());
    } 
    else 
    {
        // destroy the QToolButton/QToolBarSeparator
        item->widget()->hide();
        item->widget()->deleteLater();
    }
    invalidate();
    return item;
}

void GroupDefaultLayout::insertAction(int index, QAction* action)
{
    index = qMax( 0, index );
    index = qMin( items.count(), index );

    GroupItem *item = createItem(action);
    if (item) 
    {
        items.insert(index, item);
        invalidate();
    }
}

int GroupDefaultLayout::indexOf(QAction* action) const
{
    for (int i = 0; i < items.count(); ++i) 
    {
        if (items.at(i)->action == action)
            return i;
    }
    return -1;
}

int GroupDefaultLayout::count() const
{
    return items.count();
}

void GroupDefaultLayout::invalidate()
{
    dirty = true;
    GroupLayout::invalidate();
}

void GroupDefaultLayout::setGeometry(const QRect& rect)
{
    dirty = false;
    GroupLayout::setGeometry(rect);
    layoutActions();
}

void GroupDefaultLayout::layoutActions()
{
    if (!dirty)
        updateGeomArray();

    QList<QWidget*> showWidgets, hideWidgets;

    for (int i = 0; i < items.count(); i++)
    {
        GroupItem* item = items.at(i);

        if (item->isEmpty())
            hideWidgets << item->widget();
        else
        {
            item->setGeometry(item->rcItem);
            showWidgets << item->widget();
        }
    }

    // we have to do the show/hide here, because it triggers more calls to setGeometry :(
    for (int i = 0; i < showWidgets.count(); ++i)
        showWidgets.at(i)->show();
    for (int i = 0; i < hideWidgets.count(); ++i)
        hideWidgets.at(i)->hide();
}

void GroupDefaultLayout::updateGeomArray() const
{
    GroupDefaultLayout* that = const_cast<GroupDefaultLayout*>(this);
    that->minSize = QSize(0, 0);
    that->hint = QSize(0, 0);

    RibbonGroup* rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return;

    if (rg->isControlsGrouping())
    {
        updateGeomArray_();
        return;
    }

    const int margin = this->margin();
    const int heightGroup = rg->style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, rg);
    int height = heightGroup/3;

    QSize sizeResult(0, heightGroup);

    int resultHeight = heightGroup + 2;
    int dyPrev = 0, dxPrev = 0, x = 2;

    QList<ExWidgetWrapper*> alignWrappers;
    int maxHeight = heightGroup;
    for (int i = 0; i < items.count(); i++)
    {
        GroupItem* item = items.at(i);

        // if the action are invisible
        bool empty = item->isEmpty();
        if (empty)
            continue;

        bool fullRow = false;

        QSize szControl = item->szItem;
        if (szControl.isEmpty())
        {
            QSize szBase = item->widget()->baseSize();
            item->szItem = szBase.isNull() ? item->sizeHint() : szBase;
            szControl = item->szItem;
        }

        maxHeight = qMax(maxHeight, szControl.height());

        int dx = szControl.width();
        int dy = qMax(height, szControl.height());

        bool beginGroup = item->beginGroup && i > 0;
        bool wrapGroup  = item->wrapGroup  && i > 0;

        if (beginGroup)
            x += 6;

        if (i > 0 && !wrapGroup && !fullRow && dyPrev + dy <= resultHeight)
        {
            item->rcItem = QRect(QPoint(x, dyPrev + 1), QSize(dx, dy));
            dxPrev = qMax(dxPrev, dx);

            if (ExWidgetWrapper* wrapper = qobject_cast<ExWidgetWrapper*>(item->widget()))
            {
                if (wrapper->alignWidget())
                    alignWrappers << wrapper;
            }
        }
        else
        {
            updateAlignWidget(alignWrappers);
            alignWrappers.clear();

            x += dxPrev;
            item->rcItem = QRect(QPoint(x, 0), QSize(dx, dy));
            dxPrev = dx;

            if (ExWidgetWrapper* wrapper = qobject_cast<ExWidgetWrapper*>(item->widget()))
            {
                if (wrapper->alignWidget())
                    alignWrappers << wrapper;
            }
        }

        sizeResult.setWidth(qMax(x + dx, sizeResult.width()));
        dyPrev = item->rcItem.bottom();
    }

    updateAlignWidget(alignWrappers);
    alignWrappers.clear();

    that->width_ = sizeResult.width() + 2;

    sizeResult.setWidth(sizeResult.width() + 4*margin);
    that->minSize = sizeResult;

    updateBorders(qMax(that->width_, getMinimumWidth()), QRect());

    that->hint = sizeResult;
    that->hint.setHeight(maxHeight);
    that->dirty = false;

    int scrollBar = -rg->qtn_d().m_groupScrollPos;

    for (int i = 0; i < items.count(); i++)
    {
        GroupItem* item = items.at(i);
        item->rcItem.translate(scrollBar, 0);
    }
}

void GroupDefaultLayout::updateAlignWidget(QList<ExWidgetWrapper*>& alignWrappers) const
{
    int count = alignWrappers.count();
    if (count > 1)
    {
        int maxWidth = 0;
        for (int i = 0; i < count; i++)
        {
            ExWidgetWrapper* wrapper = alignWrappers.at(i);
            QFontMetrics fm = wrapper->fontMetrics();
            QSize sz = fm.size(Qt::TextHideMnemonic, wrapper->labelText());
            maxWidth = qMax(maxWidth, sz.width() + fm.width(QLatin1Char('x')));
        }
        for (int i = 0; i < count; i++)
            alignWrappers.at(i)->setLengthLabel(maxWidth);
    }
}

void GroupDefaultLayout::updateBorders(int width, QRect rcBorder) const
{
    RibbonGroup * rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return;

    const int margin = this->margin();
    int groupHeight = rg->style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, rg);
    int groupClientHeight = groupHeight;

    bool firstItem = true;
    int numFirstItem = 0;
    int column = 0;
    bool centerItems = rg->isControlsCentering();
    if (rg->isControlsGrouping())
        centerItems = false;

    int controlCount = items.count();

    int leftOffset = margin;

    int minWidth = getMinimumWidth();
    if (rg->isControlsCentering() && width_ < minWidth && width == minWidth)
        leftOffset = 2 + (minWidth - width_) * 0.5;

    for (int i = 0; i < controlCount; i++)
    {
        GroupItem* item = items.at(i);

        QRect rcControl = item->rcItem;
        rcControl.translate(rcBorder.left() + leftOffset, rcBorder.top() + margin/* + 1*/);
        item->rcItem = rcControl;

        item->rcRow = QRect(QPoint(rcBorder.left(), rcBorder.top()), 
            QPoint(rcBorder.left() + width, rcBorder.top() + groupClientHeight));

        if (centerItems && !firstItem)
        {
            if (column != rcControl.left())
            {
                updateCenterColumn(numFirstItem, i - 1, groupClientHeight - 3);
                column = rcControl.left();
                numFirstItem = i;
            }
        }
        else
        {
            firstItem = false;
            column = rcControl.left();
        }
    }
    if (centerItems && controlCount > 0) 
        updateCenterColumn(numFirstItem, controlCount - 1, groupClientHeight - 3);

    //    int beginGroup = rcBorder.left();
    //    int endGroup = beginGroup + width + 5;

    //    setRect(QRect(QPoint(beginGroup, rcBorder.top), QPoint(endGroup,  rcBorder.top + groupClientHeight)));
}

void GroupDefaultLayout::updateCenterColumn(int firstItem, int lastItem, int groupHeight) const
{
    int nHeight = 0;
    for (int i = firstItem; i <= lastItem; i++)
    {
        GroupItem* item = items.at(i);
        nHeight += item->rcItem.height();
    }

    if (nHeight >= groupHeight)
        return;

    int nItems = lastItem - firstItem + 1;
    int nOffset = (groupHeight - nHeight) / (nItems + 1);

    for (int i = firstItem; i <= lastItem; i++)
    {
        GroupItem* item = items.at(i);

        QRect rcControl = item->rcItem;
        rcControl.translate(0, nOffset);
        item->rcItem = rcControl;

        nOffset += nOffset;
    }
}

int GroupDefaultLayout::getSizeSpecialDynamicSize_() const
{
    RibbonGroup * rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return 0;

    int totalWidth = 0, x = 0, y = 0, rowHeight = 0;
    int dataCount = items.count();

    int groupHeight = rg->style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, rg);
    int height = groupHeight/3;

    for (int i = 0; i < dataCount; i++)
    {
        GroupItem * item = items.at(i);
        QSize szControl = item->sizeHint();

        height = qMax(height, szControl.height());
        if (item->wrap)
        {
            x = 0;
            y += rowHeight;
        }
        else
        {
            bool beginGroup = item->beginGroup && i > 0;

            if (beginGroup)
                x += 3;
        }
        item->rcItem = QRect(QPoint(2 + x, y), QSize(szControl.width(), height));

        x += szControl.width(); // -1 ???
        rowHeight = qMax(rowHeight, /*szControl.height()*/height);
        totalWidth = qMax(totalWidth, x);
    }
    return totalWidth + 4;
}

bool GroupDefaultLayout::findBestWrapSpecialDynamicSize_() const
{
    int nMax = getSizeSpecialDynamicSize_();
    int nMin = 0, nMid = 0;

    int current = wrapSpecialDynamicSize_(nMin, true);
    int target = 2;
    if (current < target)
        return false;

    if (current == target)
        return true;

    while (nMin < nMax)
    {
        nMid = (nMin + nMax) * 0.5;
        current = wrapSpecialDynamicSize_(nMid, true);

        if (current <= target)
            nMax = nMid;
        else
        {
            if (nMin == nMid)
            {
                wrapSpecialDynamicSize_(nMax, true);
                break;
            }
            nMin = nMid;
        }
    }
    return true;
}

int GroupDefaultLayout::wrapSpecialDynamicSize_(int width, bool includeTail) const
{
    int dataCount = items.count();
    int nWraps = 0, x = 0;

    for (int i = 0; i < dataCount; i++)
    {
        GroupItem * item = items.at(i);

        item->wrap = false;

        QSize szControl = item->sizeHint();//item->rcItem.size();

        bool beginGroup = item->beginGroup && i > 0;
        bool wrapGroup = item->wrapGroup && i > 0;

        if (false)
        {
            x = szControl.width();
            item->wrap = true;
            nWraps++;
            continue;
        }
        else if (wrapGroup)
        {
            if ((includeTail && (x + item->tailWidth >= width)) || 
                (!includeTail && nWraps == 0 && (x + item->tailWidth >= width)))
            {
                x = szControl.width();
                item->wrap = true;
                nWraps++;
                continue;
            }
        }

        if (beginGroup)
            x += 3;

        x += szControl.width();
    }
    return nWraps;
}

void GroupDefaultLayout::updateGeomArray_() const
{
    RibbonGroup * rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return;

    int resultHeight = rg->style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, rg);

    int dataCount = items.count();

    int tailWidth = 0;
    for (int i = dataCount - 1; i >= 0; i--)
    {
        GroupItem * item = items.at(i);

        QSize szControl = item->sizeHint();

        tailWidth += szControl.width();
        item->tailWidth = tailWidth;

        if (item->wrapGroup)
            tailWidth = 0;
    }

    int baseHeight = resultHeight/3;
    int rowCount = wrapSpecialDynamicSize_(32000, false) + 1;
    int totalWidth = 0;

    if (rowCount == 1)
    {
        rowCount = 3;
        totalWidth = getSizeSpecialDynamicSize_();

        bool allow3Row = resultHeight >= baseHeight * 3;
        bool allow2Row = resultHeight >= baseHeight * 2;

        if (!allow2Row)
        {
            wrapSpecialDynamicSize_(32000, false);
            rowCount = 1;

        }
        else if (!(allow3Row && rowCount_ == 3 && findBestWrapSpecialDynamicSize_()))
        {
            rowCount = wrapSpecialDynamicSize_(totalWidth * 0.5, false) + 1;
        }
    }

    totalWidth = getSizeSpecialDynamicSize_();

    int nOffset = (resultHeight + 1 - baseHeight * rowCount) / (rowCount + 1);
    int totalOffset = nOffset;
    int nRow = 0;

    const int margin = this->margin();

    for (int i = 0; i < dataCount; i++)
    {
        GroupItem * item = items.at(i);

        if (i > 0 && item->wrap)
        {
            nOffset = (resultHeight - baseHeight * rowCount - totalOffset) / (rowCount - nRow);
            totalOffset += nOffset;
            nRow++;
        }
        QRect rect(item->rcItem);
        rect.translate(0, totalOffset);
        item->rcItem = rect;
    }

    width_ = totalWidth;

    afterCalcSize();

    GroupDefaultLayout* that = const_cast<GroupDefaultLayout*>(this);

    QSize sizeResult(totalWidth + 3*margin, resultHeight);
    that->minSize = sizeResult;

    updateBorders(qMax(width_, getMinimumWidth()), QRect());

    that->hint = QSize(sizeResult.width(), sizeResult.height());
    that->dirty = false;
}

bool GroupDefaultLayout::extendSize(int widthAvail)
{
    int dataCount = items.count();
    for (int i = dataCount - 1; i >= 0; i--)
    {
        GroupItem * item = items.at(i);
        if (RibbonGallery* widgetGallery = qobject_cast<RibbonGallery*>(item->widget()))
        {
            if (widgetGallery->isResizable())
            {
                RibbonGalleryGroup* items = widgetGallery->galleryGroup();
                int cx = items ? items->getSize().width() : 0;
                if (cx > 0 && cx <= widthAvail)
                {
                    int count = items->getItemCount();
                    QRect rcBorders = widgetGallery->getBorders();
                    int maxWidth = count * cx + rcBorders.left() + rcBorders.right();
                    if (item->szItem.width() + cx <= maxWidth)
                    {
                        item->szItem.setWidth(qMin(maxWidth, item->szItem.width() + int(widthAvail / cx) * cx));
                        return true;
                    }
                }
                return false;
            }
        }
    }
    return false;
}

void GroupDefaultLayout::resetLayout()
{
    for (int i = 0; i < items.count(); i++)
    {
        GroupItem* item = items.at(i);
        item->szItem = QSize();
        item->rcItem = QRect();
    }
    updateGeomArray();
}

void GroupDefaultLayout::afterCalcSize() const
{
    int dataCount = items.count();
    for (int i = 0; i < dataCount; i++)
    {
        GroupItem * item = items.at(i);
        if (i == 0)
            item->wrap = true;
        item->widget()->setProperty(qtn_Prop_Wrap, item->wrap);
    }
}

QSize GroupDefaultLayout::minimumSize() const
{
    if (dirty)
        updateGeomArray();

    return minSize;
}

QSize GroupDefaultLayout::sizeHint() const
{
    if ( dirty )
        updateGeomArray();

    return hint;
}

GroupItem* GroupDefaultLayout::createItem(QAction* action)
{
    bool customWidget = false;
    bool standardButtonWidget = false;

    QWidget* widget = Q_NULL;
    RibbonGroup* rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return (GroupItem*)Q_NULL;

    if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action)) 
    {
        widget = widgetAction->requestWidget(rg);
        if (!qobject_cast<QToolButton*>(widget))
        {
            if (widget != Q_NULL) 
            {
                widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
                if (!qobject_cast<RibbonGallery*>(widget))
                    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                customWidget = true;
            }

            if (qobject_cast<QAbstractButton*>(widget))
                QObject::connect(widget, SIGNAL(released()), rg, SIGNAL(released()));
            else if (QComboBox* comboBox = qobject_cast<QComboBox*>(widget))
            {
                QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), rg, SLOT(currentIndexChanged(int)));
            }
            else if (qobject_cast<ExWidgetWrapper*>(widget))
            {
                QList<QComboBox*> comboBoxs = widget->findChildren<QComboBox*>();
                foreach(QComboBox* i, comboBoxs)
                    QObject::connect(i, SIGNAL(currentIndexChanged(int)), rg, SLOT(currentIndexChanged(int)));
            }
            else if (RibbonGallery* gallery = qobject_cast<RibbonGallery*>(widget))
            {
                QObject::connect(widget, SIGNAL(itemSelectionChanged()), rg, SIGNAL(released()));
                if (OfficePopupMenu* menu = gallery->popupMenu())
                {
                    QList<RibbonGallery*> subGallerys = menu->findChildren<RibbonGallery*>();
                    foreach(RibbonGallery* i, subGallerys)
                        QObject::connect(i, SIGNAL(itemSelectionChanged()), rg, SIGNAL(released()));
                }
            }
        }
        else
        {
            widget = insertToolButton(rg, action, (QToolButton*)widget);
            standardButtonWidget = true;
        }
    } 
    else if (action->isSeparator()) 
    {
        RibbonSeparator* sep = new RibbonSeparator(rg);
        hasSeparator = true;
        widget = sep;
    }

    if (!widget) 
    {
        QToolButton* button = new RibbonButton(rg);
        widget = insertToolButton(rg, action, button);
        standardButtonWidget = true;
    }

    widget->hide();

    bool beginGroup = false;
    if (hasSeparator && !action->isSeparator()) 
    {
        hasSeparator = false;
        beginGroup = true;
    }

    GroupItem* result = new GroupItem(widget, beginGroup, rg->isControlsGrouping());
    if (standardButtonWidget)
        result->setAlignment(Qt::AlignJustify);

    result->customWidget = customWidget;
    result->action = action;
    return result;
}

QWidget* GroupDefaultLayout::insertToolButton(RibbonGroup* rg, QAction* action, QToolButton* button)
{
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QSize szIcon(32, 32);
    if (!action->icon().isNull() && !action->icon().availableSizes().isEmpty() )
        szIcon = action->icon().availableSizes().first();

    button->setIconSize(szIcon);
    action->setIconText(action->text());
    button->setDefaultAction(action);
    QObject::connect(button, SIGNAL(triggered(QAction*)), rg, SIGNAL(actionTriggered(QAction*)));

    if (button->popupMode() != QToolButton::MenuButtonPopup)
        QObject::connect(button, SIGNAL(released()), rg, SIGNAL(released()));

    if (OfficePopupMenu* menu = qobject_cast<OfficePopupMenu*>(action->menu()))
    {
        QList<RibbonGallery*> subGallerys = menu->findChildren<RibbonGallery*>();
        foreach(RibbonGallery* i, subGallerys)
            QObject::connect(i, SIGNAL(itemSelectionChanged()), rg, SIGNAL(released()));
    }

    return button;
}

/*!
\class Qtitan::RibbonGroupPrivate
\internal
*/
RibbonGroupPrivate::RibbonGroupPrivate()
{
    m_butOption = Q_NULL;
    m_actOption = Q_NULL;
    m_info = Q_NULL;
    m_reduced = false;
    m_controlsGrouping = false;
    m_controlsCentering = false;
    m_minimizedLavel = 4;
    m_groupScrollPos = 0;
    m_pageScrollPos = 0;
}

void RibbonGroupPrivate::init()
{
    QTN_P(RibbonGroup);
    p.setMouseTracking(true);

    m_buttonScrollGroupLeft = new RibbonGroupScroll(&p, true);
    m_buttonScrollGroupLeft->setVisible(false);

    m_buttonScrollGroupRight = new RibbonGroupScroll(&p, false);
    m_buttonScrollGroupRight->setVisible(false);

    QObject::connect(m_buttonScrollGroupLeft, SIGNAL(pressed()), this, SLOT(pressLeftScrollButton()));
    QObject::connect(m_buttonScrollGroupRight, SIGNAL(pressed()), this, SLOT(pressRightScrollButton()));

    QLayout* layout = new GroupDefaultLayout();
    p.setLayout(layout);

    if (GroupDefaultLayout* gLayout = qobject_cast<GroupDefaultLayout*>(layout))
        gLayout->updateMarginAndSpacing();

    m_butOption = new RibbonGroupOption(&p);
    m_actOption = new QAction(&p);
    m_butOption->setDefaultAction(m_actOption);
    QObject::connect(m_butOption, SIGNAL(triggered(QAction*)), &p, SIGNAL(actionTriggered(QAction*)));
    m_butOption->hide();
}

void RibbonGroupPrivate::layoutWidgets()
{
    QTN_P(RibbonGroup);

    QStyleOptionGroupBox opt;
    initStyleOption(opt);

    QRect rc = p.geometry();
    QSize szBut = m_butOption->sizeHint();
    QSize sz(qMax(opt.lineWidth - 3, szBut.width()), qMax(opt.lineWidth - 3, szBut.height()));
    QRect rectButOption(QPoint(rc.right() - opt.lineWidth - 1, rc.bottom() - opt.lineWidth), sz);
    m_butOption->setGeometry(rectButOption);
}

void RibbonGroupPrivate::enableGroupScroll(bool scrollLeft, bool scrollRight)
{
    QTN_P(RibbonGroup);

    Q_ASSERT(m_buttonScrollGroupLeft);
    Q_ASSERT(m_buttonScrollGroupRight);

    if (!m_buttonScrollGroupRight || !m_buttonScrollGroupLeft)
        return;

    const int heightGroup = p.style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, &p);

    QRect rcGroup(p.geometry());
    rcGroup.setHeight(heightGroup + 4);

    if (scrollLeft)
    {
        m_buttonScrollGroupLeft->raise();
        m_buttonScrollGroupLeft->setVisible(true);
        m_buttonScrollGroupLeft->setGeometry(QRect(QPoint(rcGroup.left() - 1, rcGroup.top()), QPoint(rcGroup.left() + 12, rcGroup.bottom())));
    }
    else
        m_buttonScrollGroupLeft->setVisible(false);

    if (scrollRight)
    {
        m_buttonScrollGroupRight->raise();
        m_buttonScrollGroupRight->setVisible(true);
        m_buttonScrollGroupRight->setGeometry(QRect(QPoint(rcGroup.right() - 12, rcGroup.top()), QPoint(rcGroup.right() + 1, rcGroup.bottom())));
    }
    else
        m_buttonScrollGroupRight->setVisible(false);
}

void RibbonGroupPrivate::showGroupScroll()
{
    QTN_P(RibbonGroup);
    if (p.isReduced() && p.isVisible())
    {
        QRect screen = QApplication::desktop()->availableGeometry(&p);
        int totalWidth = p.layout()->minimumSize().width();
        int groupLength = screen.width();
        int scrollPos = m_groupScrollPos;

        if (totalWidth > groupLength)
        {
            if (scrollPos > totalWidth - groupLength)
                scrollPos = totalWidth - groupLength;
        }
        else
            scrollPos = 0;

        if (scrollPos < 0) 
            scrollPos = 0;

        m_groupScrollPos = scrollPos;

        enableGroupScroll(scrollPos > 0, totalWidth - groupLength - scrollPos > 0);
    }
}

void RibbonGroupPrivate::startLayout()
{
    QTN_P(RibbonGroup);
    GroupDefaultLayout* gLayout = qobject_cast<GroupDefaultLayout*>(p.layout());
    if (!gLayout)
        return;

    m_info = new LayoutGroupInfo;
    m_info->width = gLayout->width_;
    m_info->itemInfo = Q_NULL;
    m_info->minWidth = gLayout ? gLayout->getMinimumWidth() : 0;
    m_info->oldReduced = m_reduced;
    if (gLayout)
    {
        int widgetVisibleCount = 0;
        for (int i = 0, count = gLayout->count(); i < count; i++)
        {
            if (QLayoutItem* item = gLayout->itemAt(i))
            {
                QWidget* widget = item->widget();
                if (widget->isVisible())
                    widgetVisibleCount++;
            }
        }
        if (widgetVisibleCount== 0)
            return;

        m_info->widgetCount = widgetVisibleCount;

        m_info->itemInfo = new LayoutItemGroupInfo[widgetVisibleCount];

        int k = 0;
        for (int i = 0, count = gLayout->count(); i < count; i++)
        {
            if (GroupItem* item = (GroupItem*)gLayout->itemAt(i))
            {
                QWidget* widget = item->widget();
                if (widget->isVisible())
                {
                    m_info->itemInfo[k].item = item;
                    k++;
                }
            }
        }
        Q_ASSERT(k == widgetVisibleCount);
        gLayout->rowCount_ = 2;
    }
}

void RibbonGroupPrivate::finalLayout()
{
    if (!m_info)
        return;

    delete[] m_info->itemInfo;
    delete m_info;
    m_info = Q_NULL;
}

bool RibbonGroupPrivate::reduceSize(int level, int width)
{
    QTN_P(RibbonGroup);
    if (!m_info)
        return false;

    if (m_info->width < m_info->minWidth)
    {
        if (level == 2 && m_info->width > 16)
        {
            m_info->minWidth = qMax(m_info->width, m_info->minWidth - width);
            return true;
        }
        return false;
    }

    bool reduced = false;
    GroupDefaultLayout* gLayout = qobject_cast<GroupDefaultLayout*>(p.layout());
    if (gLayout)
    {
        if (level == 1 && m_controlsGrouping && gLayout->rowCount_ == 2 && m_minimizedLavel > level)
        {
            gLayout->rowCount_ = 3;
            reduced = true;
        }
    }
    return reduced;
}

void RibbonGroupPrivate::initStyleOption(QStyleOptionGroupBox& opt) const
{
    QTN_P(const RibbonGroup)
    opt.init(&p);
    opt.text = m_title;
    opt.lineWidth = p.style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightCaptionGroup, &opt, &p);
    opt.textAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
}

bool RibbonGroupPrivate::extendSize(int widthAvail)
{
    QTN_P(RibbonGroup)
    if (GroupDefaultLayout* gLayout = qobject_cast<GroupDefaultLayout*>(p.layout()))
        return gLayout->extendSize(widthAvail);
    return false;
}

void RibbonGroupPrivate::resetLayout()
{
    QTN_P(RibbonGroup)
    if (GroupDefaultLayout* gLayout = qobject_cast<GroupDefaultLayout*>(p.layout()))
        gLayout->resetLayout();
}

QWidget* RibbonGroupPrivate::getNextWidget(const QWidget* w) const
{
    QTN_P(const RibbonGroup)
    if (GroupDefaultLayout* gLayout = qobject_cast<GroupDefaultLayout*>(p.layout()))
        return gLayout->getNextWidget(w);
    return Q_NULL;
}

QWidget* RibbonGroupPrivate::getWidget(int index) const
{
    QTN_P(const RibbonGroup)
    if (GroupDefaultLayout* gLayout = qobject_cast<GroupDefaultLayout*>(p.layout()))
        return gLayout->getWidget(index);
    return Q_NULL;
}

int RibbonGroupPrivate::getIndexWidget(const QWidget* w) const
{
    QTN_P(const RibbonGroup)
    if (GroupDefaultLayout* gLayout = qobject_cast<GroupDefaultLayout*>(p.layout()))
        return gLayout->getIndexWidget(w);
    return -1;
}

int RibbonGroupPrivate::getNextIndex(const QWidget* w) const
{
    QTN_P(const RibbonGroup)
    if (GroupDefaultLayout* gLayout = qobject_cast<GroupDefaultLayout*>(p.layout()))
        return gLayout->getNextIndex(w);
    return -1;
}

void listPageWidth(int totalWidth, int realWidth, QList<int>& pagesWidth) 
{
    if (totalWidth > realWidth)
    {
        pagesWidth << realWidth;
        listPageWidth(totalWidth - realWidth, realWidth, pagesWidth);
    }
    else
        pagesWidth << totalWidth;
    return;
}

void RibbonGroupPrivate::pressLeftScrollButton()
{
    QTN_P(RibbonGroup)
    QList<int> pagesWidth;
    listPageWidth(p.layout()->sizeHint().width(), p.geometry().width(), pagesWidth);

    if (0 < m_pageScrollPos)
        m_pageScrollPos--; 

    m_groupScrollPos -= pagesWidth[m_pageScrollPos];
    showGroupScroll();
    p.layout()->update();
}

void RibbonGroupPrivate::pressRightScrollButton()
{
    QTN_P(RibbonGroup)
    QList<int> pagesWidth;
    listPageWidth(p.layout()->sizeHint().width(), p.geometry().width(), pagesWidth);

    m_groupScrollPos += pagesWidth[m_pageScrollPos];
    showGroupScroll();
    p.layout()->update();

    if (pagesWidth.size() - 1 > m_pageScrollPos)
        m_pageScrollPos++; 
}


/* RibbonGroup */
RibbonGroup::RibbonGroup(QWidget* parent, const QString& title)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(RibbonGroup);
    QTN_D(RibbonGroup);

    d.init();
    setTitle(title);
}

RibbonGroup::~RibbonGroup()
{
    QTN_FINI_PRIVATE();
}

bool RibbonGroup::isControlsGrouping() const
{
    QTN_D(const RibbonGroup);
    return d.m_controlsGrouping;
}

void RibbonGroup::setControlsGrouping(bool controlsGrouping)
{
    QTN_D(RibbonGroup);
    d.m_controlsGrouping = controlsGrouping;
}

/*!
\brief Sets the feature centering controls to \a controlsCentering.
*/ 
void RibbonGroup::setControlsCentering(bool controlsCentering)
{
    QTN_D(RibbonGroup);
    d.m_controlsCentering = controlsCentering;
}

/*!
\brief Returns the feature centering controls.
*/ 
bool RibbonGroup::isControlsCentering() const
{
    QTN_D(const RibbonGroup);
    return d.m_controlsCentering;
}

/*!
\brief Returns true if the group is reduced.
*/ 
bool RibbonGroup::isReduced() const
{
    QTN_D(const RibbonGroup);
    return d.m_reduced;
}

/*!
\brief Returns the title of the group.
*/ 
const QString& RibbonGroup::title() const
{
    QTN_D(const RibbonGroup);
    return d.m_title;
}

/*!
\brief Sets the \a title for the group.
 */
void RibbonGroup::setTitle(const QString& title)
{
    QTN_D(RibbonGroup);
    d.m_title = title;
    adjustSize();
    if (RibbonGroupWrapper* groupWrapper = qobject_cast<RibbonGroupWrapper*>(parentWidget()))
    {
        groupWrapper->setText(title);
        adjustSize();
    }
}

/*!
\brief Sets the \a icon for the group.
 */
void RibbonGroup::setIcon(const QIcon& icon)
{
    QTN_D(RibbonGroup);
    d.m_icon = icon;
    adjustSize();

    if (RibbonGroupWrapper* groupWrapper = qobject_cast<RibbonGroupWrapper*>(parentWidget()))
    {
        groupWrapper->setIcon(icon);
        adjustSize();
    }
}

/*!
\brief Returns the icon of the group.
*/ 
const QIcon& RibbonGroup::icon() const
{
    QTN_D(const RibbonGroup);
    return d.m_icon;
}

/*!
\brief Sets the visible state for the option button used in the group caption.
Notes: Option button is usually used to start the dialogue associated with the group.
*/
void RibbonGroup::setOptionButtonVisible(bool visible)
{
    QTN_D(const RibbonGroup);
    d.m_butOption->setVisible(visible);
    adjustSize();
}

/*!
\brief Returns the visible state for the option button used in the caption of the group.
*/

bool RibbonGroup::isOptionButtonVisible() const
{
    QTN_D(const RibbonGroup);
    return d.m_butOption->isVisible();
}

/*!
\brief Returns the pointer to action associated with the option button. 
Notes: Use this action if you want to initialize tooltips, the text in the status bar (StatusTip), icons or for to define custom events.
*/ 
QAction* RibbonGroup::getOptionButtonAction() const
{
    QTN_D(const RibbonGroup);
    return d.m_actOption;
}

/*!
  \brief Sets the given \a action for the option button of the group. Option button is a small button on the bottom of the group. If action is not set then the option button is not visible.
*/ 
void RibbonGroup::setOptionButtonAction(QAction* action)
{
    QTN_D(RibbonGroup);
    if ( d.m_actOption != NULL )
        delete d.m_actOption;

    d.m_actOption = action;
    d.m_butOption->setDefaultAction(d.m_actOption);
}

/*!
  \brief Creates and adds the action to the group with the given \a icon, \a text and \a style. If you specify the parameter \a menu, then will be constructed a special split button which will allow to cause this menu. Parameter \a mode defines the mode of the popup emerging.
*/ 
QAction* RibbonGroup::addAction(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style, 
                                QMenu* menu, QToolButton::ToolButtonPopupMode mode)
{
    bool setMenuButton = false;
    QAction* action = Q_NULL;
    if (mode == QToolButton::InstantPopup && menu)
    {
        action = menu->menuAction();
        action->setIcon(icon);
        menu->setTitle(text);
    }
    else
    {
        action = new QAction(icon, text, this);
        setMenuButton = true;
    }
    addAction(action);

    QLayoutItem* item = layout()->itemAt(layout()->count()-1);
    if (QToolButton* widgetAction = qobject_cast<QToolButton*>(item ? item->widget() : Q_NULL)) 
    {
        widgetAction->setToolButtonStyle(style);
        if (menu)
        {
            widgetAction->setPopupMode(mode);
            if (setMenuButton)
                widgetAction->setMenu(menu);
        }
    }
    return action;
}

/*!
  \brief Adds the \a action to the group with the given parameters \a style, \a menu and \a mode.
*/ 
QAction* RibbonGroup::addAction(QAction* action, Qt::ToolButtonStyle style, QMenu* menu, QToolButton::ToolButtonPopupMode mode)
{
    addAction(action);
    QLayoutItem* item = layout()->itemAt(layout()->count()-1);
    if (QToolButton* widgetAction = qobject_cast<QToolButton*>(item ? item->widget() : Q_NULL)) 
    {
        widgetAction->setToolButtonStyle(style);
        if (menu)
        {
            widgetAction->setPopupMode(mode);
            widgetAction->setMenu(menu);
        }
    }
    return action;
}

QAction* RibbonGroup::addWidget(QWidget* widget)
{
    QWidgetAction* action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    widget->setAttribute(Qt::WA_Hover);
    addAction(action);
    return action;
}

QAction* RibbonGroup::addWidget(const QIcon& icon, const QString& text, QWidget* widget)
{
    QWidgetAction* action = new QWidgetAction(this);
    action->setIcon(icon);
    action->setText(text);

    if (qobject_cast<QLineEdit*>(widget) || 
        qobject_cast<QComboBox*>(widget) || 
        qobject_cast<QAbstractSpinBox*>(widget) ||
        qobject_cast<QAbstractSlider*>(widget))
    {
        ExWidgetWrapper* exWidget = new ExWidgetWrapper(this, widget);
        exWidget->setIcon(icon);
        exWidget->setLabelText(text);
        widget = exWidget;
    }

    action->setDefaultWidget(widget);
    widget->setAttribute(Qt::WA_Hover);
    addAction(action);

    return action;
}

QAction* RibbonGroup::addWidget(const QIcon& icon, const QString& text, bool align, QWidget* widget)
{
    QAction* action = addWidget(icon, text, widget);
    if (ExWidgetWrapper* exWidget = qobject_cast<ExWidgetWrapper*>(widget->parentWidget()))
        exWidget->setAlignWidget(align);
    return action;
}

QMenu* RibbonGroup::addMenu(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style)
{
    QMenu* menu = new QMenu(text, this);

    QAction* action = menu->menuAction();
    action->setIcon(icon);

    addAction(action);

    QLayoutItem* item = layout()->itemAt(layout()->count()-1);
    if (QToolButton* widgetAction = qobject_cast<QToolButton*>(item ? item->widget() : Q_NULL)) 
    {
        if (Qt::ToolButtonFollowStyle == style && !icon.isNull())
        {
            const QSize size = icon.availableSizes().first();
            style = size.width() < 32 ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonTextUnderIcon;
        }
        widgetAction->setToolButtonStyle(style);
        widgetAction->setPopupMode(QToolButton::InstantPopup);
    }
    return menu;
}

QAction* RibbonGroup::addSeparator()
{
    QAction*action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
    return action;
}

void RibbonGroup::remove(QWidget* widget)
{
    if (!widget)
        return;

    if (ExWidgetWrapper* exWidget = qobject_cast<ExWidgetWrapper*>(widget->parentWidget()))
        widget = exWidget;

    QList<QAction *> actions = this->actions();
    for(int i = 0; i < actions.size(); i++)
    {
        if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(actions.at(i)))
        {
            if (widgetAction->defaultWidget()== widget)
            {
                removeAction(actions.at(i));
                return;
            }
        }
    }
}

void RibbonGroup::clear()
{
    QList<QAction*> actions = this->actions();
    for(int i = 0; i < actions.size(); i++)
        removeAction(actions.at(i));
}

QSize RibbonGroup::sizeHint() const
{
    QTN_D(const RibbonGroup);
    QSize sz(layout()->sizeHint());
    QStyleOptionGroupBox opt;
    d.initStyleOption(opt);
    const int heightGroup = qMax(style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, this), sz.height());
    const int heightCaptionGroup = opt.lineWidth;

    QFontMetrics fm = fontMetrics();
    QSize textSize = fm.size(Qt::TextShowMnemonic, title());
    textSize.setWidth(textSize.width() + fm.width(QLatin1Char(' '))*2);

    int width = qMax(textSize.width() + ADDITIVE_WIDTH, sz.width());
    if (d.m_butOption->isVisible())
        width = qMax(width, d.m_butOption->width() + textSize.width());

    if (isReduced() && isVisible())
    {
        QRect screen = QApplication::desktop()->availableGeometry(this);
        width = qMin(screen.width(), width);
    }

    sz.setWidth(width);
    sz.setHeight(heightGroup + heightCaptionGroup + layout()->margin()*2);
    return sz.expandedTo(QApplication::globalStrut());
}

void RibbonGroup::currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if (isVisible())
    {
        bool doReleased = true;
        QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
        if (qobject_cast<ExWidgetWrapper*>(focusWidget()))
        {
            QList<QComboBox*> comboBoxs = sender()->findChildren<QComboBox*>();
            if (comboBoxs.size() > 0)
                comboBox = comboBoxs.at(0);
        }

        if (comboBox)
        {
            QAbstractItemView* itemView = comboBox->view();
            if (itemView && !itemView->isVisible())
                doReleased = false;
        }
        if (doReleased)
            emit released();
    }
}

/*! \reimp */
bool RibbonGroup::event(QEvent* event)
{
    QTN_D(RibbonGroup);

    if (isReduced())
    {
        if (event->type() == QEvent::Show)
            d.showGroupScroll();
        else if (event->type() == QEvent::Hide)
        {
            d.m_groupScrollPos = 0;
            d.m_pageScrollPos = 0;
            layout()->update();
        }
    }

    if (RibbonGroupWrapper* wrapper = qobject_cast<RibbonGroupWrapper*>(parentWidget()))
    {
        if (!wrapper->isReducedGroup())
        {
            switch(event->type())
            {
            case QEvent::KeyPress :
                {
                    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
                    if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
                    {
                        if (QWidget* focus = focusWidget())
                        {
                            if (qobject_cast<ExWidgetWrapper*>(focus) ||
                                qobject_cast<QComboBox*>(focus)       ||
                                qobject_cast<QLineEdit*>(focus))
                                emit released();
                        }
                    }
                }
                break;
            case QEvent::Show :
                adjustSize();
                break;
            case QEvent::ShowToParent :
                wrapper->show();
                break;
            case QEvent::HideToParent :
                wrapper->hide();
                break;
            default:
                break;
            }
        }
    }
    return QWidget::event(event);
}

/*! \reimp */
void RibbonGroup::paintEvent(QPaintEvent* event)
{
    QTN_D(RibbonGroup);

    QPainter p(this);
    QRect rectEvent = event->rect();
    p.setClipRect(rectEvent);

    QStyleOptionGroupBox opt;
    d.initStyleOption(opt);

    QStyle* ribbonStyle = style();
    if (rectEvent.intersects(opt.rect))
        ribbonStyle->drawControl((QStyle::ControlElement)RibbonStyle::CE_Group, &opt, &p, this);

    int flags = opt.textAlignment;
    flags |= Qt::TextHideMnemonic;

    QRect rcEntryCaption = opt.rect;
    rcEntryCaption.setTop(rcEntryCaption.bottom() - opt.lineWidth);

    int indent = 0;
    if (d.m_butOption->isVisible())
        indent = d.m_butOption->width();

    rcEntryCaption.adjust(0, -1, -indent, -2);
    ribbonStyle->drawItemText(&p, rcEntryCaption, flags, opt.palette, opt.state & QStyle::State_Enabled, opt.text, QPalette::WindowText);
}

/*! \reimp */
void RibbonGroup::actionEvent(QActionEvent* event)
{
    QAction* action = event->action();
    QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action);

    switch (event->type()) 
    {
        case QEvent::ActionAdded: 
            if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
            {
                Q_ASSERT_X(widgetAction == 0 || gLayout->indexOf(widgetAction) == -1,
                           "RibbonGroup", "widgets cannot be inserted multiple times");

                // reparent the action to this RibbonGroup if it has been created
                // using the addAction(m_title) etc. convenience functions, to
                // preserve Qt 4.1.x behavior. The widget is already
                // reparented to us due to the createWidget call inside
                // createItem()
                // if (widgetAction != 0 && widgetAction->d_func()->autoCreated)
                if (widgetAction != Q_NULL)
                    widgetAction->setParent(this);

                int index = layout()->count();
                if (event->before()) 
                {
                    index = gLayout->indexOf(event->before());
                    Q_ASSERT_X(index != -1, "RibbonGroup::insertAction", "internal error");
                }

                if(!qobject_cast<ReducedGroupPopupMenu*>(action->menu()))
                    gLayout->insertAction(index, action);

                break;
            }

        case QEvent::ActionChanged:
            {
                layout()->invalidate();
                QApplication::postEvent(parentWidget()->parentWidget(), new QEvent(QEvent::LayoutRequest));
                break;
            }

        case QEvent::ActionRemoved: 
            if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
            {
                event->action()->disconnect(this);

                int index = gLayout->indexOf(action);
                if (index != -1) 
                {
                    // to remove the shell, if it exists
                    if (QWidgetItem* itemWidget = dynamic_cast<QWidgetItem*>(gLayout->itemAt(index)))
                    {
                        if (ExWidgetWrapper* exWidget = qobject_cast<ExWidgetWrapper*>(itemWidget->widget()))
                        {
                            const QObjectList& listChildren = exWidget->children();
                            for (int i = 0, count = listChildren.count(); count > i; i++)
                            {
                                if (QWidget* widget = qobject_cast<QWidget*>(listChildren[i]))
                                    widget->setParent(Q_NULL);
                            }
                            delete exWidget;
                        }
                    }
                    delete gLayout->takeAt(index);
                }
                QApplication::postEvent(parentWidget()->parentWidget(), new QEvent(QEvent::LayoutRequest));
                break;
            }
        default:
            Q_ASSERT_X(false, "RibbonGroup::actionEvent", "internal error");
    }
}

/*! \reimp */
void RibbonGroup::changeEvent(QEvent* event)
{
    switch (event->type()) 
    {
        case QEvent::FontChange: 
            if (RibbonGroupWrapper* wrapper = qobject_cast<RibbonGroupWrapper*>(parentWidget()))
            {
                if (!wrapper->isReducedGroup())
                {
                    if (QWidget* parent = wrapper->parentWidget()->parentWidget())
                    {
                        QEvent ev(QEvent::LayoutRequest);
                        QApplication::sendEvent(parent, &ev);
                        parent->updateGeometry();
                    }
                }
            }
            break;
        default:
            break;
    };
    return QWidget::changeEvent(event);
}

/*! \reimp */
void RibbonGroup::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    if (qobject_cast<ReducedGroupPopupMenu*>(parentWidget()))
        update();
}

/*! \reimp */
void RibbonGroup::leaveEvent( QEvent* event )
{
    Q_UNUSED(event);
    if (qobject_cast<ReducedGroupPopupMenu*>(parentWidget()))
        update();
}

/*! \reimp */
void RibbonGroup::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    QTN_D(RibbonGroup);
    d.layoutWidgets();
}

/*! \reimp */
void RibbonGroup::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
}
