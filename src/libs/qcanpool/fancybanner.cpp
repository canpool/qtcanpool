/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#include "fancybanner.h"

#include <QMouseEvent>
#include <QPainter>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColor>
#include <QPropertyAnimation>
#include <QDebug>
#include <QTimer>
#include <QStackedLayout>

static QColor s_frontColor = QColor(220, 0, 0);
static QColor s_backColor = QColor(200, 200, 200);

///
/// \brief The FancyBannerIndicator class
///
class FancyBannerIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit FancyBannerIndicator(QWidget *parent = nullptr);
    ~FancyBannerIndicator();

    void select(bool selected);

signals:
    void entered();

protected:
    void enterEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool m_selected;
};


FancyBannerIndicator::FancyBannerIndicator(QWidget *parent)
    : QWidget(parent)
{
    m_selected = false;
}

FancyBannerIndicator::~FancyBannerIndicator()
{
}

void FancyBannerIndicator::select(bool selected)
{
    m_selected = selected;
    update();
}

void FancyBannerIndicator::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    emit entered();
}

void FancyBannerIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    if (m_selected) {
        painter.setBrush(QBrush(s_frontColor));
    } else {
        painter.setBrush(QBrush(s_backColor));
    }

    painter.drawRect(rect());
}

class FancyBannerArrow : public QWidget
{
    Q_OBJECT

public:
    enum ArrowType {Left, Right};
    explicit FancyBannerArrow(ArrowType type, QWidget *parent = nullptr);
    ~FancyBannerArrow();

signals:
    void clicked();

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool m_bMouseHover;
    bool m_bMousePress;
    ArrowType m_type;
};


FancyBannerArrow::FancyBannerArrow(ArrowType type, QWidget *parent)
    : QWidget(parent)
{
    m_bMouseHover = false;
    m_bMousePress = false;
    m_type = type;
    setFixedSize(12, 20);
}

FancyBannerArrow::~FancyBannerArrow()
{
}

void FancyBannerArrow::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bMouseHover = true;
    update();
}

void FancyBannerArrow::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bMouseHover = false;
    update();
}

void FancyBannerArrow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = true;
        emit clicked();
    }
}

void FancyBannerArrow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = false;
    }
}

void FancyBannerArrow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    pen.setWidth(3);

    if (m_bMouseHover) {
        pen.setBrush(QColor(255, 255, 255));
    } else {
        pen.setBrush(QColor(150, 150, 150));
    }

    painter.setPen(pen);
    int margin = 2;

    switch (m_type) {
        case Right: {
            painter.drawLine(QPointF(margin, margin), QPointF(this->width() - margin, this->height() / 2));
            painter.drawLine(QPointF(margin, this->height() - margin), QPointF(this->width() - margin, this->height() / 2));
            break;
        }
        case Left: {
            painter.drawLine(QPointF(this->width() - margin, margin), QPointF(margin, this->height() / 2));
            painter.drawLine(QPointF(this->width() - margin, this->height() - margin), QPointF(margin, this->height() / 2));
            break;
        }
    }

    QWidget::paintEvent(event);
}

///
/// \brief The FancyBannerPage class
///
class FancyBannerPage : public QLabel
{
    Q_OBJECT
public:
    explicit FancyBannerPage(QWidget *parent = nullptr);
    ~FancyBannerPage();

    void setActive(bool active);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool m_bActive;
};

FancyBannerPage::FancyBannerPage(QWidget *parent)
    : QLabel(parent)
{
    m_bActive = false;
}

FancyBannerPage::~FancyBannerPage()
{
}

void FancyBannerPage::setActive(bool active)
{
    m_bActive = active;
}

void FancyBannerPage::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

void FancyBannerPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QLabel::paintEvent(event);

    if (!m_bActive) {
        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 150));
        painter.drawRect(rect());
    }
}

///
/// \brief The FancyBannerView class
///
class FancyBannerView : public QWidget
{
    Q_OBJECT
public:
    explicit FancyBannerView(QWidget *parent = nullptr);
    ~FancyBannerView();

    void setLeftPixmap(const QPixmap &pixmap);
    void setCenterPixmap(const QPixmap &pixmap);
    void setRightPixmap(const QPixmap &pixmap);

