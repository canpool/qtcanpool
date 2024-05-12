#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include "ribbonwindow.h"


namespace Qtitan
{
    class RibbonGalleryGroup;
    class RibbonGallery;
}


/* GalleriesDialog */
class GalleriesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GalleriesDialog(QWidget* parent = Q_NULL);
    virtual ~GalleriesDialog();

protected:
    Qtitan::RibbonGalleryGroup* m_itemsEffects;
    Qtitan::RibbonGallery* m_galleryEffects;

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
    explicit MainWindow(QWidget* parent = Q_NULL);
    virtual ~MainWindow();

private:
    void createRibbon();
    void createStatusBar();
    void createGalleryItems();

protected Q_SLOTS:
    void dialogGallery();
    void aboutToHideUndo();
    void itemColorPressed(RibbonGalleryItem* item);
    void itemStyleFontPressed(RibbonGalleryItem* item);
    void itemTablePressed(RibbonGalleryItem* item);
    void itemUndoPressed(RibbonGalleryItem* item);
private:
    virtual void updateActionsTheme();
    void updateImageStyles();

private:
    QAction* m_actionUndo;
    Qtitan::RibbonGallery* m_widgetFontTextColor;
    Qtitan::RibbonGallery* m_widgetStyleFont;
    Qtitan::RibbonGallery* m_popupStyleFont;
    Qtitan::RibbonGallery* m_popupTableGallery;
    Qtitan::RibbonGallery* m_popupUndoGallery;
    Qtitan::RibbonGallery* m_popupEffects;
    Qtitan::RibbonGallery* m_popupThemes;

    Qtitan::RibbonGalleryGroup* m_itemsFontTextColor;
    Qtitan::RibbonGalleryGroup* m_itemsStyles;
    Qtitan::RibbonGalleryGroup* m_itemsTable;
    Qtitan::RibbonGalleryGroup* m_itemsUndo;
    Qtitan::RibbonGalleryGroup* m_itemsShapes;
    Qtitan::RibbonGalleryGroup* m_itemsEffects;
    Qtitan::RibbonGalleryGroup* m_itemsThemes;

private:
    Q_DISABLE_COPY(MainWindow)
};

#endif // MAINWINDOW_H
