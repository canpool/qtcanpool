#include "dockwidgets.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTreeWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QFileSystemModel>
#include <QToolBox>
#include <QTreeView>
#include <QPixmap>
#include <QBitmap>

#include <QtnCommonStyle.h>
#include <QtnStyleHelpers.h>


/* StyleDock */
StyleDock::StyleDock(QWidget* parent)
    : QFrame(parent)
{
    createWidgets();
}

void StyleDock::createWidgets()
{
    QWidget* styleGroup = new QWidget();
    QVBoxLayout* listLayout = new QVBoxLayout();
    listLayout->setMargin(0);

    QListWidget* customerList = new QListWidget();
    for (int i = 0; 10 > i; i++)
        customerList->addItems(QStringList(QObject::tr("Item %1").arg(i+1)));

    listLayout->addWidget(customerList);
    styleGroup->setLayout(listLayout);

    QGroupBox* optionsGroup = new QGroupBox(tr("Preview"));
    QVBoxLayout* optionsLayout = new QVBoxLayout();
    QCheckBox* showPreviewBox = new QCheckBox(tr("Show Preview"));
    QCheckBox* linkedStylesBox = new QCheckBox(tr("Disable Linked Styles"));
    optionsLayout->addWidget(showPreviewBox);
    optionsLayout->addWidget(linkedStylesBox);
    optionsGroup->setLayout(optionsLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(styleGroup);
    mainLayout->addWidget(optionsGroup);
    //    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

/* TreeDock */
TreeDock::TreeDock(QWidget* parent)
    : QFrame(parent)
{
    createWidgets();
}

void TreeDock::createWidgets()
{
    QWidget* styleGroup = new QWidget();
    QVBoxLayout* treeLayout = new QVBoxLayout();
    treeLayout->setMargin(0);

    QFileSystemModel* model = new QFileSystemModel;
    model->setRootPath("");
    QTreeView* tree = new QTreeView();
    tree->setModel(model);

    tree->setAnimated(false);
    tree->setIndentation(20);
    tree->setSortingEnabled(true);

    treeLayout->addWidget(tree);
    styleGroup->setLayout(treeLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setMargin(2);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(styleGroup);
    setLayout(mainLayout);
}



/* ModulesDock */
ModulesDock::ModulesDock(QWidget* parent)
    : QFrame(parent)
{
    createWidgets();
}

void ModulesDock::createWidgets()
{
    QTableWidget* tableWidget = new QTableWidget;
    tableWidget->setColumnCount(11);
    tableWidget->setRowCount(6);
    tableWidget->setShowGrid(false);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
//    tableWidget->setSortingEnabled(true);
//  tableWidget->verticalHeader()->hide();
//  tableWidget->horizontalHeader()->setStretchLastSection(true);

    tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Name")));
    tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Path")));
    tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Optimized")));
    tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("User Code")));
    tableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem(tr("Symbol Status")));
    tableWidget->setHorizontalHeaderItem(5, new QTableWidgetItem(tr("Symbol File")));
    tableWidget->setHorizontalHeaderItem(6, new QTableWidgetItem(tr("Order")));
    tableWidget->setHorizontalHeaderItem(7, new QTableWidgetItem(tr("Version")));
    tableWidget->setHorizontalHeaderItem(8, new QTableWidgetItem(tr("Timestamp")));
    tableWidget->setHorizontalHeaderItem(9, new QTableWidgetItem(tr("Addresswewe")));
    tableWidget->setHorizontalHeaderItem(10, new QTableWidgetItem(tr("Process")));
//    tableWidget->sortItems(4);

    int height = tableWidget->horizontalHeader()->sizeHint().height();

    tableWidget->setRowHeight(0, height);
    tableWidget->setVerticalHeaderItem(0, new QTableWidgetItem(QIcon(":/res/modules.png"),tr("")));
    tableWidget->setItem(0, 0, new QTableWidgetItem(tr("ntdll.dll")));
    tableWidget->setItem(0, 1, new QTableWidgetItem(tr("C:/Windows/System32/ntdll.dll")));
    tableWidget->setItem(0, 2, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(0, 3, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(0, 4, new QTableWidgetItem(tr("Symbols loaded.")));
    tableWidget->setItem(0, 5, new QTableWidgetItem(tr("C:/Qtitan/customstyles.pdb")));
    tableWidget->setItem(0, 6, new QTableWidgetItem(tr("1")));
    tableWidget->setItem(0, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
    tableWidget->setItem(0, 8, new QTableWidgetItem(tr("27.10.2010 8:32")));
    tableWidget->setItem(0, 9, new QTableWidgetItem(tr("773A0000-774DD000")));
    tableWidget->setItem(0, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));

    tableWidget->setRowHeight(1, height);
    tableWidget->setVerticalHeaderItem(1, new QTableWidgetItem(QIcon(":/res/modules.png"),tr("")));
    tableWidget->setItem(1, 0, new QTableWidgetItem(tr("ntdll.dll")));
    tableWidget->setItem(1, 1, new QTableWidgetItem(tr("C:/Windows/System32/kernel32.dll")));
    tableWidget->setItem(1, 2, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(1, 3, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(1, 4, new QTableWidgetItem(tr("No symbols loaded.")));
    tableWidget->setItem(1, 5, new QTableWidgetItem(tr("")));
    tableWidget->setItem(1, 6, new QTableWidgetItem(tr("2")));
    tableWidget->setItem(1, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
    tableWidget->setItem(1, 8, new QTableWidgetItem(tr("27.10.2010 8:32")));
    tableWidget->setItem(1, 9, new QTableWidgetItem(tr("872A0000-766DD000")));
    tableWidget->setItem(1, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));

    tableWidget->setRowHeight(2, height);
    tableWidget->setVerticalHeaderItem(2, new QTableWidgetItem(QIcon(":/res/modules.png"),tr("")));
    tableWidget->setItem(2, 0, new QTableWidgetItem(tr("kernelBase.dll")));
    tableWidget->setItem(2, 1, new QTableWidgetItem(tr("C:/Windows/System32/kernelBase.dll")));
    tableWidget->setItem(2, 2, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(2, 3, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(2, 4, new QTableWidgetItem(tr("No symbols loaded.")));
    tableWidget->setItem(2, 5, new QTableWidgetItem(tr("")));
    tableWidget->setItem(2, 6, new QTableWidgetItem(tr("3")));
    tableWidget->setItem(2, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
    tableWidget->setItem(2, 8, new QTableWidgetItem(tr("08.10.2010 8:32")));
    tableWidget->setItem(2, 9, new QTableWidgetItem(tr("772A0000-566DD000")));
    tableWidget->setItem(2, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));

    tableWidget->setRowHeight(3, height);
    tableWidget->setVerticalHeaderItem(3, new QTableWidgetItem(QIcon(":/res/modules.png"),tr("")));
    tableWidget->setItem(3, 0, new QTableWidgetItem(tr("QtCoreed4.dll")));
    tableWidget->setItem(3, 1, new QTableWidgetItem(tr("C:/Qt/4.6.2/Bin/QtCored4.dll")));
    tableWidget->setItem(3, 2, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(3, 3, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(3, 4, new QTableWidgetItem(tr("Symbols loaded.")));
    tableWidget->setItem(3, 5, new QTableWidgetItem(tr("C:/Qt/4.6.2/lib/QtCored4.pdb")));
    tableWidget->setItem(3, 6, new QTableWidgetItem(tr("4")));
    tableWidget->setItem(3, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
    tableWidget->setItem(3, 8, new QTableWidgetItem(tr("08.10.2010 8:32")));
    tableWidget->setItem(3, 9, new QTableWidgetItem(tr("772A0000-566DD000")));
    tableWidget->setItem(3, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));

    tableWidget->setRowHeight(4, height);
    tableWidget->setVerticalHeaderItem(4, new QTableWidgetItem(QIcon(":/res/modules.png"),tr("")));
    tableWidget->setItem(4, 0, new QTableWidgetItem(tr("user32.dll")));
    tableWidget->setItem(4, 1, new QTableWidgetItem(tr("C:/Windows/System32/user32.dll")));
    tableWidget->setItem(4, 2, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(4, 3, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(4, 4, new QTableWidgetItem(tr("No symbols loaded.")));
    tableWidget->setItem(4, 5, new QTableWidgetItem(tr("")));
    tableWidget->setItem(4, 6, new QTableWidgetItem(tr("3")));
    tableWidget->setItem(4, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
    tableWidget->setItem(4, 8, new QTableWidgetItem(tr("08.10.2010 8:32")));
    tableWidget->setItem(4, 9, new QTableWidgetItem(tr("552A0000-544AD000")));
    tableWidget->setItem(4, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));

    tableWidget->setRowHeight(5, height);
    tableWidget->setVerticalHeaderItem(5, new QTableWidgetItem(QIcon(":/res/modules.png"),tr("")));
    tableWidget->setItem(5, 0, new QTableWidgetItem(tr("usp10.dll")));
    tableWidget->setItem(5, 1, new QTableWidgetItem(tr("C:/Windows/System32/usp10.dll")));
    tableWidget->setItem(5, 2, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(5, 3, new QTableWidgetItem(tr("N/A")));
    tableWidget->setItem(5, 4, new QTableWidgetItem(tr("No symbols loaded.")));
    tableWidget->setItem(5, 5, new QTableWidgetItem(tr("")));
    tableWidget->setItem(5, 6, new QTableWidgetItem(tr("3")));
    tableWidget->setItem(5, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
    tableWidget->setItem(5, 8, new QTableWidgetItem(tr("08.10.2010 8:32")));
    tableWidget->setItem(5, 9, new QTableWidgetItem(tr("552A0000-544AD000")));
    tableWidget->setItem(5, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));


    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->setMargin(2);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(tableWidget);
    setLayout(mainLayout);
}


/* ToolBoxDock */
ToolBoxDock::ToolBoxDock(QWidget* parent)
    : QFrame(parent)
{
    createWidgets();
}

static QIcon getIconToolBox(const QPixmap& pixmap, int index)
{
    QRect rcImage(index*16, 0, 16, 16);

    QImage copyImage = pixmap.copy(rcImage.left(), rcImage.top(), 16, 16).toImage();
    copyImage.setAlphaChannel(copyImage.createMaskFromColor(QColor(0, 255, 0).rgb(), Qt::MaskOutColor));
    return QIcon(QPixmap::fromImage(copyImage));
}

void ToolBoxDock::createWidgets()
{
    QToolBox* toolBox = new QToolBox();
    toolBox->layout()->setSpacing(0);

    QPixmap pixmap(":/res/toolbox.png");

    QListWidget* listWidget = new QListWidget;
    toolBox->addItem(listWidget, tr("PropertyPanes"));
    listWidget->insertItem(0, new QListWidgetItem(getIconToolBox(pixmap, 0), tr("Pointer")));

    listWidget = new QListWidget;
    toolBox->addItem(listWidget, tr("Data"));
    listWidget->insertItem(0, new QListWidgetItem(getIconToolBox(pixmap, 0), tr("Pointer")));
    listWidget->insertItem(1, new QListWidgetItem(getIconToolBox(pixmap, 26), tr("DataSet")));
    listWidget->insertItem(2, new QListWidgetItem(getIconToolBox(pixmap, 27), tr("OleDbDataAdapter")));
    listWidget->insertItem(3, new QListWidgetItem(getIconToolBox(pixmap, 28), tr("OleDbConnection")));
    listWidget->insertItem(4, new QListWidgetItem(getIconToolBox(pixmap, 29), tr("OleDbCommand")));
    listWidget->insertItem(5, new QListWidgetItem(getIconToolBox(pixmap, 30), tr("SqlDataAdapter")));
    listWidget->insertItem(6, new QListWidgetItem(getIconToolBox(pixmap, 31), tr("SqlConnection")));
    listWidget->insertItem(7, new QListWidgetItem(getIconToolBox(pixmap, 32), tr("SqlCommand")));
    listWidget->insertItem(8, new QListWidgetItem(getIconToolBox(pixmap, 33), tr("DataView")));

    listWidget = new QListWidget;
    toolBox->addItem(listWidget, tr("Components"));
    listWidget->insertItem(0, new QListWidgetItem(getIconToolBox(pixmap, 0), tr("Pointer")));
    listWidget->insertItem(1, new QListWidgetItem(getIconToolBox(pixmap, 34), tr("FileSystemWatcher")));
    listWidget->insertItem(2, new QListWidgetItem(getIconToolBox(pixmap, 35), tr("EventLog")));
    listWidget->insertItem(3, new QListWidgetItem(getIconToolBox(pixmap, 36), tr("DirectoryEntry")));
    listWidget->insertItem(4, new QListWidgetItem(getIconToolBox(pixmap, 37), tr("DirectorySearcher")));
    listWidget->insertItem(5, new QListWidgetItem(getIconToolBox(pixmap, 38), tr("MessageQueue")));
    listWidget->insertItem(7, new QListWidgetItem(getIconToolBox(pixmap, 39), tr("PerformanceCounter")));
    listWidget->insertItem(8, new QListWidgetItem(getIconToolBox(pixmap, 40), tr("Process")));
    listWidget->insertItem(9, new QListWidgetItem(getIconToolBox(pixmap, 41), tr("ServiceController")));
    listWidget->insertItem(10, new QListWidgetItem(getIconToolBox(pixmap, 42), tr("Timer")));

    listWidget = new QListWidget;
//    listWidget->setAttribute(Qt::WA_MouseTracking, true);
    toolBox->addItem(listWidget, tr("Dialog Editor"));
    listWidget->insertItem(0, new QListWidgetItem(getIconToolBox(pixmap, 14), tr("Progress Control")));
    listWidget->insertItem(1, new QListWidgetItem(getIconToolBox(pixmap, 15), tr("Hot Key")));
    listWidget->insertItem(2, new QListWidgetItem(getIconToolBox(pixmap, 16), tr("List Control")));
    listWidget->insertItem(3, new QListWidgetItem(getIconToolBox(pixmap, 17), tr("Tree Control")));
    listWidget->insertItem(4, new QListWidgetItem(getIconToolBox(pixmap, 18), tr("Tab Control")));
    listWidget->insertItem(5, new QListWidgetItem(getIconToolBox(pixmap, 19), tr("Animation Control")));
    listWidget->insertItem(6, new QListWidgetItem(getIconToolBox(pixmap, 20), tr("Rich Edit 2.0 Control")));
    listWidget->insertItem(7, new QListWidgetItem(getIconToolBox(pixmap, 21), tr("Data Time Picker")));
    listWidget->insertItem(8, new QListWidgetItem(getIconToolBox(pixmap, 22), tr("Month Calendar Control")));
    listWidget->insertItem(9, new QListWidgetItem(getIconToolBox(pixmap, 23), tr("IP Address Control")));
    listWidget->insertItem(10, new QListWidgetItem(getIconToolBox(pixmap, 24), tr("Extended Combo Box")));
    listWidget->insertItem(11, new QListWidgetItem(getIconToolBox(pixmap, 25), tr("Custom Control")));
    listWidget->insertItem(12, new QListWidgetItem(getIconToolBox(pixmap, 13), tr("Spin Control")));
//    listWidget->insertItem(13, new QListWidgetItem(getIconToolBox(pixmap, 13), tr("Progress Control")));

    listWidget = new QListWidget;
    toolBox->addItem(listWidget, tr("Clipboard Ring"));
    listWidget->insertItem(0, new QListWidgetItem(getIconToolBox(pixmap, 0), tr("Pointer")));

    listWidget = new QListWidget;
    toolBox->addItem(listWidget, tr("General"));
    listWidget->insertItem(0, new QListWidgetItem(getIconToolBox(pixmap, 0), tr("Pointer")));

    toolBox->setCurrentIndex(3);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(toolBox);
    setLayout(mainLayout);
}

/* TreeBoxDock */
TreeBoxDock::TreeBoxDock(QWidget* parent)
    : QFrame(parent)
{
    createWidgets();
}

void TreeBoxDock::createWidgets()
{
    QTreeWidget* treeWidget = new QTreeWidget;
    treeWidget->setMinimumWidth(190);
    treeWidget->setHeaderHidden(true);

    QStringList strings;
    strings.append(tr("Item 1"));
    QTreeWidgetItem* mainItem = new QTreeWidgetItem(strings);
    mainItem->setExpanded(false);
    mainItem->setIcon(0, QIcon(":/res/forward.png"));
    treeWidget->insertTopLevelItem(0, mainItem);

    strings.clear();
    strings.append(tr("Item 2"));
    QTreeWidgetItem* item = new QTreeWidgetItem(mainItem, strings);
    item->setIcon(0, QIcon(":/res/forward.png"));
    item->setExpanded(true);
    treeWidget->insertTopLevelItem(1, item);

    strings.clear();
    strings.append(tr("Item 3"));
    item = new QTreeWidgetItem(mainItem, strings);
    item->setIcon(0, QIcon(":/res/forward.png"));
    item->setExpanded(true);
    treeWidget->insertTopLevelItem(1, item);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(treeWidget);
    setLayout(mainLayout);
}