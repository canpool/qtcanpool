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
#ifndef QTN_RIBBONGALLERY_H
#define QTN_RIBBONGALLERY_H

#include <QIcon>
#include <QWidget>

#include "QtitanDef.h"

class QStyleOptionSlider;
class QMenu;

QTITAN_BEGIN_NAMESPACE

class RibbonGallery;
class RibbonGalleryGroup;
class RibbonGalleryItemPrivate;

/* RibbonGalleryItem */
class QTITAN_EXPORT RibbonGalleryItem
{
public:
    RibbonGalleryItem();
    virtual ~RibbonGalleryItem();
public:
    virtual QSize sizeHint() const;
    void setSizeHint(const QSize& size);
    QIcon icon() const;
    void setIcon(const QIcon& icon);
    QString caption() const;
    void setCaption(const QString& caption);
    QString toolTip() const;
    void setToolTip(const QString& toolTip);
    QString statusTip() const;
    void setStatusTip(const QString& statusTip);
    int getIndex() const;
    bool isSeparator() const;
    void setSeparator(bool b);
    void setEnabled(bool enabled);
    bool isEnabled() const;
    void setVisible(bool visible);
    bool isVisible() const;
    virtual QVariant data(int role) const;
    virtual void setData(int role, const QVariant& value);
    virtual void draw(QPainter* p, RibbonGallery* gallery, QRect rectItem,
    bool enabled, bool selected, bool pressed, bool checked);
private:
    QTN_DECLARE_PRIVATE(RibbonGalleryItem)
    Q_DISABLE_COPY(RibbonGalleryItem)
    friend class RibbonGalleryGroup;
    friend class RibbonGalleryGroupPrivate;
};

class RibbonGalleryGroupPrivate;
class OfficePopupMenu;
/* RibbonGalleryGroup */
class QTITAN_EXPORT RibbonGalleryGroup : public QObject
{
    Q_OBJECT
public:
    RibbonGalleryGroup(QObject* parent = Q_NULL);
    virtual ~RibbonGalleryGroup();

public:
    static RibbonGalleryGroup* createGalleryGroup();

public:
    RibbonGalleryItem* addItem(const QString& caption, const QPixmap& pixmap = QPixmap(), const QColor& transparentColor = QColor());
    RibbonGalleryItem* addItemFromMap(const QString& caption, int mapIndex, const QPixmap& map, const QSize& mapItemSize, const QColor& transparentColor = QColor());

    void appendItem(RibbonGalleryItem* item);
    void insertItem(int index, RibbonGalleryItem* item);

    RibbonGalleryItem* addSeparator(const QString& caption);

    void clear();
    void remove(int index);

    void setSize(const QSize& size);
    QSize getSize() const;

    int getItemCount() const;
    RibbonGalleryItem* getItem(int index) const;

public:
    void setClipItems(bool clipItems);

private:
    friend class RibbonGalleryItem;
    friend class RibbonGallery;
    friend class RibbonGalleryPrivate;
    QTN_DECLARE_PRIVATE(RibbonGalleryGroup)
    Q_DISABLE_COPY(RibbonGalleryGroup)
};

class RibbonGalleryPrivate;
/* RibbonGallery */
class QTITAN_EXPORT RibbonGallery : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isBorderVisible READ isBorderVisible WRITE setBorderVisible)
    Q_PROPERTY(Qt::ScrollBarPolicy scrollBarPolicy READ scrollBarPolicy WRITE setScrollBarPolicy)
    Q_PROPERTY(bool isResizable READ isResizable)

public:
    RibbonGallery(QWidget* parent = Q_NULL);
    virtual ~RibbonGallery();

public:
    void setGalleryGroup(RibbonGalleryGroup* items);
    RibbonGalleryGroup* galleryGroup() const;

    void setBorderVisible(bool isBorderVisible);
    bool isBorderVisible() const;

    void setLabelsVisible(bool showLabels);
    bool isLabelsVisible() const;

    void setScrollBarPolicy(Qt::ScrollBarPolicy policy);
    Qt::ScrollBarPolicy scrollBarPolicy() const;

    void ensureVisible(int index);

    void setPopupMenu(OfficePopupMenu* popupMenu);
    OfficePopupMenu* popupMenu() const;

public:
    int getItemCount() const;

    RibbonGalleryItem* getItem(int index) const;
    void setSelectedItem(int selected);
    int getSelectedItem() const;

    void setCheckedIndex(int index);
    int checkedIndex() const;

    void setCheckedItem(const RibbonGalleryItem* item);
    RibbonGalleryItem* checkedItem() const;

    bool isShowAsButton() const;
    bool isItemSelected() const;

public:
    virtual QRect getBorders() const;
    QRect getDrawItemRect(int index);
    bool isResizable() const;

    int hitTestItem(QPoint point, QRect* rect = Q_NULL) const;

    QRect getItemsRect() const;
    void hideSelection();

Q_SIGNALS:
    void itemPressed(RibbonGalleryItem* item);
    void itemClicked(RibbonGalleryItem* item);
    void itemClicking(RibbonGalleryItem* item, bool& handled);
    void currentItemChanged(RibbonGalleryItem* current, RibbonGalleryItem* previous);
    void itemSelectionChanged();

public:
    virtual void selectedItemChanged();
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void leaveEvent(QEvent*);
    virtual void focusOutEvent(QFocusEvent*);
    virtual void resizeEvent(QResizeEvent*);

private:
    friend class RibbonGalleryGroupPrivate;
    QTN_DECLARE_PRIVATE(RibbonGallery)
    Q_DISABLE_COPY(RibbonGallery)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONGALLERY_H
