#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonmdiarea.h"
#include "qxribbon/ribbonpage.h"

#include <QAction>
#include <QIcon>
#include <QKeySequence>
#include <QLabel>
#include <QMdiSubWindow>
#include <QStatusBar>
#include <QStringList>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonMainWindow(parent)
    , m_mdiArea(Q_NULLPTR)
    , m_textPage(Q_NULLPTR)
    , m_sketchPage(Q_NULLPTR)
    , m_textCount(0)
    , m_sketchCount(0)
{
    setWindowTitle(tr("MDI Tabs Example"));
    resize(900, 600);

    RibbonBar *rb = ribbonBar();

    // The start page belongs to no document, so the area never manages it: it
    // stays up on its own terms and is what the user sees before any document
    // is active.
    RibbonPage *home = rb->addPage(tr("Home"));
    createFileGroup(home);
    createWindowGroup(home);

    createDocumentPages();

    m_mdiArea = new RibbonMdiArea(this);
    setCentralWidget(m_mdiArea);
    connect(m_mdiArea, &QMdiArea::subWindowActivated, this, [this]() {
        updateStatus();
    });

    // Open one document so there is something to switch away from. It is
    // added before the window is shown, and showing the area is what makes
    // the first document the active one.
    newDocument(false);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createFileGroup(RibbonPage *page)
{
    RibbonGroup *g = page->addGroup(tr("File"));
    QIcon icon(QStringLiteral(":/logo"));

    QAction *newText = new QAction(icon, tr("New Text"), this);
    newText->setShortcut(QKeySequence::New);
    connect(newText, &QAction::triggered, this, [this]() {
        newDocument(false);
    });
    g->addLargeAction(newText);

    QAction *newSketch = new QAction(icon, tr("New Sketch"), this);
    connect(newSketch, &QAction::triggered, this, [this]() {
        newDocument(true);
    });
    g->addLargeAction(newSketch);

    QAction *save = new QAction(icon, tr("Save"), this);
    save->setShortcut(QKeySequence::Save);
    connect(save, &QAction::triggered, this, [this]() {
        QMdiSubWindow *active = m_mdiArea->activeSubWindow();
        statusBar()->showMessage(active ? tr("Saved '%1'").arg(active->windowTitle()) : tr("Nothing to save"));
    });
    g->addLargeAction(save);

    QAction *close = new QAction(icon, tr("Close"), this);
    close->setShortcut(QKeySequence::Close);
    connect(close, &QAction::triggered, m_mdiArea, &QMdiArea::closeActiveSubWindow);
    g->addLargeAction(close);
}

void MainWindow::createWindowGroup(RibbonPage *page)
{
    RibbonGroup *g = page->addGroup(tr("Window"));

    QAction *tile = new QAction(tr("Tile"), this);
    connect(tile, &QAction::triggered, m_mdiArea, &QMdiArea::tileSubWindows);
    g->addLargeAction(tile);

    QAction *cascade = new QAction(tr("Cascade"), this);
    connect(cascade, &QAction::triggered, m_mdiArea, &QMdiArea::cascadeSubWindows);
    g->addLargeAction(cascade);

    QAction *closeAll = new QAction(tr("Close All"), this);
    connect(closeAll, &QAction::triggered, this, [this]() {
        // Close from the back so the list does not shift under the loop.
        const QList<QMdiSubWindow *> subs = m_mdiArea->subWindowList();
        for (int i = subs.size() - 1; i >= 0; --i)
            subs[i]->close();
    });
    g->addLargeAction(closeAll);

    QAction *tabbed = new QAction(tr("Tabbed Documents"), this);
    tabbed->setCheckable(true);
    tabbed->setChecked(true);
    connect(tabbed, &QAction::triggered, this, [this](bool checked) {
        m_mdiArea->setTabbedDocuments(checked);
        updateStatus();
    });
    g->addLargeAction(tabbed);
}

void MainWindow::createDocumentPages()
{
    RibbonBar *rb = ribbonBar();

    // The text page edits whatever document is active - the page swap and
    // "the page acts on the active document" are the same mechanism seen
    // from two sides.
    m_textPage = rb->addPage(tr("Text"));
    RibbonGroup *edit = m_textPage->addGroup(tr("Edit"));
    const QStringList editActions = {QStringLiteral("undo"), QStringLiteral("redo"), QStringLiteral("cut"),
                                     QStringLiteral("copy"), QStringLiteral("paste")};
    for (const QString &name : editActions) {
        QAction *action = new QAction(name, this);
        connect(action, &QAction::triggered, this, [this, name]() {
            QTextEdit *editor = activeEditor();
            if (!editor) {
                statusBar()->showMessage(tr("No active text document"));
                return;
            }
            if (name == QStringLiteral("undo"))
                editor->undo();
            else if (name == QStringLiteral("redo"))
                editor->redo();
            else if (name == QStringLiteral("cut"))
                editor->cut();
            else if (name == QStringLiteral("copy"))
                editor->copy();
            else
                editor->paste();
        });
        edit->addLargeAction(action);
    }

    // The sketch page stands in for a canvas: drawing is not what this
    // example is about, so the tools only report themselves.
    m_sketchPage = rb->addPage(tr("Sketch"));
    RibbonGroup *shapes = m_sketchPage->addGroup(tr("Shapes"));
    const QStringList shapeTools = {QStringLiteral("Rectangle"), QStringLiteral("Ellipse")};
    for (const QString &shape : shapeTools) {
        QAction *action = new QAction(shape, this);
        connect(action, &QAction::triggered, this, [this, shape]() {
            statusBar()->showMessage(tr("%1 tool selected").arg(shape));
        });
        shapes->addLargeAction(action);
    }
}

void MainWindow::newDocument(bool sketch)
{
    QWidget *content = Q_NULLPTR;
    QString title;
    RibbonPage *page = Q_NULLPTR;

    if (sketch) {
        QWidget *canvas = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(canvas);
        QLabel *label = new QLabel(tr("Sketch canvas (stand-in)"));
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        content = canvas;
        title = tr("Sketch %1").arg(++m_sketchCount);
        page = m_sketchPage;
    } else {
        content = new QTextEdit;
        title = tr("Text %1").arg(++m_textCount);
        page = m_textPage;
    }

    // Handing the page over is the whole wiring: from here on the area shows
    // it while this document is the active one and hides it otherwise.
    m_mdiArea->addDocument(content, title, page);
    updateStatus();
}

void MainWindow::updateStatus()
{
    QMdiSubWindow *active = m_mdiArea->activeSubWindow();
    statusBar()->showMessage(
        active ? tr("Active document: %1 (%2 open)").arg(active->windowTitle()).arg(m_mdiArea->subWindowList().size())
               : tr("No active document (%1 open)").arg(m_mdiArea->subWindowList().size()));
}

QTextEdit *MainWindow::activeEditor() const
{
    QMdiSubWindow *active = m_mdiArea->activeSubWindow();
    if (!active)
        return Q_NULLPTR;
    return qobject_cast<QTextEdit *>(active->widget());
}
