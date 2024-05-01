#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ribbonwindow.h"
#include <QDialog>

namespace QRibbon
{
class RibbonGalleryGroup;
class RibbonGallery;
}

/* GalleriesDialog */
class GalleriesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GalleriesDialog(QWidget *parent = Q_NULL);
    virtual ~GalleriesDialog();
protected:
    QRibbon::RibbonGalleryGroup *m_itemsEffects;
    QRibbon::RibbonGallery *m_galleryEffects;
protected:
    virtual void resizeEvent(QResizeEvent *);
private:
    Q_DISABLE_COPY(GalleriesDialog)
};

/* MainWindow */
class MainWindow : public RibbonWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = Q_NULL);
    virtual ~MainWindow();
private:
    void createRibbon();
    void createStatusBar();
    void createGalleryItems();

protected Q_SLOTS:
    void dialogGallery();
    void aboutToHideUndo();
    void itemColorPressed(RibbonGalleryItem *item);
    void itemStyleFontPressed(RibbonGalleryItem *item);
    void itemTablePressed(RibbonGalleryItem *item);
    void itemUndoPressed(RibbonGalleryItem *item);
private:
    virtual void updateActionsTheme();
    void updateImageStyles();
private:
    QAction *m_actionUndo;
    QRibbon::RibbonGallery *m_widgetFontTextColor;
    QRibbon::RibbonGallery *m_widgetStyleFont;
    QRibbon::RibbonGallery *m_popupStyleFont;
    QRibbon::RibbonGallery *m_popupTableGallery;
    QRibbon::RibbonGallery *m_popupUndoGallery;
    QRibbon::RibbonGallery *m_popupEffects;
    QRibbon::RibbonGallery *m_popupThemes;

    QRibbon::RibbonGalleryGroup *m_itemsFontTextColor;
    QRibbon::RibbonGalleryGroup *m_itemsStyles;
    QRibbon::RibbonGalleryGroup *m_itemsTable;
    QRibbon::RibbonGalleryGroup *m_itemsUndo;
    QRibbon::RibbonGalleryGroup *m_itemsShapes;
    QRibbon::RibbonGalleryGroup *m_itemsEffects;
    QRibbon::RibbonGalleryGroup *m_itemsThemes;
private:
    Q_DISABLE_COPY(MainWindow)
};

#endif   // MAINWINDOW_H
