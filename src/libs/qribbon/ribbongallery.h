/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include <QIcon>
#include <QWidget>

class QStyleOptionSlider;
class QMenu;

QRIBBON_BEGIN_NAMESPACE

class RibbonGallery;
class RibbonGalleryGroup;
class RibbonGalleryItemPrivate;

/* RibbonGalleryItem */
class QRIBBON_EXPORT RibbonGalleryItem
{
public:
    RibbonGalleryItem();
    virtual ~RibbonGalleryItem();
public:
    virtual QSize sizeHint() const;
    void setSizeHint(const QSize &size);
    QIcon icon() const;
    void setIcon(const QIcon &icon);
    QString caption() const;
    void setCaption(const QString &caption);
    QString toolTip() const;
    void setToolTip(const QString &toolTip);
    QString statusTip() const;
    void setStatusTip(const QString &statusTip);
    int getIndex() const;
    bool isSeparator() const;
    void setSeparator(bool b);
    void setEnabled(bool enabled);
    bool isEnabled() const;
    void setVisible(bool visible);
    bool isVisible() const;
    virtual QVariant data(int role) const;
    virtual void setData(int role, const QVariant &value);
    virtual void draw(QPainter *p, RibbonGallery *gallery, QRect rectItem, bool enabled, bool selected, bool pressed,
                      bool checked);
private:
    QRN_DECLARE_PRIVATE(RibbonGalleryItem)
    Q_DISABLE_COPY(RibbonGalleryItem)
    friend class RibbonGalleryGroup;
    friend class RibbonGalleryGroupPrivate;
};

class RibbonGalleryGroupPrivate;
class OfficePopupMenu;
/* RibbonGalleryGroup */
class QRIBBON_EXPORT RibbonGalleryGroup : public QObject
{
    Q_OBJECT
public:
    RibbonGalleryGroup(QObject *parent = Q_NULL);
    virtual ~RibbonGalleryGroup();
public:
    static RibbonGalleryGroup *createGalleryGroup();
public:
    RibbonGalleryItem *addItem(const QString &caption, const QPixmap &pixmap = QPixmap(),
                               const QColor &transparentColor = QColor());
    RibbonGalleryItem *addItemFromMap(const QString &caption, int mapIndex, const QPixmap &map,
                                      const QSize &mapItemSize, const QColor &transparentColor = QColor());

    void appendItem(RibbonGalleryItem *item);
    void insertItem(int index, RibbonGalleryItem *item);

    RibbonGalleryItem *addSeparator(const QString &caption);

    void clear();
    void remove(int index);

    int itemCount() const;
    RibbonGalleryItem *item(int index) const;
    RibbonGalleryItem *takeItem(int index);

    QSize size() const;
    void setSize(const QSize &size);
public:
    void setClipItems(bool clipItems);
private:
    friend class RibbonGalleryItem;
    friend class RibbonGallery;
    friend class RibbonGalleryPrivate;
    QRN_DECLARE_PRIVATE(RibbonGalleryGroup)
    Q_DISABLE_COPY(RibbonGalleryGroup)
};

class RibbonGalleryPrivate;
/* RibbonGallery */
class QRIBBON_EXPORT RibbonGallery : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isBorderVisible READ isBorderVisible WRITE setBorderVisible)
    Q_PROPERTY(int minimumColumnCount READ minimumColumnCount WRITE setMinimumColumnCount)
    Q_PROPERTY(int maximumColumnCount READ maximumColumnCount WRITE setMaximumColumnCount)
    Q_PROPERTY(Qt::ScrollBarPolicy scrollBarPolicy READ scrollBarPolicy WRITE setScrollBarPolicy)
    Q_PROPERTY(bool isResizable READ isResizable)
    Q_PROPERTY(bool autoWidth READ autoWidth WRITE setAutoWidth)
public:
    RibbonGallery(QWidget *parent = Q_NULL);
    virtual ~RibbonGallery();
public:
    void setGalleryGroup(RibbonGalleryGroup *items);
    RibbonGalleryGroup *galleryGroup() const;

    void setBorderVisible(bool isBorderVisible);
    bool isBorderVisible() const;

    void setLabelsVisible(bool showLabels);
    bool isLabelsVisible() const;

    void setScrollBarPolicy(Qt::ScrollBarPolicy policy);
    Qt::ScrollBarPolicy scrollBarPolicy() const;

    void ensureVisible(int index);

    QAction *setPopupMenu(OfficePopupMenu *popupMenu);
    OfficePopupMenu *popupMenu() const;

    void setMinimumColumnCount(int count);
    int minimumColumnCount() const;

    void setMaximumColumnCount(int count);
    int maximumColumnCount() const;

    void setColumnCount(int count);
    int columnCount() const;
public:
    int itemCount() const;

    RibbonGalleryItem *item(int index) const;
    void setSelectedItem(int selected);
    int selectedItem() const;

    void setCheckedIndex(int index);
    int checkedIndex() const;

    void setCheckedItem(const RibbonGalleryItem *item);
    RibbonGalleryItem *checkedItem() const;

    bool isShowAsButton() const;
    bool isItemSelected() const;
public:
    virtual QRect borders() const;
    QRect getDrawItemRect(int index);
    bool isResizable() const;

    bool autoWidth() const;
    void setAutoWidth(bool width);

    int hitTestItem(QPoint point, QRect *rect = Q_NULL) const;

    QRect getItemsRect() const;
    void hideSelection();
    void updatelayout();
    void bestFit();

Q_SIGNALS:
    void itemPressed(RibbonGalleryItem *item);
    void itemClicked(RibbonGalleryItem *item);
    void itemClicking(RibbonGalleryItem *item, bool &handled);
    void currentItemChanged(RibbonGalleryItem *current, RibbonGalleryItem *previous);
    void itemSelectionChanged();
public:
    virtual void selectedItemChanged();
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void focusOutEvent(QFocusEvent *);
    virtual void resizeEvent(QResizeEvent *);
private:
    friend class RibbonGalleryGroupPrivate;
    QRN_DECLARE_PRIVATE(RibbonGallery)
    Q_DISABLE_COPY(RibbonGallery)
};

QRIBBON_END_NAMESPACE