    void startAnimation(int direction);

signals:
    void clicked(int flag);

private:
    void setPagePixmap(FancyBannerPage *page, const QPixmap &pixmap);
    void setArrowHidden(bool hidden);

private slots:
    void slotPageClicked();
    void slotArrowClicked();

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    FancyBannerPage *m_leftPage;
    FancyBannerPage *m_centerPage;
    FancyBannerPage *m_rightPage;
    FancyBannerArrow *m_leftArrow;
    FancyBannerArrow *m_rightArrow;

    QRect m_centerRect;
    QRect m_leftRect;
    QRect m_rightRect;
};

#define PAGE_WIDTH          500
#define PAGE_HEIGHT         200
#define PAGE_WIDTH_OFFSET   130
#define PAGE_HEIGHT_OFFSET  12

FancyBannerView::FancyBannerView(QWidget *parent)
    : QWidget(parent)
{
    m_leftPage = new FancyBannerPage(this);
    m_centerPage = new FancyBannerPage(this);
    m_centerPage->setActive(true);
    m_rightPage = new FancyBannerPage(this);
    connect(m_leftPage, SIGNAL(clicked()), this, SLOT(slotPageClicked()));
    connect(m_centerPage, SIGNAL(clicked()), this, SLOT(slotPageClicked()));
    connect(m_rightPage, SIGNAL(clicked()), this, SLOT(slotPageClicked()));

//    this->setFixedSize(PAGE_WIDTH+PAGE_WIDTH_OFFSET*2+10,PAGE_HEIGHT);
    this->setMinimumWidth(PAGE_WIDTH + PAGE_WIDTH_OFFSET * 2 + 10);
    this->setFixedHeight(PAGE_HEIGHT);
    m_centerPage->setFixedSize(PAGE_WIDTH, PAGE_HEIGHT);
    m_leftPage->setFixedWidth(PAGE_WIDTH);
    m_leftPage->setFixedHeight(PAGE_HEIGHT - PAGE_HEIGHT_OFFSET);
    m_rightPage->setFixedWidth(PAGE_WIDTH);
    m_rightPage->setFixedHeight(PAGE_HEIGHT - PAGE_HEIGHT_OFFSET);

    m_leftArrow = new FancyBannerArrow(FancyBannerArrow::Left, this);
    m_rightArrow = new FancyBannerArrow(FancyBannerArrow::Right, this);
    setArrowHidden(true);
    connect(m_leftArrow, SIGNAL(clicked()), this, SLOT(slotArrowClicked()));
    connect(m_rightArrow, SIGNAL(clicked()), this, SLOT(slotArrowClicked()));

    const int x = this->x() + this->width() / 2 - PAGE_WIDTH / 2 - PAGE_WIDTH_OFFSET;
    const int y = this->y();
    m_leftPage->move(x, y + PAGE_HEIGHT_OFFSET);
    m_centerPage->move(x + PAGE_WIDTH_OFFSET, y);
    m_rightPage->move(x + PAGE_WIDTH_OFFSET * 2, y + PAGE_HEIGHT_OFFSET);
    m_centerPage->raise();

    m_centerRect = m_centerPage->geometry();
    m_leftRect = m_leftPage->geometry();
    m_rightRect = m_rightPage->geometry();
}

FancyBannerView::~FancyBannerView()
{
}

void FancyBannerView::setLeftPixmap(const QPixmap &pixmap)
{
    setPagePixmap(m_leftPage, pixmap);
}

void FancyBannerView::setCenterPixmap(const QPixmap &pixmap)
{
    setPagePixmap(m_centerPage, pixmap);
}

void FancyBannerView::setRightPixmap(const QPixmap &pixmap)
{
    setPagePixmap(m_rightPage, pixmap);
}

