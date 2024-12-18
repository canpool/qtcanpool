#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonbutton.h"
#include "qxribbon/ribbonmenu.h"
#include "qxribbon/ribboncontainers.h"
#include "qxribbon/ribboncolorbutton.h"

#include <QAction>
#include <QToolBar>
#include <QColorDialog>

#include "colorbutton.h"

QX_RIBBON_USE_NAMESPACE

struct colorPreviewRect
{
    QColor clr;
    QString caption;
};

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    QIcon icon(":/logo");

    RibbonBar *rb = ribbonBar();

    RibbonPage *page = rb->addPage(tr("page1"));
    RibbonGroup *group = page->addGroup(tr("group1"));
    RibbonButton *btn = group->addLargeAction(new QAction(icon, tr("La"), this));
    btn->setPopupMode(QToolButton::InstantPopup);
    RibbonMenu *menu = new RibbonMenu(this);
    btn->setMenu(menu);
    QToolBar *tb = new QToolBar(this);
    for (int i = 1; i <= 5; ++i) {
        tb->addAction(icon, tr("action%1").arg(i));
    }
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    menu->addWidget(tb);
    menu->addAction(tr("action3"));
    menu->addSeparator();
    menu->addAction(tr("action4"));
    connect(tb, &QToolBar::actionTriggered, this, [menu]() {
        menu->hide();
    });

    RibbonGridContainer *container = nullptr;
    container = new RibbonGridContainer(this);
    container->setColumnCount(10);
    for (int i = 1; i <= 28; ++i) {
        RibbonButton *btn = new RibbonButton(this);
        btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        btn->setIcon(icon);
        btn->setText(tr("button%1").arg(i));
        container->addButton(btn);
    }
    menu->addWidget(container);
    menu->addAction(tr("action5"));

    RibbonColorButton *rcb = new RibbonColorButton(this);
    rcb->setIcon(icon);
    rcb->setText(tr("color button"));
    rcb->setButtonType(RibbonButton::LargeButton);
    rcb->setPopupMode(QToolButton::InstantPopup);
    rcb->setColor(Qt::blue);
    group->addLargeWidget(rcb);
    menu = new RibbonMenu(this);
    rcb->setMenu(menu);
    container = new RibbonGridContainer(this);
    container->setColumnCount(10);

    QList<colorPreviewRect> standardColors({
        {QColor(192, 0,   0),   QObject::tr("Dark Red")},
        {QColor(255, 0,   0),   QObject::tr("Red")},
        {QColor(255, 192, 0),   QObject::tr("Orange")},
        {QColor(255, 255, 0),   QObject::tr("Yellow")},
        {QColor(146, 208, 80),  QObject::tr("Light Green")},
        {QColor(0,   176, 80),  QObject::tr("Green")},
        {QColor(0,   176, 240), QObject::tr("Light Blue")},
        {QColor(0,   112, 192), QObject::tr("Blue")},
        {QColor(0,   32,  96),  QObject::tr("Dark Blue")},
        {QColor(112, 48,  160), QObject::tr("Purple")}
    });
    for (colorPreviewRect c : standardColors) {
        ColorButton *cb = createColorButton(c.clr, c.caption);
        container->addButton(cb);
        connect(cb, &ColorButton::colorClicked, this, [rcb](const QColor &color, bool) {
           rcb->setColor(color);
        });
    }
    QList<int> factors {180, 160, 140, 75, 50};
    for (int factor : factors) {
        for (colorPreviewRect c : standardColors) {
            ColorButton *cb = createColorButton(c.clr.lighter(factor), c.caption);
            container->addButton(cb);
            connect(cb, &ColorButton::colorClicked, this, [rcb](const QColor &color, bool) {
               rcb->setColor(color);
            });
        }
    }
    menu->addAction(tr("action1"));
    menu->addWidget(container);
    menu->addSeparator();
    QAction *moreAct = menu->addAction(tr("More Colors..."));
    connect(moreAct, &QAction::triggered, this, [rcb]() {
        QColorDialog dlg;
        if (dlg.exec() == QDialog::Accepted) {
            rcb->setColor(dlg.selectedColor());
        }
    });


    page = rb->addPage(tr("loooooooooooongpage2"));

    resize(800, 400);
}

MainWindow::~MainWindow()
{
}

ColorButton *MainWindow::createColorButton(const QColor &color, const QString &text)
{
    ColorButton *cb = new ColorButton(color);
    cb->setText(text);
    return cb;
}
