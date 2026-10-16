#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"

#include <QAction>
#include <QIcon>
#include <QTextEdit>
#include <QResizeEvent>
#include <QStatusBar>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonMainWindow(parent)
{
    QIcon icon(":/logo");

    auto *editor = new QTextEdit();
    editor->setPlainText(tr("Narrow this window to watch the ribbon groups "
                            "shrink from the right before the page-level scroll "
                            "buttons appear. The status bar shows each group's "
                            "RibbonGroup::isReduced() state as it changes."));
    setCentralWidget(editor);

    RibbonBar *rb = ribbonBar();
    RibbonPage *page = rb->addPage(tr("Home"));

    // Each group carries at least one large button, so it owns a whole column
    // and therefore has a reduced presentation (B11: a group narrows itself
    // before the page falls back to its scroll buttons).
    struct GroupSpec {
        QString title;
        int large;
        int small;
    };
    const GroupSpec specs[] = {
        {tr("Clipboard"), 1, 2}, {tr("Font"), 2, 1}, {tr("Paragraph"), 1, 3}, {tr("Styles"), 3, 0}, {tr("View"), 1, 2},
    };
    for (const GroupSpec &s : specs) {
        RibbonGroup *g = page->addGroup(s.title);
        for (int i = 0; i < s.large; ++i)
            g->addLargeAction(new QAction(icon, tr("Large %1").arg(i + 1), this));
        for (int i = 0; i < s.small; ++i)
            g->addSmallAction(new QAction(icon, tr("Small %1").arg(i + 1), this));
        m_groups.append(g);
        m_titles.append(s.title);
    }

    resize(960, 480);
    updateReducedReadout();
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateReducedReadout()
{
    QStringList parts;
    for (int i = 0; i < m_groups.count(); ++i)
        parts.append(QString::fromLatin1("%1:%2").arg(m_titles.at(i), m_groups.at(i)->isReduced()
                                                                          ? QString::fromLatin1("reduced")
                                                                          : QString::fromLatin1("full")));
    statusBar()->showMessage(tr("group state  ->  ") + parts.join(QString::fromLatin1("   ")));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    RibbonMainWindow::resizeEvent(event);
    updateReducedReadout();
}