void FancyBannerView::startAnimation(int direction)
{
    QRect tmpRect = QRect(m_centerRect.x(), m_centerRect.y() + 12,
                          m_centerRect.width(), m_centerRect.height() - 12);
    QPropertyAnimation *m_leftAnimation = new QPropertyAnimation(m_leftPage, "geometry");
    QPropertyAnimation *m_centerAnimation = new QPropertyAnimation(m_centerPage, "geometry");
    QPropertyAnimation *m_rightAnimation = new QPropertyAnimation(m_rightPage, "geometry");
    m_leftAnimation->setDuration(200);
    m_centerAnimation->setDuration(250);
    m_rightAnimation->setDuration(200);

    if (direction < 0) {
        m_centerAnimation->setStartValue(m_leftRect);
        m_centerAnimation->setEndValue(m_centerRect);
        m_rightPage->raise();
    } else if (direction > 0) {
        m_centerAnimation->setStartValue(m_rightRect);
        m_centerAnimation->setEndValue(m_centerRect);
        m_leftPage->raise();
    }

    m_leftAnimation->setStartValue(tmpRect);
    m_leftAnimation->setEndValue(m_leftRect);
    m_rightAnimation->setStartValue(tmpRect);
    m_rightAnimation->setEndValue(m_rightRect);
    m_centerPage->raise();
    m_leftArrow->raise();
    m_rightArrow->raise();
    m_leftAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    m_rightAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    m_centerAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void FancyBannerView::setPagePixmap(FancyBannerPage *page, const QPixmap &pixmap)
{
    if (page) {
        page->setPixmap(pixmap.scaled(QSize(PAGE_WIDTH, PAGE_HEIGHT), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
}

void FancyBannerView::setArrowHidden(bool hidden)
{
    m_leftArrow->setHidden(hidden);
    m_rightArrow->setHidden(hidden);

    if (!hidden) {
        m_leftArrow->move(m_leftRect.x() + 8, m_leftRect.y() + (m_leftRect.height() - m_leftArrow->height()) / 2);
        m_rightArrow->move(m_rightRect.x() + m_rightRect.width() - m_rightArrow->width() - 8,
                           m_rightRect.y() + (m_rightRect.height() - m_rightArrow->height()) / 2);
        m_leftArrow->raise();
        m_rightArrow->raise();
    }
}

void FancyBannerView::slotPageClicked()
{
    FancyBannerPage *page = qobject_cast<FancyBannerPage *>(sender());

    if (page == nullptr) {
        return;
    }

    int flag = 0;

    if (page == m_leftPage) {
        flag = -1;
    } else if (page == m_rightPage) {
        flag = 1;
    }

    emit clicked(flag);
}

void FancyBannerView::slotArrowClicked()
{
    FancyBannerArrow *arrow = qobject_cast<FancyBannerArrow *>(sender());

    if (arrow == nullptr) {
        return;
    }

    if (arrow == m_leftArrow) {
        emit clicked(-1);
    } else if (arrow == m_rightArrow) {
        emit clicked(1);
    }
}

void FancyBannerView::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    setArrowHidden(false);
}

void FancyBannerView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    setArrowHidden(true);
}

void FancyBannerView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
//    QPainter painter(this);
//    painter.setBrush(QBrush(QColor(255,0,0, 100)));
//    painter.drawRect(rect());
    QWidget::paintEvent(event);
}

///
/// \brief The FancyBannerPrivate class
///
class FancyBannerPrivate : public QObject
{
    Q_OBJECT
public:
    FancyBannerPrivate();
    ~FancyBannerPrivate();

    void init();
    void addPage(QPixmap pixmap);
    void addPages(QList<QPixmap> pixmaps);

    void play(int index);

public slots:
    void switchIndicator();
    void switchPage(int flag);
    void slotTimeOut();

public:
    FancyBanner *q;
    FancyBannerView *m_view;
    QHBoxLayout *m_indLayout;
    QList<QPixmap> m_pixmaps;
    QList<FancyBannerIndicator *> m_indicators;
    int m_currentIndex;
    QTimer *m_timer;
    int m_interval;
};

FancyBannerPrivate::FancyBannerPrivate()
{
    q = nullptr;
    m_view = nullptr;
    m_indLayout = nullptr;
    m_indicators.clear();
    m_pixmaps.clear();
    m_currentIndex = -1;
    m_timer = nullptr;
    m_interval = 2000;
}

FancyBannerPrivate::~FancyBannerPrivate()
{
}

void FancyBannerPrivate::init()
{
    m_indLayout = new QHBoxLayout();
    m_indLayout->setSpacing(5);
    m_view = new FancyBannerView();
    connect(m_view, SIGNAL(clicked(int)), this, SLOT(switchPage(int)));
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(0);
    bottomLayout->setMargin(0);
    bottomLayout->addStretch();
    bottomLayout->addLayout(m_indLayout);
    bottomLayout->addStretch();
    QHBoxLayout *viewLayout = new QHBoxLayout();
    viewLayout->setSpacing(0);
    viewLayout->setMargin(0);
    viewLayout->addStretch();
    viewLayout->addWidget(m_view);
    viewLayout->addStretch();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addStretch();
    layout->addLayout(viewLayout);
    layout->addLayout(bottomLayout);
    layout->addStretch();
    q->setLayout(layout);
}

void FancyBannerPrivate::addPage(QPixmap pixmap)
{
    m_pixmaps.append(pixmap);
    FancyBannerIndicator *ind = new FancyBannerIndicator();
    ind->setFixedSize(18, 3);
    connect(ind, SIGNAL(entered()), this, SLOT(switchIndicator()));
    m_indicators.append(ind);
    m_indLayout->addWidget(ind);

    if (m_currentIndex == -1) {
        m_currentIndex = 0;
        m_indicators.at(m_currentIndex)->select(true);
    }

    play(m_currentIndex);
}

void FancyBannerPrivate::addPages(QList<QPixmap> pixmaps)
{
    foreach (QPixmap pixmap, pixmaps) {
        addPage(pixmap);
    }
}

void FancyBannerPrivate::play(int index)
{
    if (index < 0) {
        return;
    }

    int count = m_indicators.count();
    int leftIndex = 0, rightIndex = 0;

    if (count > 1) {
        if (index == 0) {
            leftIndex = count - 1;
            rightIndex = index + 1;
        } else if (index == count - 1) {
            leftIndex = index - 1;
            rightIndex = 0;
        } else {
            leftIndex = index - 1;
            rightIndex = index + 1;
        }
    }

    m_view->setLeftPixmap(m_pixmaps.at(leftIndex));
    m_view->setCenterPixmap(m_pixmaps.at(index));
    m_view->setRightPixmap(m_pixmaps.at(rightIndex));
}

void FancyBannerPrivate::switchIndicator()
{
    FancyBannerIndicator *ind = qobject_cast<FancyBannerIndicator *>(sender());

    if (ind == nullptr) {
        return;
    }

    int index = m_indicators.indexOf(ind);

    if (index == m_currentIndex) {
        return;
    }

    if (m_currentIndex != -1) {
        m_indicators.at(m_currentIndex)->select(false);
    }

    int flag = 0;

    if (m_currentIndex < index) {
        flag = 1;
    } else if (m_currentIndex > index) {
        flag = -1;
    }

    m_currentIndex = index;
    m_indicators.at(m_currentIndex)->select(true);
    play(m_currentIndex);
    m_view->startAnimation(flag);
}

void FancyBannerPrivate::switchPage(int flag)
{
    if (m_timer->isActive()) {
        m_timer->start(m_interval);
    }

    if (m_currentIndex == -1) {
        return;
    }

    if (flag == 0) {
        emit q->currentClicked(m_currentIndex);
        return;
    }

    int count = m_indicators.count();

    if (count < 2) {
        return;
    }

    m_indicators.at(m_currentIndex)->select(false);
    m_currentIndex += flag;

    if (m_currentIndex == -1) {
        m_currentIndex = count - 1;
    } else if (m_currentIndex == count) {
        m_currentIndex = 0;
    }

    m_indicators.at(m_currentIndex)->select(true);
    play(m_currentIndex);
    m_view->startAnimation(flag);
}

void FancyBannerPrivate::slotTimeOut()
{
    switchPage(1);
}

FancyBanner::FancyBanner(QWidget *parent)
    : QWidget(parent), d(new FancyBannerPrivate())
{
    d->q = this;
    d->init();
}

void FancyBanner::addPage(QPixmap pixmap)
{
    d->addPage(pixmap);
}

void FancyBanner::addPages(QList<QPixmap> pixmaps)
{
    d->addPages(pixmaps);
}

void FancyBanner::startAutoPlay()
{
    d->m_timer->start(d->m_interval);
}

void FancyBanner::stopAutoPlay()
{
    d->m_timer->stop();
}

void FancyBanner::setDelayTime(int msec, bool start)
{
    if (msec < 500 || msec > 10000) {
        return;
    }

    d->m_interval = msec;

    if (start) {
        startAutoPlay();
    }
}

void FancyBanner::setIndicatorFrontColor(QColor color)
{
    s_frontColor = color;
}

void FancyBanner::setIndicatorBackColor(QColor color)
{
    s_backColor = color;
}

void FancyBanner::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

#include "fancybanner.moc"

