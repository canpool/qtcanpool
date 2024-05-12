#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QPrinter>
#include <QPrintDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QLineEdit>
#include <QRadioButton>
#include <QScrollBar>
#include <QFontComboBox>
#include <QAction>
#include <QMenu>
#include <QLabel>
#include <QSpinBox>
#include <QMessageBox>
#include <QButtonGroup>

#include <QtnRibbonQuickAccessBar.h>
#include <QtnRibbonGallery.h>
#include <QtnOfficePopupMenu.h>

#include "galleryItems.h"
#include "aboutdialog.h"
#include "mainwindow.h"


static int g_margin = 2;

/* GalleriesDialog */
GalleriesDialog::GalleriesDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QRect geom = parent->geometry();
    int width = 2 * geom.width() / 4;


    m_itemsEffects = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsEffects->setSize(QSize(72, 56));
    for (int numEffect = 0; numEffect < 20; ++numEffect)
    {
        RibbonGalleryItem* item = m_itemsEffects->addItemFromMap(QString(), numEffect, QPixmap(":/res/galleryeffects.png"), QSize(64, 48));
        QString str = QObject::tr("Effect, %1").arg(numEffect + 1);
        item->setToolTip(str);
    }

    m_galleryEffects = new Qtitan::RibbonGallery(this);
    m_galleryEffects->setMinimumColumnCount(3);
    m_galleryEffects->setBorderVisible(true);
    m_galleryEffects->setLabelsVisible(false);
    m_galleryEffects->setAutoWidth(true);
    m_galleryEffects->setGalleryGroup(m_itemsEffects);

    m_galleryEffects->move(QPoint(g_margin, g_margin));
    m_galleryEffects->resize(width, 56);

    resize(width + g_margin * 2, 56 + g_margin * 2 );
}

GalleriesDialog::~GalleriesDialog()
{
    delete m_itemsEffects;
}

void GalleriesDialog::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);
    QSize delta = event->size() - event->oldSize();
    if (delta.isNull())
        return;
    m_galleryEffects->resize(width() - g_margin * 2, 56);
}



/* MainWindow */
MainWindow::MainWindow(QWidget* parent)
    : RibbonWindow(parent)
{
    setWindowTitle(tr("QtitanRibbon Galleries Sample"));

    m_widgetFontTextColor = Q_NULL;
    m_popupTableGallery = Q_NULL;
    m_popupUndoGallery = Q_NULL;

    createGalleryItems();
    createRibbon();
    createStatusBar();

    move(QPoint(200, 200));
    QRect geom = QApplication::desktop()->availableGeometry();
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);
}

MainWindow::~MainWindow()
{
    delete m_itemsFontTextColor;
    delete m_itemsStyles;
    delete m_itemsEffects;
    delete m_itemsThemes;
    delete m_itemsTable;
    delete m_itemsUndo;
    delete m_itemsShapes;
}

