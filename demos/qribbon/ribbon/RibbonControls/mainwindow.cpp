#include "mainwindow.h"
#include "aboutdialog.h"
#include "qribbon/ribbonquickaccessbar.h"
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopWidget>
#include <QEvent>
#include <QFileDialog>
#include <QFontComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSettings>
#include <QSpinBox>

#define Prop_ContextColor "ContextColor"

/* MainWindow */
MainWindow::MainWindow(QWidget *parent) : RibbonWindow(parent)
{
    m_valueProgress = 0;
    m_progressBar = Q_NULL;
    m_progressBar1 = Q_NULL;
    m_progressBar2 = Q_NULL;
    m_contextPage = Q_NULL;

    setWindowTitle(QObject::tr("Ribbon Controls"));

    setCentralWidget(new RibbonWorkspace);

    createMenuFile();
    createQuickAccessBar();
    createRibbon();
    createStatusBar();

    m_timerProgressBar.setInterval(200);
    connect(&m_timerProgressBar, SIGNAL(timeout()), this, SLOT(animateProgressBar()));

    move(QPoint(200, 200));
    QRect geom = QApplication::desktop()->availableGeometry();
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    settings.setValue("recentFileList", files);
    emit updateRecentFileActions(files);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenuFile()
{
    QIcon iconLogo;
    iconLogo.addPixmap(QPixmap(":/shared/res/logo.png"));
    if (QAction *actionFile = ribbonBar()->addSystemButton(iconLogo, tr("&File"))) {
        actionFile->setToolTip(tr("Click here to see everything<br />you can do with your<br />document"));

        if (RibbonSystemPopupBar *popupBar = qobject_cast<RibbonSystemPopupBar *>(actionFile->menu())) {
            QAction *newFile = popupBar->addAction(::ribbonIcon(Image_New), tr("&New"));
            newFile->setShortcut(tr("Ctrl+N"));
            newFile->setStatusTip(tr("Create a new document"));
            newFile->setToolTip(tr("New"));
            newFile->setEnabled(false);

            QAction *openFile = popupBar->addAction(::ribbonIcon(Image_Open, true), tr("&Open..."));
            openFile->setShortcut(tr("Ctrl+O"));
            openFile->setToolTip(tr("Open"));
            openFile->setStatusTip(tr("Open an existing document"));
            connect(openFile, SIGNAL(triggered()), this, SLOT(open()));

            QAction *saveFile = popupBar->addAction(::ribbonIcon(Image_Save, true), tr("&Save"));
            saveFile->setShortcut(tr("Ctrl+S"));
            saveFile->setToolTip(tr("Save"));
            saveFile->setStatusTip(tr("Save the active document"));
            connect(saveFile, SIGNAL(triggered()), this, SLOT(save()));

            QAction *saveAsFile = popupBar->addAction(tr("Save &As..."));
            saveAsFile->setToolTip(tr("Save As"));
            saveAsFile->setStatusTip(tr("Save the active document with a new name"));
            connect(saveAsFile, SIGNAL(triggered()), this, SLOT(save()));

            popupBar->addSeparator();

            QAction *printFile = popupBar->addAction(QIcon(":/res/print.png"), tr("&Print"));
            printFile->setShortcut(tr("Ctrl+P"));
            printFile->setToolTip(tr("Print"));
            printFile->setStatusTip(tr("Print the active document"));
            connect(printFile, SIGNAL(triggered()), this, SLOT(print()));

            QAction *printPreviewFile = popupBar->addAction(QIcon(":/res/printPreview.png"), tr("Print Pre&view"));
            printPreviewFile->setToolTip(tr("Print Preview"));
            printPreviewFile->setStatusTip(tr("Display full pages"));

            QAction *printSetupFile = popupBar->addAction(tr("P&rint Setup..."));
            printSetupFile->setToolTip(tr("Print Setup"));
            printSetupFile->setStatusTip(tr("Change the printer and printing options"));
            connect(printSetupFile, SIGNAL(triggered()), this, SLOT(printSetup()));
            popupBar->addSeparator();

            QAction *actClose = popupBar->addAction(::ribbonIcon(Image_Close), tr("&Close"));
            actClose->setShortcut(tr("Ctrl+C"));
            actClose->setStatusTip(tr("Exit"));
            connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
            popupBar->addPopupBarAction(actClose, Qt::ToolButtonTextBesideIcon);

            QAction *option = new QAction(QPixmap(":/res/smalloption.png"), tr("Opt&ion"), this);
            popupBar->addPopupBarAction(option, Qt::ToolButtonTextBesideIcon);
            option->setEnabled(false);

            if (RibbonPageSystemRecentFileList *pageRecentFile = popupBar->addPageRecentFile(tr("Recent Documents"))) {
                pageRecentFile->setSize(9);
                connect(pageRecentFile, SIGNAL(openRecentFile(const QString &)), this,
                        SLOT(openRecentFile(const QString &)));
                connect(this, SIGNAL(updateRecentFileActions(const QStringList &)), pageRecentFile,
                        SLOT(updateRecentFileActions(const QStringList &)));
            }
        }
    }
}

void MainWindow::createQuickAccessBar()
{
    if (RibbonQuickAccessBar *quickAccessBar = ribbonBar()->quickAccessBar()) {
        QAction *action = quickAccessBar->actionCustomizeButton();
        action->setToolTip(tr("Customize Quick Access Bar"));

        QAction *smallButton = quickAccessBar->addAction(::ribbonIcon(Image_New), tr("New"));
        smallButton->setToolTip(tr("Create a new document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));
        quickAccessBar->setActionVisible(smallButton, false);

        smallButton = quickAccessBar->addAction(::ribbonIcon(Image_Open), tr("Open"));
        smallButton->setToolTip(tr("Open an existing document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));
        quickAccessBar->setActionVisible(smallButton, false);

        smallButton = quickAccessBar->addAction(::ribbonIcon(Image_Save), tr("Save"));
        smallButton->setToolTip(tr("Save the active document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

        smallButton = quickAccessBar->addAction(::ribbonIcon(Image_Undo), tr("&Undo"));
        smallButton->setShortcut(QKeySequence::Undo);
        smallButton->setEnabled(false);

        smallButton = quickAccessBar->addAction(::ribbonIcon(Image_Redo), tr("&Redo"));
        smallButton->setShortcut(QKeySequence::Redo);
        smallButton->setEnabled(false);

        ribbonBar()->showQuickAccess(true);
    }
}

void MainWindow::createRibbon()
{
    if (RibbonPage *pageButtons = ribbonBar()->addPage(tr("&Buttons"))) {
        QMenu *splitPopup = new QMenu(ribbonBar());
        connect(splitPopup->addAction(tr("Option 1")), SIGNAL(triggered()), this, SLOT(pressButton()));
        connect(splitPopup->addAction(tr("Option 2")), SIGNAL(triggered()), this, SLOT(pressButton()));
        connect(splitPopup->addAction(tr("Option 3")), SIGNAL(triggered()), this, SLOT(pressButton()));

        QIcon iconSplit;
        iconSplit.addPixmap(QPixmap(":/res/largeSplitButton.png"));
        iconSplit.addPixmap(QPixmap(":/res/smallSplitButton.png"));

        QIcon iconButtton;
        iconButtton.addPixmap(QPixmap(":/res/largeButton.png"));
        iconButtton.addPixmap(QPixmap(":/res/smallButton.png"));

        QIcon iconPopupButtton;
        iconPopupButtton.addPixmap(QPixmap(":/res/largePopupButton.png"));
        iconPopupButtton.addPixmap(QPixmap(":/res/smallPopupButton.png"));

        // Large Buttons
        if (RibbonGroup *groupClipboard = pageButtons->addGroup(tr("&Large Buttons"))) {
            groupClipboard->setOptionButtonVisible();

            QAction *act = groupClipboard->optionButtonAction();
            act->setToolTip(tr("Large Buttons"));
            connect(act, SIGNAL(triggered()), this, SLOT(pressButton()));

            QAction *action = groupClipboard->addAction(iconButtton, tr("&Button"), Qt::ToolButtonTextUnderIcon);
            RibbonControl *control = groupClipboard->controlByAction(action);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            connect(action, SIGNAL(triggered()), this, SLOT(pressButton()));

            action = groupClipboard->addAction(QIcon(":/res/Edit.Create.svg"), tr("Button &SVG"),
                                               Qt::ToolButtonTextUnderIcon);
            control = groupClipboard->controlByAction(action);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            connect(action, SIGNAL(triggered()), this, SLOT(pressButton()));

            action = groupClipboard->addAction(QIcon(":/res/largeToggleButton.png"), tr("Toggle Button"),
                                               Qt::ToolButtonTextUnderIcon);
            action->setCheckable(true);
            action->setChecked(true);
            control = groupClipboard->controlByAction(action);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);

            QMenu *menuPopup =
                groupClipboard->addMenu(iconPopupButtton, tr("Popup Button"), Qt::ToolButtonTextUnderIcon);
            control = groupClipboard->controlByAction(menuPopup->defaultAction());
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            connect(menuPopup->addAction(tr("Option 1")), SIGNAL(triggered()), this, SLOT(pressButton()));
            connect(menuPopup->addAction(tr("Option 2")), SIGNAL(triggered()), this, SLOT(pressButton()));
            connect(menuPopup->addAction(tr("Option 3")), SIGNAL(triggered()), this, SLOT(pressButton()));

            action = groupClipboard->addAction(iconSplit, tr("S&plit Button"), Qt::ToolButtonTextUnderIcon, splitPopup);
            control = groupClipboard->controlByAction(action);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)
                ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
            connect(action, SIGNAL(triggered()), this, SLOT(pressButton()));
        }

        // Small Buttons
        if (RibbonGroup *groupSmallButtons = pageButtons->addGroup(tr("Small Buttons"))) {
            groupSmallButtons->setOptionButtonVisible();

            QAction *act = groupSmallButtons->optionButtonAction();
            act->setText(tr("S&mall"));
            act->setToolTip(tr("Small Buttons"));
            connect(act, SIGNAL(triggered()), this, SLOT(pressButton()));

            QAction *smallButton =
                groupSmallButtons->addAction(iconButtton, tr("B&utton"), Qt::ToolButtonTextBesideIcon);
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            QMenu *menuPopup =
                groupSmallButtons->addMenu(iconPopupButtton, tr("Popup Button"), Qt::ToolButtonTextBesideIcon);
            menuPopup->addAction(tr("Option &1"));
            menuPopup->addAction(tr("Option &2"));
            menuPopup->addAction(tr("Option &3"));

            smallButton =
                groupSmallButtons->addAction(iconSplit, tr("Split Button"), Qt::ToolButtonTextBesideIcon, splitPopup);
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            smallButton = groupSmallButtons->addAction(iconButtton, tr("B&utton"), Qt::ToolButtonIconOnly);
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            menuPopup = groupSmallButtons->addMenu(iconPopupButtton, tr("Popup Button"), Qt::ToolButtonIconOnly);
            menuPopup->addAction(tr("Option &1"));
            menuPopup->addAction(tr("Option &2"));
            menuPopup->addAction(tr("Option &3"));

            smallButton =
                groupSmallButtons->addAction(iconSplit, tr("Split Button"), Qt::ToolButtonIconOnly, splitPopup);
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));
        }

        // CheckBoxes and Radio Buttons
        if (RibbonGroup *groupCheckboxButtons = pageButtons->addGroup(tr("Checkboxes && Radio Buttons"))) {
            groupCheckboxButtons->setOptionButtonVisible();
            QAction *act = groupCheckboxButtons->optionButtonAction();
            act->setToolTip(tr("Checkboxes && Radio Buttons"));
            connect(act, SIGNAL(triggered()), this, SLOT(pressButton()));
            groupCheckboxButtons->setContentAlignment(Qt::AlignVCenter);

            RibbonCheckBoxControl *checkBoxControl = new RibbonCheckBoxControl(tr("CheckBox 1"));
            groupCheckboxButtons->addControl(checkBoxControl);
            QCheckBox *check1 = checkBoxControl->widget();
            check1->setToolTip(tr("CheckBox 1"));
            check1->setCheckState(Qt::Checked);

            checkBoxControl = new RibbonCheckBoxControl(tr("CheckBox 2"));
            groupCheckboxButtons->addControl(checkBoxControl);
            QCheckBox *check2 = checkBoxControl->widget();
            check2->setToolTip(tr("CheckBox 2"));
            check2->setCheckState(Qt::Unchecked);

            checkBoxControl = new RibbonCheckBoxControl(tr("CheckBox 3"));
            groupCheckboxButtons->addControl(checkBoxControl);
            QCheckBox *check3 = checkBoxControl->widget();
            check3->setToolTip(tr("CheckBox 3"));
            check3->setCheckState(Qt::PartiallyChecked);

            groupCheckboxButtons->addSeparator();

            QButtonGroup *groupButton = new QButtonGroup(this);

            RibbonRadioButtonControl *radioButtonControl = new RibbonRadioButtonControl(tr("Radio Button 1"));
            groupCheckboxButtons->addControl(radioButtonControl);
            QRadioButton *radio1 = radioButtonControl->widget();
            groupButton->addButton(radio1);
            radio1->setToolTip(tr("Radio Button 1"));
            radio1->setChecked(true);

            radioButtonControl = new RibbonRadioButtonControl(tr("Radio Button 2"));
            groupCheckboxButtons->addControl(radioButtonControl);
            QRadioButton *radio2 = radioButtonControl->widget();
            groupButton->addButton(radio2);
            radio2->setToolTip(tr("Radio Button 2"));

            radioButtonControl = new RibbonRadioButtonControl(tr("Radio Button 3"));
            groupCheckboxButtons->addControl(radioButtonControl);
            QRadioButton *radio3 = radioButtonControl->widget();
            groupButton->addButton(radio3);
            radio3->setToolTip(tr("Radio Button 3"));
        }

        // Groups
        if (RibbonGroup *groupGroups = pageButtons->addGroup(tr("Groups"))) {
            groupGroups->setContentAlignment(Qt::AlignVCenter);

            RibbonToolBarControl *toolBarControl = new RibbonToolBarControl(groupGroups);
            QAction *smallButton =
                toolBarControl->addAction(::ribbonIcon(Image_New), tr("New"), Qt::ToolButtonIconOnly);
            smallButton->setToolTip(tr("Create a new document"));
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            smallButton = toolBarControl->addAction(::ribbonIcon(Image_Open), tr("Open"), Qt::ToolButtonIconOnly);
            smallButton->setToolTip(tr("Open an existing document"));
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            smallButton = toolBarControl->addAction(::ribbonIcon(Image_Save), tr("Save"), Qt::ToolButtonIconOnly);
            smallButton->setToolTip(tr("Save the active document"));
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            smallButton = toolBarControl->addAction(::ribbonIcon(Image_Cut), tr("Cut"), Qt::ToolButtonIconOnly);
            smallButton->setToolTip(tr("Cut the selection and put it on the Clipboard"));
            smallButton->setEnabled(false);
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            smallButton = toolBarControl->addAction(QIcon(":/res/smallCopy.png"), tr("Copy"), Qt::ToolButtonIconOnly);
            smallButton->setToolTip(tr("Copy the selection and put it on the Clipboard"));
            smallButton->setEnabled(false);
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            smallButton = toolBarControl->addAction(::ribbonIcon(Image_Clipboard), tr("Paste"), Qt::ToolButtonIconOnly);
            smallButton->setToolTip(tr("Insert Clipboard contents"));
            smallButton->setEnabled(false);
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            toolBarControl->addSeparator();

            smallButton = toolBarControl->addAction(::ribbonIcon(Image_Undo), tr("Undo"), Qt::ToolButtonIconOnly);
            smallButton->setToolTip(tr("Undo the last action"));
            smallButton->setEnabled(false);
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            smallButton = toolBarControl->addAction(::ribbonIcon(Image_Redo), tr("Redo"), Qt::ToolButtonIconOnly);
            smallButton->setToolTip(tr("Redo the previously undone action"));
            smallButton->setEnabled(false);
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            toolBarControl->addSeparator();

            smallButton = toolBarControl->addAction(QIcon(":/res/smallPrint.png"), tr("Print"), Qt::ToolButtonIconOnly);
            smallButton->setToolTip(tr("Print the active document"));
            connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

            smallButton =
                toolBarControl->addAction(QIcon(":/shared/res/about.png"), tr("About"), Qt::ToolButtonIconOnly);
            smallButton->setToolTip(tr("Display program information, version number and copyright"));
            connect(smallButton, SIGNAL(triggered()), this, SLOT(about()));

            groupGroups->addControl(toolBarControl);
        }
    }

    if (RibbonPage *pageEditors = ribbonBar()->addPage(tr("&Editors"))) {
        if (RibbonGroup *groupStandard = pageEditors->addGroup(tr("Standard"))) {
            RibbonLineEditControl *lineEditControl = new RibbonLineEditControl();
            groupStandard->addControl(lineEditControl);
            QLineEdit *controlEdit = lineEditControl->widget();
            controlEdit->setToolTip(tr("Edit"));
            controlEdit->setText(tr("Edit"));

            RibbonComboBoxControl *comboBoxControl = new RibbonComboBoxControl();
            groupStandard->addControl(comboBoxControl);
            QComboBox *controlComboBox = comboBoxControl->widget();
            controlComboBox->setToolTip(tr("Combo"));
            controlComboBox->setEditable(true);
            controlComboBox->addItem(tr("ComboBox"));
            controlComboBox->addItem(tr("Item 1"));
            controlComboBox->addItem(tr("Item 2"));
            controlComboBox->addItem(tr("Item 3"));

            comboBoxControl = new RibbonComboBoxControl();
            groupStandard->addControl(comboBoxControl);
            controlComboBox = comboBoxControl->widget();
            controlComboBox->setToolTip(tr("Combo"));
            controlComboBox->addItem(tr("ComboBox"));
            controlComboBox->addItem(tr("Item 1"));
            controlComboBox->addItem(tr("Item 2"));
        }

        if (RibbonGroup *groupExtendedEditors = pageEditors->addGroup(tr("Extended"))) {
            RibbonSpinBoxControl *spinBoxControl = new RibbonSpinBoxControl();
            groupExtendedEditors->addControl(spinBoxControl);
            QSpinBox *spinBox = spinBoxControl->widget();
            spinBox->setToolTip(tr("Edit"));
            spinBox->setMaximumWidth(150);
            spinBox->setMinimumWidth(150);
            QWidgetAction *action = new QWidgetAction(this);
            action->setText(tr("Edit:"));
            action->setIcon(QIcon(":/res/smallEditor.png"));
            spinBoxControl->setDefaultAction(action);

            RibbonComboBoxControl *comboBoxControl = new RibbonComboBoxControl();
            groupExtendedEditors->addControl(comboBoxControl);
            QComboBox *controlComboBox = comboBoxControl->widget();
            controlComboBox->setMaximumWidth(150);
            controlComboBox->setMinimumWidth(150);
            controlComboBox->setToolTip(tr("ComboBox"));
            controlComboBox->setEditable(true);
            controlComboBox->addItem(tr("ComboBox"));
            controlComboBox->addItem(tr("Item 1"));
            controlComboBox->addItem(tr("Item 2"));
            action = new QWidgetAction(this);
            action->setText(tr("Combo:"));
            action->setIcon(QIcon(":/res/smallComboBox.png"));
            comboBoxControl->setDefaultAction(action);

            comboBoxControl = new RibbonComboBoxControl();
            groupExtendedEditors->addControl(comboBoxControl);
            controlComboBox = comboBoxControl->widget();
            controlComboBox->setMaximumWidth(150);
            controlComboBox->setMinimumWidth(150);
            controlComboBox->setToolTip(tr("ComboBox"));
            controlComboBox->setEditable(true);
            controlComboBox->addItem(tr("ComboBox"));
            controlComboBox->addItem(tr("Item 1"));
            controlComboBox->addItem(tr("Item 2"));
            action = new QWidgetAction(this);
            action->setText(tr("Combo:"));
            action->setIcon(QIcon(":/res/smallComboBox.png"));
            comboBoxControl->setDefaultAction(action);
        }

        if (RibbonGroup *groupDataTime = pageEditors->addGroup(tr("Data/Time"))) {
            RibbonTimeEditControl *timeEdit = new RibbonTimeEditControl();
            QWidgetAction *action = new QWidgetAction(this);
            action->setText(tr("Time:"));
            timeEdit->setDefaultAction(action);
            groupDataTime->addControl(timeEdit);

            RibbonDateEditControl *dataEdit = new RibbonDateEditControl();
            action = new QWidgetAction(this);
            action->setText(tr("Data 1:"));
            dataEdit->setDefaultAction(action);
            groupDataTime->addControl(dataEdit);

            dataEdit = new RibbonDateEditControl();
            action = new QWidgetAction(this);
            action->setText(tr("Data 2:"));
            dataEdit->setDefaultAction(action);
            dataEdit->setCalendarPopup(true);
            groupDataTime->addControl(dataEdit);

            groupDataTime->addSeparator();

            RibbonDateTimeEditControl *dataComboBox = new RibbonDateTimeEditControl();
            action = new QWidgetAction(this);
            action->setText(tr("Data/Time 1:"));
            dataComboBox->setDefaultAction(action);
            groupDataTime->addControl(dataComboBox);

            dataComboBox = new RibbonDateTimeEditControl();
            action = new QWidgetAction(this);
            action->setText(tr("Data/Time 2:"));
            dataComboBox->setDefaultAction(action);
            dataComboBox->setCalendarPopup(true);
            groupDataTime->addControl(dataComboBox);
        }

        if (RibbonGroup *groupGallery = pageEditors->addGroup(tr("Gallery"))) {
            RibbonFontComboBoxControl *fontComboBox = new RibbonFontComboBoxControl();
            QWidgetAction *action = new QWidgetAction(this);
            action->setText(tr("Font:"));
            fontComboBox->setDefaultAction(action);
            groupGallery->addControl(fontComboBox);

            QFontComboBox *comboFont = fontComboBox->widget();
            comboFont->setFontFilters(QFontComboBox::MonospacedFonts);
            if (comboFont->count() > 0)
                comboFont->setCurrentIndex(0);
            else
                comboFont->setEditText(tr(""));
        }
    }

    if (RibbonPage *pageMisc = ribbonBar()->addPage(tr("&Misc"))) {
        RibbonGroup *groupLable = pageMisc->addGroup(tr("Label"));
        groupLable->addControl(new RibbonLabelControl(QObject::tr("Label Text 1")));
        groupLable->addControl(new RibbonLabelControl(QObject::tr("Label Text 2")));
        groupLable->addControl(new RibbonLabelControl(QObject::tr("Label Text 3")));

        RibbonGroup *groupBitmap = pageMisc->addGroup(tr("Bitmap"));
        groupBitmap->setContentAlignment(Qt::AlignVCenter);
        RibbonLabelControl *labelControl = new RibbonLabelControl();
        groupBitmap->addControl(labelControl);
        QLabel *pLabel = labelControl->widget();
        pLabel->setPixmap(QPixmap(":/res/Windows.png"));

        RibbonGroup *groupBitmapLinux = pageMisc->addGroup(tr("Bitmap"));
        groupBitmapLinux->setContentAlignment(Qt::AlignVCenter);
        labelControl = new RibbonLabelControl();
        groupBitmapLinux->addControl(labelControl);
        pLabel = labelControl->widget();
        pLabel->setPixmap(QPixmap(":/res/linux.png"));

        RibbonGroup *groupProgressBar = pageMisc->addGroup(tr("ProgressBar"));
        groupProgressBar->setContentAlignment(Qt::AlignVCenter);
        m_progressBar = new QProgressBar();
        m_progressBar->setAlignment(Qt::AlignHCenter);
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(40);
        m_progressBar->setMinimumWidth(100);
        m_progressBar->setMaximumHeight(22);
        groupProgressBar->addWidget(m_progressBar);

        m_progressBar1 = new QProgressBar();
        m_progressBar1->setTextVisible(false);
        m_progressBar1->setRange(0, 100);
        m_progressBar1->setValue(40);
        m_progressBar1->setMinimumWidth(100);
        m_progressBar1->setMaximumHeight(22);
        groupProgressBar->addWidget(m_progressBar1);

        RibbonCheckBoxControl *checkBoxControl = new RibbonCheckBoxControl(QObject::tr("Animation bar"));
        groupProgressBar->addControl(checkBoxControl);
        QCheckBox *checkAnimationBar = checkBoxControl->widget();
        connect(checkAnimationBar, SIGNAL(stateChanged(int)), this, SLOT(startProgressBar(int)));
        checkAnimationBar->setCheckState(Qt::Unchecked);

        RibbonGroup *groupScrollBar = pageMisc->addGroup(tr("ScrollBar"));
        groupScrollBar->setContentAlignment(Qt::AlignVCenter);

        QScrollBar *scrollBar = new QScrollBar(Qt::Horizontal);
        scrollBar->setMinimumWidth(100);
        scrollBar->setMaximumHeight(18);
        scrollBar->setMinimum(0);
        scrollBar->setMaximum(20);
        groupScrollBar->addWidget(scrollBar);

        RibbonGroup *groupSlider = pageMisc->addGroup(tr("Slider"));
        groupSlider->setContentAlignment(Qt::AlignVCenter);

        RibbonSliderPaneControl *sliderPaneControl = new RibbonSliderPaneControl();
        groupSlider->addControl(sliderPaneControl);
        RibbonSliderPane *sliderPane = sliderPaneControl->widget();
        sliderPane->setScrollButtons(true);
        sliderPane->setMaximumWidth(100);
        sliderPane->setMaximumHeight(18);
        sliderPane->setRange(0, 100);
        sliderPane->setSingleStep(10);
        sliderPane->setSliderPosition(50);

        RibbonSliderControl *sliderControl = new RibbonSliderControl(Qt::Horizontal);
        groupSlider->addControl(sliderControl);
        QSlider *slider = sliderControl->widget();
        slider->setMinimumWidth(100);
        slider->setMaximumHeight(20);
        slider->setRange(0, 100);
        slider->setTickPosition(QSlider::TicksLeft);
        slider->setTickInterval(20);
        slider->setSingleStep(10);
        slider->setSliderPosition(50);
        connect(sliderPane, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
        connect(slider, SIGNAL(valueChanged(int)), sliderPane, SLOT(setValue(int)));

        sliderControl = new RibbonSliderControl(Qt::Horizontal);
        groupSlider->addControl(sliderControl);
        slider = sliderControl->widget();
        slider->setMinimumWidth(100);
        slider->setMaximumHeight(20);
        slider->setTickPosition(QSlider::TicksRight);
        slider->setTickInterval(20);

        groupSlider->addSeparator();

        sliderControl = new RibbonSliderControl(Qt::Vertical);
        groupSlider->addControl(sliderControl);
        slider = sliderControl->widget();
        slider->setMaximumHeight(60);
        slider->setTickPosition(QSlider::TicksLeft);
        slider->setTickInterval(20);

        sliderControl = new RibbonSliderControl(Qt::Vertical);
        groupSlider->addControl(sliderControl);
        slider = sliderControl->widget();
        slider->setMaximumHeight(60);
        slider->setTickPosition(QSlider::TicksRight);
        slider->setTickInterval(20);

        groupSlider->addSeparator();

        sliderControl = new RibbonSliderControl(Qt::Vertical);
        groupSlider->addControl(sliderControl);
        slider = sliderControl->widget();
        slider->setMaximumHeight(60);
    }

    if (RibbonPage *contextPage = ribbonBar()->addPage(tr("&Context Tab"))) {
        m_contextPage = contextPage;
        m_contextPage->setContextColor(RibbonPage::ContextColorRed);

        RibbonGroup *groupContextColor = m_contextPage->addGroup(tr("Context Color"));
        Q_ASSERT(groupContextColor != Q_NULL);

        QButtonGroup *groupColorButton = new QButtonGroup(this);
        connect(groupColorButton, SIGNAL(buttonClicked(QAbstractButton *)), this,
                SLOT(buttonColorClicked(QAbstractButton *)));

        RibbonRadioButtonControl *radioButtonControl = new RibbonRadioButtonControl(tr("Green"));
        groupContextColor->addControl(radioButtonControl);
        QRadioButton *radio = radioButtonControl->widget();
        radio->setProperty(Prop_ContextColor, RibbonPage::ContextColorGreen);
        radio->setToolTip(tr("Color Green"));
        groupColorButton->addButton(radio);

        radioButtonControl = new RibbonRadioButtonControl(tr("Blue"));
        groupContextColor->addControl(radioButtonControl);
        radio = radioButtonControl->widget();
        radio->setProperty(Prop_ContextColor, RibbonPage::ContextColorBlue);
        radio->setToolTip(tr("Color Blue"));
        groupColorButton->addButton(radio);

        radioButtonControl = new RibbonRadioButtonControl(tr("Red"));
        groupContextColor->addControl(radioButtonControl);
        radio = radioButtonControl->widget();
        radio->setProperty(Prop_ContextColor, RibbonPage::ContextColorRed);
        radio->setToolTip(tr("Color Red"));
        radio->setChecked(true);
        groupColorButton->addButton(radio);

        radioButtonControl = new RibbonRadioButtonControl(tr("Yellow"));
        groupContextColor->addControl(radioButtonControl);
        radio = radioButtonControl->widget();
        radio->setProperty(Prop_ContextColor, RibbonPage::ContextColorYellow);
        radio->setToolTip(tr("Color Yellow"));
        groupColorButton->addButton(radio);

        radioButtonControl = new RibbonRadioButtonControl(tr("Cyan"));
        groupContextColor->addControl(radioButtonControl);
        radio = radioButtonControl->widget();
        radio->setProperty(Prop_ContextColor, RibbonPage::ContextColorCyan);
        radio->setToolTip(tr("Color Cyan"));
        groupColorButton->addButton(radio);

        radioButtonControl = new RibbonRadioButtonControl(tr("Purple"));
        groupContextColor->addControl(radioButtonControl);
        radio = radioButtonControl->widget();
        radio->setProperty(Prop_ContextColor, RibbonPage::ContextColorPurple);
        radio->setToolTip(tr("Color Purple"));
        groupColorButton->addButton(radio);

        radioButtonControl = new RibbonRadioButtonControl(tr("Orange"));
        groupContextColor->addControl(radioButtonControl);
        radio = radioButtonControl->widget();
        radio->setProperty(Prop_ContextColor, RibbonPage::ContextColorOrange);
        radio->setToolTip(tr("Color Orange"));
        groupColorButton->addButton(radio);
    }
    ribbonBar()->setFrameThemeEnabled();
    setButtonStyleTheme(m_ribbonStyle->getTheme());
}

void MainWindow::createStatusBar()
{
    RibbonSliderPane *sliderPane = new RibbonSliderPane();
    sliderPane->setScrollButtons(true);
    sliderPane->setRange(0, 100);   // Range
    sliderPane->setMaximumWidth(130);
    sliderPane->setSingleStep(10);
    sliderPane->setSliderPosition(50);
    statusBar()->addPermanentWidget(sliderPane);
    statusBar();
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"));
    if (!fileName.isEmpty()) {
        setCurrentFile(fileName);
        statusBar()->showMessage(tr("File loaded"), 2000);
    }
}

bool MainWindow::save()
{
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Save As"), tr("Document"), QLatin1Char('*') + QLatin1String(".txt"));
    if (!fileName.isEmpty()) {
        setCurrentFile(fileName);
        statusBar()->showMessage(tr("File saved"), 2000);
        return true;
    }
    return false;
}

void MainWindow::openRecentFile(const QString &file)
{
    QMessageBox messageBox(QMessageBox::Information, windowTitle(), QLatin1String(""), QMessageBox::Ok, this);
    messageBox.setInformativeText(QLatin1String("Open Recent File.") + QLatin1String(" ") + file);
    messageBox.exec();
}

void MainWindow::print()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
    }
}

void MainWindow::printSetup()
{
}

void MainWindow::pressButton()
{
    QMessageBox messageBox(QMessageBox::Information, windowTitle(), QLatin1String(""), QMessageBox::Ok, this);
    messageBox.setInformativeText(QLatin1String("Press button."));
    messageBox.exec();
}

void MainWindow::startProgressBar(int state)
{
    if (state == Qt::Checked) {
        m_progressBar2 = new QProgressBar();
        m_progressBar2->setMaximumWidth(200);
        int height = statusBar()->sizeHint().height();
        m_progressBar2->setMaximumHeight(height - height / 3);
        statusBar()->addPermanentWidget(m_progressBar2);

        m_valueProgress = 0;
        m_timerProgressBar.start();
    } else if (state == Qt::Unchecked) {
        m_timerProgressBar.stop();
        m_valueProgress = 40;
        m_progressBar->setValue(m_valueProgress);
        m_progressBar1->setValue(m_valueProgress);
        m_progressBar2->setValue(m_valueProgress);
        statusBar()->removeWidget(m_progressBar2);

        delete m_progressBar2;
        m_progressBar2 = Q_NULL;
    }
}

void MainWindow::animateProgressBar()
{
    m_progressBar->setValue(m_valueProgress);
    m_progressBar1->setValue(m_valueProgress);
    m_progressBar2->setValue(m_valueProgress);

    m_valueProgress += 10;

    if (m_valueProgress == 100) {
        m_progressBar->setValue(m_valueProgress);
        m_progressBar1->setValue(m_valueProgress);
        m_progressBar2->setValue(m_valueProgress);
        m_valueProgress = 0;
    }
}

void MainWindow::buttonColorClicked(QAbstractButton *but)
{
    uint numColor = but->property(Prop_ContextColor).toUInt();
    m_contextPage->setContextColor((RibbonPage::ContextColor)numColor);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    QString curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);

    while (files.size() > 9)
        files.removeLast();

    settings.setValue("recentFileList", files);

    emit updateRecentFileActions(files);
}
