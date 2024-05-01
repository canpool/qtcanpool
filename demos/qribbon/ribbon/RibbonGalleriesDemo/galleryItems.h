#ifndef QTN_GALLERYITEMS_H
#define QTN_GALLERYITEMS_H

#include <qribbon/ribbongallery.h>

class RibbonGallery;

/* GalleryItemTable */
class GalleryItemTable : public QRibbon::RibbonGalleryItem
{
public:
    GalleryItemTable();
public:
    virtual void draw(QPainter *p, QRibbon::RibbonGallery *gallery, QRect rcItem, bool enabled, bool selected,
                      bool pressed, bool checked);
};

/* GalleryItemTableLabel */
class GalleryItemTableLabel : public QRibbon::RibbonGalleryItem
{
public:
    GalleryItemTableLabel();
public:
    virtual void draw(QPainter *p, QRibbon::RibbonGallery *gallery, QRect rcItem, bool enabled, bool selected,
                      bool pressed, bool checked);
};

/* GalleryItemFontColor */
class GalleryItemFontColor : public QRibbon::RibbonGalleryItem
{
public:
    GalleryItemFontColor(QColor color, const QString &caption, bool topMargin = true, bool botttomMargin = true);
public:
    static void addStandardColors(QRibbon::RibbonGalleryGroup *items);
    static void addStandardBackColors(QRibbon::RibbonGalleryGroup *items);
    static void addThemeColors(QRibbon::RibbonGalleryGroup *items, int theme);

    virtual void draw(QPainter *p, QRibbon::RibbonGallery *gallery, QRect rcItem, bool enabled, bool selected,
                      bool pressed, bool checked);
    QColor getColor() const
    {
        return m_color;
    }
protected:
    bool m_topMargin;
    bool m_bottomMargin;
    QColor m_color;
};

/* GalleryItemUndo */
class GalleryItemUndo : public QRibbon::RibbonGalleryItem
{
public:
    GalleryItemUndo(const QString &caption);
public:
    virtual void draw(QPainter *p, QRibbon::RibbonGallery *gallery, QRect rcItem, bool enabled, bool selected,
                      bool pressed, bool checked);
};

class QLabel;
/* WidgetGalleryUndo */
class WidgetGalleryUndo : public QRibbon::RibbonGallery
{
public:
    WidgetGalleryUndo();
public:
    QLabel *findInfoWidget();
    virtual void selectedItemChanged();
};

#endif   // QTN_GALLERYITEMS_H
