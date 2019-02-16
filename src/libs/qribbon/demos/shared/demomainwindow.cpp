
#include <QResizeEvent>
#include <QPainter>
#include <QHBoxLayout>

#include "demomainwindow.h"
#include "aboutdialog.h"

/* DemoMainWindow */
DemoMainWindow::DemoMainWindow(const QString& productName, const QString& version)
    : QWidget(), m_infoButton(0), m_demoWidget(0), m_productName(productName), m_version(version), hl(Q_NULL)
{
}

void DemoMainWindow::aboutLinkActivated(const QString& link)
{
    Q_UNUSED(link);
    Qtitan::AboutDialog::show(this, m_productName + " " + tr("Demo"), m_productName, m_version);
}

void DemoMainWindow::setDemoWidget(QWidget* widget, QWidget* settings, Qt::Alignment alignment)
{
    m_demoWidget = widget;
    m_settings = settings;
    QVBoxLayout* vl = new QVBoxLayout(this);
    hl = new QHBoxLayout();
    m_infoButton = new QLabel(this);
    m_infoButton->setCursor(Qt::PointingHandCursor);
    m_infoButton->setText("<a href=\"#\"><img src=\":res/question.png\"></a>");
    connect(m_infoButton, SIGNAL(linkActivated(const QString&)), this, SLOT(aboutLinkActivated(const QString&)));
    vl->addWidget(m_infoButton, false, Qt::AlignRight);
    hl->addWidget(m_demoWidget, m_settings != 0 ? 1 : 0);
    if (m_settings != 0)
        hl->addWidget(m_settings, 0, alignment);
    hl->setContentsMargins(20, 40, 20, 20);
    vl->addSpacing(5);
    vl->setContentsMargins(5, 5, 5, 5);
    vl->addLayout(hl, true);
    m_demoWidget->raise();
}

void DemoMainWindow::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    QRect r(QPoint(0, 0), size());
    r.setHeight(80);
    painter.fillRect(r, QBrush(QPixmap(":/res/topheader.png")));
    painter.drawPixmap(10, 0, QPixmap(":/res/qtitanlogo64x64.png"));

    r.setTop(r.height());
    r.setHeight(size().height() - r.height());
    painter.fillRect(r, QColor(0xFFFFFFFF));

    painter.save();
    QRect borderRect = hl->contentsRect().adjusted(-10, -10, 10, 10);
    painter.setOpacity(0.3);
    QLinearGradient linearGrad(QPointF(200, 100), QPointF(500, 300));
    linearGrad.setColorAt(0, Qt::darkGray);
    linearGrad.setColorAt(1, Qt::lightGray);
    painter.fillRect(borderRect, linearGrad);
    painter.setOpacity(1);
    painter.restore();

    QFont f = painter.font();
    f.setPixelSize(20);
    f.setBold(true);
    painter.setFont(f);
    painter.setPen(QColor(0xFFFFFFFF));
    painter.drawText(84, 40, m_productName + " " + m_version);

}
