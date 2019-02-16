#ifndef QTN_GALLERYITEMS_H
#define QTN_GALLERYITEMS_H

#include <QtnRibbonGallery.h>

class RibbonGallery;

/* GalleryItemTable */
class GalleryItemTable : public Qtitan::RibbonGalleryItem
{
public:
    GalleryItemTable();
public:
    virtual void draw(QPainter* p, Qtitan::RibbonGallery* gallery, QRect rcItem, 
        bool enabled, bool selected, bool pressed, bool checked);
};

/* GalleryItemTableLabel */
class GalleryItemTableLabel : public Qtitan::RibbonGalleryItem
{
public:
    GalleryItemTableLabel();
public:
    virtual void draw(QPainter* p, Qtitan::RibbonGallery* gallery, QRect rcItem, 
        bool enabled, bool selected, bool pressed, bool checked);
};


/* GalleryItemFontColor */
class GalleryItemFontColor : public Qtitan::RibbonGalleryItem
{
public:
    GalleryItemFontColor(QColor color, const QString& caption, bool topMargin = true, bool botttomMargin = true);

public:
    static void addStandardColors(Qtitan::RibbonGalleryGroup* items);
    static void addStandardBackColors(Qtitan::RibbonGalleryGroup* items);
    static void addThemeColors(Qtitan::RibbonGalleryGroup* items, int theme);

    virtual void draw(QPainter* p, Qtitan::RibbonGallery* gallery, QRect rcItem, 
        bool enabled, bool selected, bool pressed, bool checked);
    QColor getColor() const { return m_color; }

protected:
    bool m_topMargin;
    bool m_bottomMargin;
    QColor m_color;
};

/* GalleryItemUndo */
class GalleryItemUndo : public Qtitan::RibbonGalleryItem
{
public:
    GalleryItemUndo(const QString& caption);

public:
    virtual void draw(QPainter* p, Qtitan::RibbonGallery* gallery, QRect rcItem, 
                      bool enabled, bool selected, bool pressed, bool checked);
};

class QLabel;
/* WidgetGalleryUndo */
class WidgetGalleryUndo : public Qtitan::RibbonGallery
{
public:
    WidgetGalleryUndo();

public:
    QLabel* findInfoWidget();
    virtual void selectedItemChanged();
};


#endif // QTN_GALLERYITEMS_H
