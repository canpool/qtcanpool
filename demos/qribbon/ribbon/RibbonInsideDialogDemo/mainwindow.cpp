#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QLayout>
#include <QPushButton>
#include <qevent.h>

QRIBBON_USE_NAMESPACE

/* MainDialog */
MainDialog::MainDialog(QWidget *parent) : QDialog(parent)
{
    m_dlgRibbonBar = new RibbonBar(this);
    m_dlgRibbonBar->addPage(QObject::tr("Home"));

    QLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(m_dlgRibbonBar);
    layout->setAlignment(m_dlgRibbonBar, Qt::AlignTop);
    this->setLayout(layout);

    QRect geom = QApplication::desktop()->availableGeometry();
    move(QPoint(200, 200));
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);
}

MainDialog::~MainDialog()
{
}
