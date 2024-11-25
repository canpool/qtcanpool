#include "tst_global.h"

#include "qxdock/dockwidget.h"

QX_DOCK_USE_NAMESPACE

class tst_DockWidget : public QObject
{
    Q_OBJECT
private slots:
    void feature();
};

void tst_DockWidget::feature()
{
    DockWidget dw("dock1");
    QCOMPARE(DockWidget::DefaultDockWidgetFeatures, dw.features());
}

TEST_ADD(tst_DockWidget)

#include "tst_dockwidget.moc"
