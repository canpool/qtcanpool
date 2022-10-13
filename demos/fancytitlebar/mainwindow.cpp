#include "mainwindow.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>

#include "qcanpool/fancytitlebar.h"

QCANPOOL_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setWindowFlags(Qt::WindowSystemMenuHint |
                Qt::WindowMinimizeButtonHint |
                Qt::WindowMaximizeButtonHint |
                Qt::FramelessWindowHint |
                Qt::Window
                );

    QWidget *titleWidget = new QWidget();
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(250, 100, 100));
    titleWidget->setPalette(palette);
    titleWidget->setAutoFillBackground(true);
    titleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    FancyTitleBar *titleBar = new FancyTitleBar(this);
    titleBar->setTitleWidget(titleWidget);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(1, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(titleBar->logoButton());
    layout->addStretch();
    layout->addWidget(titleBar->titleLabel());
    layout->addStretch();
    layout->addWidget(titleBar->systemToolBar());
    titleWidget->setLayout(layout);

    setMenuWidget(titleWidget);
    setMouseTracking(true);
    raise();
    activateWindow();

    setWindowTitle(tr("fancytitlebar demo"));

    titleBar->updateWidgetFlags();
    resize(QSize(600, 300));
}

MainWindow::~MainWindow()
{
}