void MainWindow::createRibbon()
{
    if (Qtitan::RibbonPage* pageGallery = ribbonBar()->addPage( tr("&Galleries 1")))
    {
        if (Qtitan::RibbonGroup* groupPopup = pageGallery->addGroup(tr("Popup Galleries")))
        {
            groupPopup->setOptionButtonVisible();
            groupPopup->setContentAlignment(Qt::AlignVCenter);

            QAction* act = groupPopup->optionButtonAction();
            act->setToolTip(tr("Popup Galleries"));
            connect(act, SIGNAL(triggered()), this, SLOT(dialogGallery()));

            OfficePopupMenu* popup = OfficePopupMenu::createPopupMenu(this);
            Qtitan::RibbonGallery* popupGallery = new Qtitan::RibbonGallery;
            popup->setMinimumSize(QSize(307, 168));
            popupGallery->resize(QSize(307, 168));
            popupGallery->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            popupGallery->setGalleryGroup(m_itemsStyles);
            popup->addWidget(popupGallery);
            popup->addSeparator();
            popup->addAction(tr("Save Style"));
            popup->addAction(tr("Apply Style"));
            RibbonControl* control = groupPopup->controlByAction(groupPopup->addAction(QIcon(":/res/largeStyleButton.png"), tr("Styles"), 
                Qt::ToolButtonTextUnderIcon, popup, QToolButton::InstantPopup));
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

            popup = OfficePopupMenu::createPopupMenu(this);
            m_popupTableGallery = new Qtitan::RibbonGallery;
            m_popupTableGallery->setMinimumSize(QSize(193, 175));
            m_popupTableGallery->setMaximumSize(QSize(193, 175));
            m_popupTableGallery->setLabelsVisible(true);
            m_popupTableGallery->setScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_popupTableGallery->setGalleryGroup(m_itemsTable);
            popup->addWidget(m_popupTableGallery);
            control = groupPopup->controlByAction(groupPopup->addAction(QIcon(":/res/largeTable.png"), tr("Table"), 
                Qt::ToolButtonTextUnderIcon, popup, QToolButton::InstantPopup));
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            connect(m_popupTableGallery, SIGNAL(itemPressed(RibbonGalleryItem*)), this, SLOT(itemTablePressed(RibbonGalleryItem*)));

            popup = OfficePopupMenu::createPopupMenu(this);
            popupGallery = new Qtitan::RibbonGallery();

            popupGallery->setMaximumColumnCount(10);
            popupGallery->setLabelsVisible(true);
            popupGallery->setScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            popupGallery->setGalleryGroup(m_itemsFontTextColor);
            popup->addWidget(popupGallery);
            control = groupPopup->controlByAction(groupPopup->addAction(QIcon(":/res/largeColorButton.png"), tr("Color"), Qt::ToolButtonTextUnderIcon, popup, QToolButton::InstantPopup));
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            popup->addSeparator();
            popup->addAction(tr("More Colors..."));

            popup = OfficePopupMenu::createPopupMenu(this);
            popup->setGripVisible(false);
            m_popupUndoGallery = new WidgetGalleryUndo();
            m_popupUndoGallery->setGalleryGroup(m_itemsUndo);
            m_popupUndoGallery->resize(QSize(120, m_itemsUndo->size().height() * m_itemsUndo->itemCount() + 2));
            m_popupUndoGallery->setScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            popup->addWidget(m_popupUndoGallery);
            QLabel* label = new QLabel();
            label->setAlignment(Qt::AlignCenter);
            label->setMinimumWidth(118);
            popup->addWidget(label);
            m_popupUndoGallery->selectedItemChanged();

            m_actionUndo = groupPopup->addAction(QIcon(":/res/largeUndo.png"), tr("Undo"), Qt::ToolButtonTextUnderIcon, popup);
            control = groupPopup->controlByAction(m_actionUndo);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

            connect(popup, SIGNAL(aboutToHide()), this, SLOT(aboutToHideUndo()));
            connect(m_popupUndoGallery, SIGNAL(itemPressed(RibbonGalleryItem*)), this, SLOT(itemUndoPressed(RibbonGalleryItem*)));
        }

        if (Qtitan::RibbonGroup* groupInline = pageGallery->addGroup(tr("Inline Galleries")))
        {
            groupInline->setOptionButtonVisible();
            groupInline->setContentAlignment(Qt::AlignVCenter);

            QAction* act = groupInline->optionButtonAction();
            act->setToolTip(tr("Inline Galleries"));
            connect(act, SIGNAL(triggered()), this, SLOT(dialogGallery()));
/*
            RibbonGalleryControl* galleryControl = new RibbonGalleryControl();
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMaximumItemCount(10);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMinimumItemCount(10);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMaximumItemCount(10);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMinimumItemCount(10);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMaximumItemCount(10);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMinimumItemCount(10);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMaximumItemCount(10);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMinimumItemCount(10);
            groupInline->addControl(galleryControl);

            m_widgetFontTextColor = galleryControl->widget();
            m_widgetFontTextColor->setLabelsVisible(false);
            m_widgetFontTextColor->setScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            m_widgetFontTextColor->setGalleryGroup(m_itemsFontTextColor);
            m_widgetFontTextColor->setCheckedIndex(1);
            connect(m_widgetFontTextColor, SIGNAL(itemPressed(RibbonGalleryItem*)), this, SLOT(itemColorPressed(RibbonGalleryItem*)));

            groupInline->addSeparator();
*/
            RibbonGalleryControl* galleryControl = new RibbonGalleryControl();
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMaximumItemCount(8);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMinimumItemCount(8);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMaximumItemCount(7);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMinimumItemCount(4);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMaximumItemCount(3);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMinimumItemCount(3);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMaximumItemCount(8);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMinimumItemCount(8);
            groupInline->addControl(galleryControl);

            m_widgetStyleFont = galleryControl->widget();
            m_widgetStyleFont->setLabelsVisible(false);
            m_widgetStyleFont->setBorderVisible(true);
            m_widgetStyleFont->setGalleryGroup(m_itemsStyles);
            m_widgetStyleFont->setCheckedIndex(0);

            connect(m_widgetStyleFont, SIGNAL(itemPressed(RibbonGalleryItem*)), this, SLOT(itemStyleFontPressed(RibbonGalleryItem*)));

            OfficePopupMenu* popup = OfficePopupMenu::createPopupMenu(this);
            m_widgetStyleFont->setPopupMenu(popup)->setText(tr("Styl&e"));

            m_popupStyleFont = new Qtitan::RibbonGallery;
            popup->setMinimumSize(QSize(307, 168));
            m_popupStyleFont->resize(QSize(307, 168));

            m_popupStyleFont->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            m_popupStyleFont->setGalleryGroup(m_itemsStyles);
            m_popupStyleFont->setCheckedIndex(0);
            connect(m_popupStyleFont, SIGNAL(itemPressed(RibbonGalleryItem*)), this, SLOT(itemStyleFontPressed(RibbonGalleryItem*)));
            popup->addWidget(m_popupStyleFont);

            popup->addSeparator();
            // hide page, if enabled minimization signal - released()
            QAction* actSaveStyle = popup->addAction(tr("&Save Style"));
            QObject::connect(actSaveStyle, SIGNAL(triggered()), groupInline, SIGNAL(released()));

            QAction* actApplyStyle = popup->addAction(tr("&Apply Style"));
            QObject::connect(actApplyStyle, SIGNAL(triggered()), groupInline, SIGNAL(released()));
        }
    }

    if (Qtitan::RibbonPage* pageGallery = ribbonBar()->addPage( tr("G&alleries 2")))
    {
        if (Qtitan::RibbonGroup* groupPopup = pageGallery->addGroup(tr("Shapes 1")))
        {
            // gallery shapes
            RibbonGalleryControl* galleryControl = new RibbonGalleryControl();

            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMaximumItemCount(20);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMinimumItemCount(20);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMaximumItemCount(19);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMinimumItemCount(10);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMaximumItemCount(9);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMinimumItemCount(9);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMaximumItemCount(9);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMinimumItemCount(9);
            groupPopup->addControl(galleryControl);

            Qtitan::RibbonGallery* widgetGallery = galleryControl->widget();
            widgetGallery->setBorderVisible(true);
            widgetGallery->setLabelsVisible(false);
            widgetGallery->setGalleryGroup(m_itemsShapes);

            OfficePopupMenu* popup = OfficePopupMenu::createPopupMenu(this);
            widgetGallery->setPopupMenu(popup)->setText(tr("Shapes"));
            Qtitan::RibbonGallery* popupGallery = new Qtitan::RibbonGallery;
            popupGallery->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            popupGallery->setGalleryGroup(m_itemsShapes);
            popup->addWidget(popupGallery);

            // gallery effects
            galleryControl = new RibbonGalleryControl();
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMaximumItemCount(6);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMinimumItemCount(6);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMaximumItemCount(5);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMinimumItemCount(3);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMaximumItemCount(2);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMinimumItemCount(2);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMaximumItemCount(3);
            galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMinimumItemCount(3);

            groupPopup->addControl(galleryControl);

            widgetGallery = galleryControl->widget();
            widgetGallery->setBorderVisible(true);
            widgetGallery->setLabelsVisible(false);
            widgetGallery->setGalleryGroup(m_itemsEffects);

            popup = OfficePopupMenu::createPopupMenu(this);
            widgetGallery->setPopupMenu(popup)->setText(tr("Arro&ws"));

            m_popupEffects = new Qtitan::RibbonGallery;
            popup->setMinimumSize(QSize(307, 168));
            m_popupEffects->resize(QSize(307, 168));

            m_popupEffects->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            m_popupEffects->setGalleryGroup(m_itemsEffects);
            m_popupEffects->setCheckedIndex(0);
            connect(m_popupEffects, SIGNAL(itemPressed(RibbonGalleryItem*)), this, SLOT(itemStyleFontPressed(RibbonGalleryItem*)));
            popup->addWidget(m_popupEffects);

        }

        if (Qtitan::RibbonGroup* groupPopup = pageGallery->addGroup(tr("Shapes 2")))
        {
          // gallery effects
          RibbonGalleryControl* galleryControl = new RibbonGalleryControl();

          galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMaximumItemCount(6);
          galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMinimumItemCount(6);
          galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMaximumItemCount(5);
          galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMinimumItemCount(3);
          galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMaximumItemCount(2);
          galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMinimumItemCount(2);
          galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMaximumItemCount(3);
          galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMinimumItemCount(3);

          groupPopup->addControl(galleryControl);

          Qtitan::RibbonGallery* widgetGallery = galleryControl->widget();
          widgetGallery->setBorderVisible(true);
          widgetGallery->setLabelsVisible(false);
          widgetGallery->setGalleryGroup(m_itemsThemes);

          OfficePopupMenu* popup = OfficePopupMenu::createPopupMenu(this);
          widgetGallery->setPopupMenu(popup)->setText(tr("The&mes"));

          m_popupThemes = new Qtitan::RibbonGallery;
          popup->setMinimumSize(QSize(307, 168));
          m_popupThemes->resize(QSize(307, 168));

          m_popupThemes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
          m_popupThemes->setGalleryGroup(m_itemsThemes);
          m_popupThemes->setCheckedIndex(0);
          connect(m_popupThemes, SIGNAL(itemPressed(RibbonGalleryItem*)), this, SLOT(itemStyleFontPressed(RibbonGalleryItem*)));
          popup->addWidget(m_popupThemes);
        }
    }
    ribbonBar()->setFrameThemeEnabled();
}

