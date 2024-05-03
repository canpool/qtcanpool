#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include "ribbondockwindow.h"

/* RibbonDockWindow */
RibbonDockWindow::RibbonDockWindow(QWidget *parent)
    : Qtitan::DockMainWindow(parent)
{
    setWindowTitle("QtitanRibbonDocking - Simple Dock Panels");
    RibbonBar *ribbonBar = new RibbonBar(this);
    setMenuBar(ribbonBar);
    ribbonBar->addPage(tr("&Buttons_1"));
    ribbonBar->addPage(tr("&Buttons_2"));

    dockPanelManager()->setCentralWidget(new QTextEdit);
    DockWidgetPanel *panel_1 = dockPanelManager()->addDockPanel(tr("Panel 1"), Qtitan::LeftDockPanelArea);
    panel_1->setWidget(new QTextEdit);
    panel_1->setIcon(QIcon(":/res/resource_view.ico"));

    DockWidgetPanel *panel_2 = dockPanelManager()->addDockPanel(tr("Panel 2"), Qtitan::RightDockPanelArea);
    panel_2->setWidget(new QTextEdit);
    panel_2->setIcon(QIcon(":/res/web.ico"));

    DockWidgetPanel *panel_3 = dockPanelManager()->addDockPanel(tr("Panel 3"), Qtitan::BottomDockPanelArea);
    panel_3->setWidget(new QTextEdit);
    panel_3->setIcon(QIcon(":/res/xml.ico"));

    ribbonBar->setFrameThemeEnabled();

    QRect geom = QApplication::desktop()->availableGeometry(this);
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);

    VisualStudio2019Style *style = new VisualStudio2019Style();
    style->setTheme(VisualStudio2019Style::Light);
    qApp->setStyle(style);
}

RibbonDockWindow::~RibbonDockWindow()
{
}
