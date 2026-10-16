#include "mainwindow.h"

#include "qxribbon/ribbonbackstageview.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"

#include <QAction>
#include <QCheckBox>
#include <QIcon>
#include <QLabel>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>

QX_RIBBON_USE_NAMESPACE

namespace
{

/// A page of the backstage: a read-only text the host wants shown while the user
/// works on the document as a whole. The entry in the column takes its text from
/// the window title, so that is set before the page is added.
QTextEdit *makeTextPage(const QString &title, const QString &text)
{
    QTextEdit *page = new QTextEdit();
    page->setWindowTitle(title);
    page->setReadOnly(true);
    page->setPlainText(text);
    return page;
}

}   // namespace

MainWindow::MainWindow(QWidget *parent)
    : RibbonMainWindow(parent)
{
    QIcon icon(":/logo");

    auto *editor = new QTextEdit();
    editor->setPlainText(tr("Click the application button at the top left of the ribbon: the backstage takes "
                            "over the window with the document commands on the left and their pages on the "
                            "right. Esc - or the back button at the top of the column - brings the ribbon "
                            "back."));
    setCentralWidget(editor);

    RibbonBar *rb = ribbonBar();
    RibbonPage *page = rb->addPage(tr("Home"));
    RibbonGroup *group = page->addGroup(tr("Document"));
    group->addLargeAction(new QAction(icon, tr("Save"), this));
    group->addLargeAction(new QAction(icon, tr("Open"), this));

    // The backstage belongs to the ribbon bar and covers the bar's host, which
    // for this window is the window itself.
    m_backstage = new RibbonBackstageView(rb);

    QAction *saveAction = m_backstage->addAction(icon, tr("Save"));
    QAction *openAction = m_backstage->addAction(icon, tr("Open"));
    m_backstage->addSeparator();
    m_backstage->addAction(icon, tr("Print"));

    QTextEdit *infoPage = makeTextPage(tr("Info"), tr("The Info page.\n\n"
                                                      "A page is any widget the host hands over: the view "
                                                      "reparents it, keeps it hidden while another page is up, "
                                                      "and sizes it to the area right of the column."));
    m_backstage->addPage(infoPage);

    QWidget *optionsPage = new QWidget();
    optionsPage->setWindowTitle(tr("Options"));
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsPage);
    optionsLayout->addWidget(new QLabel(tr("The Options page.")));
    QCheckBox *preventClose = new QCheckBox(tr("Prevent the backstage from closing"));
    optionsLayout->addWidget(preventClose);
    optionsLayout->addStretch();
    m_backstage->addPage(optionsPage);

    QTextEdit *aboutPage = makeTextPage(tr("About"), tr("The About page.\n\n"
                                                        "Clicking an entry in the column shows its page and "
                                                        "checks the entry; the one before it is hidden and "
                                                        "unchecked."));
    m_backstage->addPage(aboutPage);

    m_backstage->addSeparator();
    QAction *exitAction = m_backstage->addAction(icon, tr("Exit"));

    // Nothing is up until the host says what should be.
    m_backstage->setActivePage(infoPage);

    auto hint = [this](const QString &text) {
        statusBar()->showMessage(text, 3000);
    };
    connect(saveAction, &QAction::triggered, this, [hint] {
        hint(tr("Save triggered"));
    });
    connect(openAction, &QAction::triggered, this, [hint] {
        hint(tr("Open triggered"));
    });
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
    connect(preventClose, &QCheckBox::toggled, m_backstage, &RibbonBackstageView::setClosePrevented);
    connect(m_backstage, &RibbonBackstageView::aboutToShow, this, [hint] {
        hint(tr("Backstage open"));
    });
    connect(m_backstage, &RibbonBackstageView::aboutToHide, this, [hint] {
        hint(tr("Backstage closed"));
    });

    resize(960, 600);
}

MainWindow::~MainWindow()
{
    // The backstage answers the teardown of its host like any other child: it is
    // hidden along with the window, which emits aboutToHide, and the slots here
    // reach into the status bar - a widget that is already gone by then. Cutting
    // the connections before the base class takes the children down keeps the
    // hints from firing into a half-destroyed window.
    disconnect(m_backstage, nullptr, this, nullptr);
}