void MainWindow::createStatusBar()
{
    Qtitan::RibbonSliderPane* sliderPane = new Qtitan::RibbonSliderPane();
    sliderPane->setScrollButtons(true);
    sliderPane->setRange(0, 100); // Range
    sliderPane->setMinimumWidth(100);
    sliderPane->setSingleStep(10);
    sliderPane->setMaximumWidth(130);
    statusBar()->addPermanentWidget(sliderPane);
}

void MainWindow::createGalleryItems()
{
    m_itemsFontTextColor = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsFontTextColor->setSize(QSize(17, 17));
    m_itemsFontTextColor->addSeparator(tr("Theme Colors"));
    GalleryItemFontColor::addThemeColors(m_itemsFontTextColor, 0);
    m_itemsFontTextColor->addSeparator(tr("Standard Colors"));
    GalleryItemFontColor::addStandardColors(m_itemsFontTextColor);

    m_itemsStyles = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsStyles->setSize(QSize(72, 56));
    for (int numStyle = 0; numStyle < 16; numStyle++)
    {
        RibbonGalleryItem* item = m_itemsStyles->addItemFromMap(QString(), numStyle, QPixmap(":/res/galleryStyles.png"), QSize(64, 48));
        QString str = QObject::tr("Style, %1").arg(numStyle + 1);
        item->setToolTip(str);
    }

    //-----------------------------------------------------------
    m_itemsTable = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsTable->setClipItems(false); 
    m_itemsTable->setSize(QSize(19, 19));

    /*RibbonGalleryItem* item =*/ m_itemsTable->appendItem(new GalleryItemTableLabel());
    int indexItem;
    for (indexItem= 0; indexItem < 10 * 8; indexItem++)
        m_itemsTable->appendItem(new GalleryItemTable());
    //-----------------------------------------------------------

    m_itemsUndo = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsUndo->setSize(QSize(0, 21));
    m_itemsUndo->setClipItems(false);
    for (int i = 0; i < 6; i++)
        m_itemsUndo->appendItem(new GalleryItemUndo(QObject::tr("Undo Caption %1").arg(i + 1)));


    m_itemsShapes = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsShapes->setSize(QSize(20, 20));

    m_itemsShapes->addSeparator(tr("Lines"));
    int indexShape;
    for (indexShape = 0; indexShape < 12; indexShape++)
        m_itemsShapes->addItemFromMap(QString(), indexShape, QPixmap(":/res/galleryshapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));

    m_itemsShapes->addSeparator(tr("Basic Shapes"));
    for (; indexShape < 12 + 32; indexShape++)
        m_itemsShapes->addItemFromMap(QString(), indexShape, QPixmap(":/res/galleryshapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));

    m_itemsShapes->addSeparator(tr("Block Arrows"));
    for (; indexShape < 12 + 32 + 27; indexShape++)
        m_itemsShapes->addItemFromMap(QString(), indexShape, QPixmap(":/res/galleryshapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));

    m_itemsShapes->addSeparator(tr("Flowchart"));
    for (; indexShape < 12 + 32 + 27 + 28; indexShape++)
        m_itemsShapes->addItemFromMap(QString(), indexShape, QPixmap(":/res/galleryshapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));

    m_itemsShapes->addSeparator(tr("Callouts"));
    for (; indexShape < 12 + 32 + 27 + 28 + 20; indexShape++)
        m_itemsShapes->addItemFromMap(QString(), indexShape, QPixmap(":/res/galleryshapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));

    m_itemsShapes->addSeparator(tr("Stars and Banners"));
    for (; indexShape < 12 + 32 + 27 + 28 + 20 + 16; indexShape++)
        m_itemsShapes->addItemFromMap(QString(), indexShape, QPixmap(":/res/galleryshapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));

    m_itemsEffects = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsEffects->setSize(QSize(72, 56));
    for (int numEffect = 0; numEffect < 20; ++numEffect)
    {
      RibbonGalleryItem* item = m_itemsEffects->addItemFromMap(QString(), numEffect, QPixmap(":/res/galleryeffects.png"), QSize(64, 48));
      QString str = QObject::tr("Effect, %1").arg(numEffect + 1);
      item->setToolTip(str);
    }

    m_itemsThemes = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsThemes->setSize(QSize(72, 56));
    for (int numTheme = 0; numTheme < 20; ++numTheme)
    {
      RibbonGalleryItem* item = m_itemsThemes->addItemFromMap(QString(), numTheme, QPixmap(":/res/gallerythemes.png"), QSize(64, 48));
      QString str = QObject::tr("Theme, %1").arg(numTheme + 1);
      item->setToolTip(str);
    }
}

void MainWindow::dialogGallery()
{
    GalleriesDialog dlg(this);
    dlg.exec();
}

void MainWindow::aboutToHideUndo()
{
    m_itemsUndo->clear();

    int count = qrand() % 20 + 3;
    for (int i = 0; i < count; i++)
        m_itemsUndo->appendItem(new GalleryItemUndo(QObject::tr("Undo Action %1").arg(i + 1)));

    m_popupUndoGallery->setBaseSize(QSize(120, m_itemsUndo->size().height() * count + 2));
}

void MainWindow::itemColorPressed(RibbonGalleryItem* item)
{
    if (m_widgetFontTextColor && m_widgetFontTextColor->checkedItem() != item)
    {
        m_widgetFontTextColor->setCheckedItem(item);
        if (GalleryItemFontColor* itemColor = dynamic_cast<GalleryItemFontColor*>(item))
        {
            QMessageBox messageBox(QMessageBox::Information, windowTitle(), QLatin1String(""), QMessageBox::Ok, this);
            QString text = tr("<p>Choose Color.</p>");
            QColor color = itemColor->getColor();
            text += tr("<p>R=%1 G=%2 B=%3</p>").arg(color.red()).arg(color.green()).arg(color.blue());
            text += tr("<p>name=%4</p>").arg(itemColor->caption());
            messageBox.setInformativeText(text);
            messageBox.exec();
        }
    }
}

void MainWindow::itemStyleFontPressed(RibbonGalleryItem* item)
{
    if (m_widgetStyleFont && m_widgetStyleFont->checkedItem() != item)
    {
        m_widgetStyleFont->setCheckedItem(item);
        m_popupStyleFont->setCheckedItem(item); 
    }
}

void MainWindow::itemTablePressed(RibbonGalleryItem* item)
{
    Q_UNUSED(item);
    if (m_popupTableGallery)
    {
        int item = m_popupTableGallery->isItemSelected() ? m_popupTableGallery->selectedItem() : -1;

        QRect rcItems = m_popupTableGallery->getItemsRect();
        QSize szItems = m_popupTableGallery->galleryGroup()->size();
        int itemsPerRow = rcItems.width() / szItems.width();

        int columns = item < 1 ? 0 : (item - 1) % itemsPerRow + 1;
        int rows = item < 1 ? 0 : (item- 1) / itemsPerRow + 1;

        QMessageBox messageBox(QMessageBox::Information, windowTitle(), QLatin1String(""), QMessageBox::Ok, this);
        messageBox.setInformativeText(tr("Table size %1 x %2").arg(rows).arg(columns));
        messageBox.exec();
    }
}

void MainWindow::itemUndoPressed(RibbonGalleryItem* item)
{
    Q_UNUSED(item);
    if (m_popupUndoGallery)
    {
        QMessageBox messageBox(QMessageBox::Information, windowTitle(), QLatin1String(""), QMessageBox::Ok, this);
        messageBox.setInformativeText(tr("Undo last %1 actions").arg(m_popupUndoGallery->selectedItem() + 1));
        messageBox.exec();
    }
}

void MainWindow::updateActionsTheme()
{
    RibbonWindow::updateActionsTheme();
    updateImageStyles();
}

void MainWindow::updateImageStyles()
{
    QPixmap px(":/res/galleryStyles.png");
    if (m_ribbonStyle->getTheme() == OfficeStyle::Office2016Black)
        px = QPixmap::fromImage(DrawHelpers::invertColors(px.toImage()));

    QSize sz(64, 48);
    for (int i = 0, count = m_itemsStyles->itemCount(); count > i; i++)
    {
        RibbonGalleryItem* item = m_itemsStyles->item(i);
        QRect rcImage(i * sz.width(), 0, sz.width(), sz.height());
        QPixmap copyPix = px.copy(rcImage.left(), rcImage.top(), rcImage.width(), rcImage.height());
        item->setIcon(copyPix);
    }

    px = QPixmap(":/res/galleryshapes.png");
    QColor transparentColor(0xFF,0,0xFF);
    QImage copyImage = px.copy(0, 0, px.width(), px.height()).toImage();
    if (m_ribbonStyle->getTheme() == OfficeStyle::Office2016Black)
    {
        if (transparentColor.isValid())
            copyImage = DrawHelpers::addaptColors(copyImage, transparentColor.rgb(), QColor(255-48, 255-48, 255-48).rgb());
        px = QPixmap::fromImage(DrawHelpers::invertColors(copyImage));
    }
    else
    {
        if (transparentColor.isValid())
            copyImage.setAlphaChannel(copyImage.createMaskFromColor(transparentColor.rgb(), Qt::MaskOutColor));
        px = QPixmap::fromImage(copyImage);
    }

    sz = QSize(20, 20);
    for (int i = 0, count = m_itemsShapes->itemCount(); count > i; i++)
    {
        RibbonGalleryItem* item = m_itemsShapes->item(i);
        QRect rcImage(i * sz.width(), 0, sz.width(), sz.height());
        QPixmap copyPix = px.copy(rcImage.left(), rcImage.top(), rcImage.width(), rcImage.height());
        item->setIcon(copyPix);
    }
}
