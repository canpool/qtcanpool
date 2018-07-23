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
#include "tinytabwidget.h"

#include <QStackedWidget>
#include <QToolButton>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>

////////////////////////////////////////////////////////////////////////
/// \brief The TinyTab class
///
class TinyTab : public QToolButton
{
    Q_OBJECT
public:
    enum SlidePosition { Left, Bottom, Right };
    explicit TinyTab(QWidget *parent = 0);

    void select(bool selected);

    void setHoverColor(const QColor &color) { m_hoverColor = color;}
    QColor hoverColor() const { return m_hoverColor;}

    void setPressColor(const QColor &color) { m_pressColor = color;}
    QColor pressColor() const { return m_pressColor;}

    void setTextColor(const QColor &color);
    void setSelectedTextColor(const QColor &color);

    void setColor(const QColor &color);

    void setSlideColor(const QColor &color);
    void setNormalColor(const QColor &color);

    void setSlidePosition(SlidePosition pos);

private:
    void painterInfo(QColor &color);
    void painterSlide(QColor &color);

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

private:
    bool m_bMouseHover;
    bool m_bMousePress;

    QColor m_hoverColor;
    QColor m_pressColor;
    QColor m_textColor;
    QColor m_selectedTextColor;
    QColor m_normalColor;
    QColor m_slideColor;

    SlidePosition m_slidePos;
};

TinyTab::TinyTab(QWidget *parent)
    : QToolButton(parent)
{
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    setIconSize(QSize(22,22));

    setAutoRaise(true);

    m_bMouseHover = false;
    m_bMousePress = false;

    m_slidePos = Bottom;

    m_hoverColor = QColor(255, 255, 255, 50);
    m_pressColor = QColor(0, 0, 0, 100);
    m_textColor = QColor(255,255,255);
    m_selectedTextColor = QColor(255,255,255);
    m_normalColor = QColor(0, 0, 0, 20);
    m_slideColor = QColor(250, 0, 0);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    setStyleSheet("QToolButton{background-color: transparent;color: white;}");

    update();
}

void TinyTab::select(bool selected)
{
    m_bMousePress = selected;
    update();
}

void TinyTab::setTextColor(const QColor &color)
{
    m_textColor = color;
}

void TinyTab::setSelectedTextColor(const QColor &color)
{
    m_selectedTextColor = color;
}

void TinyTab::setColor(const QColor &color)
{
    setStyleSheet(QString("QToolButton{background-color: transparent;color: rgba(%1,%2,%3,%4);}")
                  .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha())
                  );
}

void TinyTab::setSlideColor(const QColor &color)
{
    m_slideColor = color;
}

void TinyTab::setNormalColor(const QColor &color)
{
    m_normalColor = color;
}

void TinyTab::setSlidePosition(TinyTab::SlidePosition pos)
{
    m_slidePos = pos;
}

void TinyTab::painterInfo(QColor &color)
{
    QPainter painter(this);
    QPen pen(Qt::NoBrush, 1);
    painter.setPen(pen);

    painter.setBrush(color);
    painter.drawRect(rect());
}

void TinyTab::painterSlide(QColor &color)
{
    QPainter painter(this);
    QPen pen;
    pen.setColor(color);
    pen.setWidth(2);
    painter.setPen(pen);

    QPointF p1;
    QPointF p2;
    switch (m_slidePos) {
    case Left:
        p1 = this->rect().topLeft();
        p2 = this->rect().bottomLeft();
        break;
    case Bottom:
        p1 = this->rect().bottomLeft();
        p2 = this->rect().bottomRight();
        break;
    case Right:
        p1 = this->rect().topRight();
        p2 = this->rect().bottomRight();
        break;
    default:
        break;
    }
    painter.drawLine(p1, p2);
}

void TinyTab::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(!isEnabled())
        return;
    m_bMouseHover = true;
}

void TinyTab::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bMouseHover = false;
}

void TinyTab::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(isEnabled()){
            emit clicked();
        }
    }
}

void TinyTab::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
    }
}

void TinyTab::paintEvent(QPaintEvent *event)
{
    if(m_bMousePress){
        painterInfo(m_pressColor);
        setColor(m_selectedTextColor);
        painterSlide(m_slideColor);
    }else if(m_bMouseHover){
        painterInfo(m_hoverColor);
        setColor(m_selectedTextColor);
        painterSlide(m_slideColor);
    }else{
        painterInfo(m_normalColor);
        setColor(m_textColor);
        painterSlide(m_normalColor);
    }

    QToolButton::paintEvent(event);
}

///////////////////////////////////////////////////////////////////////
/// \brief The TinyTabBar class
///
class TinyTabBar : public QWidget
{
    Q_OBJECT
public:
    enum Direction {Horizontal, Vertical};
    explicit TinyTabBar(QWidget *parent = 0);

    void setDirection(Direction direction);
    void setSlidePosition(TinyTab::SlidePosition pos);

    void addTab(const QString &label);
    void addTab(const QIcon &icon, const QString &label);

    void setHoverColor(const QColor &color);
    QColor hoverColor() const;

    void setPressColor(const QColor &color);
    QColor pressColor() const;

    void setTextColor(const QColor &color);
    void setSelectedTextColor(const QColor &color);

    void setSlideColor(const QColor &color);
    void setNormalColor(const QColor &color);

    void setTabSpace(int space);

private:
    void addTab(TinyTab *tab);

signals:
    void currentChanged(int index);

public slots:
    void setCurrentIndex(int index);

private slots:
    void switchTab();

private:
    Direction m_direction;
    int m_currentIndex;

    QList<TinyTab*> m_tabs;
    QBoxLayout *m_layout;
};

TinyTabBar::TinyTabBar(QWidget *parent)
    : QWidget(parent)
{
    m_direction = Horizontal;
    m_currentIndex = -1;
    m_tabs.clear();
    m_layout = 0;

    m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_layout->setMargin(0);
    m_layout->setSpacing(1);
    m_layout->addStretch();
    setLayout(m_layout);
    if(m_direction == Horizontal){
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }else{
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
}

void TinyTabBar::setDirection(TinyTabBar::Direction direction)
{
    if(m_direction == direction)
        return;
    m_direction = direction;

    QBoxLayout::Direction layDirection;
    switch (m_direction) {
    case Horizontal:
        layDirection = QBoxLayout::LeftToRight;
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        break;
    case Vertical:
    default:
        layDirection = QBoxLayout::TopToBottom;
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        break;
    }
    m_layout->setDirection(layDirection);

    if(m_direction == Vertical){
        foreach (TinyTab *tab, m_tabs) {
            tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        }
    }else if(m_direction == Horizontal){
        foreach (TinyTab *tab, m_tabs) {
            tab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        }
    }
}

void TinyTabBar::setSlidePosition(TinyTab::SlidePosition pos)
{
    foreach (TinyTab *tab, m_tabs) {
        tab->setSlidePosition(pos);
    }
}

void TinyTabBar::addTab(const QString &label)
{
    TinyTab *tab = new TinyTab(this);
    tab->setText(label);
    addTab(tab);
}

void TinyTabBar::addTab(const QIcon &icon, const QString &label)
{
    TinyTab *tab = new TinyTab(this);
    tab->setText(label);
    tab->setIcon(icon);
    addTab(tab);
}

void TinyTabBar::setHoverColor(const QColor &color)
{
    foreach (TinyTab *tab, m_tabs) {
        tab->setHoverColor(color);
    }
}

QColor TinyTabBar::hoverColor() const
{
    if(m_tabs.count() > 0){
        return m_tabs.at(0)->hoverColor();
    }
    return QColor();
}

void TinyTabBar::setPressColor(const QColor &color)
{
    foreach (TinyTab *tab, m_tabs) {
        tab->setPressColor(color);
    }
}

QColor TinyTabBar::pressColor() const
{
    if(m_tabs.count() > 0){
        return m_tabs.at(0)->pressColor();
    }
    return QColor();
}

void TinyTabBar::setTextColor(const QColor &color)
{
    foreach (TinyTab *tab, m_tabs) {
        tab->setTextColor(color);
    }
}

void TinyTabBar::setSelectedTextColor(const QColor &color)
{
    foreach (TinyTab *tab, m_tabs) {
        tab->setSelectedTextColor(color);
    }
}

void TinyTabBar::setSlideColor(const QColor &color)
{
    foreach (TinyTab *tab, m_tabs) {
        tab->setSlideColor(color);
    }
}

void TinyTabBar::setNormalColor(const QColor &color)
{
    foreach (TinyTab *tab, m_tabs) {
        tab->setNormalColor(color);
    }
}

void TinyTabBar::setTabSpace(int space)
{
    m_layout->setSpacing(space);
}

void TinyTabBar::addTab(TinyTab *tab)
{
    if(m_direction == Vertical){
        tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }else if(m_direction == Horizontal){
        tab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
    m_tabs.append(tab);
    connect(tab, SIGNAL(clicked(bool)), this, SLOT(switchTab()));
    m_layout->insertWidget(m_tabs.count()-1, tab);
//    m_layout->addWidget(tab);
    m_tabs.at(0)->select(true);
    m_currentIndex = 0;
}

void TinyTabBar::setCurrentIndex(int index)
{
    if(index == m_currentIndex) return;
    if(m_currentIndex != -1){
        m_tabs.at(m_currentIndex)->select(false);
    }
    m_currentIndex = index;
    m_tabs.at(m_currentIndex)->select(true);
}

void TinyTabBar::switchTab()
{
    TinyTab *tab = qobject_cast<TinyTab*>(sender());
    if(tab == 0) return;
    int index = m_tabs.indexOf(tab);
    if(index == m_currentIndex) return;
    if(m_currentIndex != -1){
        m_tabs.at(m_currentIndex)->select(false);
    }
    m_currentIndex = index;
    m_tabs.at(m_currentIndex)->select(true);
    emit currentChanged(m_currentIndex);
}

///////////////////////////////////////////////////////////////////////////
/// \brief The TinyTabWidgetPrivate class
///
class TinyTabWidgetPrivate : public QObject
{
    Q_OBJECT
public:
    explicit TinyTabWidgetPrivate(QObject *parent = 0);
    void updateTabBarPosition();
    void init();

    TinyTabBar *m_tabBar;
    QStackedWidget *m_stack;

    TinyTabWidget *q;
    QBoxLayout *m_layout;
    TinyTabWidget::TabPosition m_pos;

};

TinyTabWidgetPrivate::TinyTabWidgetPrivate(QObject *parent)
    : QObject(parent), m_tabBar(0), m_stack(0), q(0),
      m_layout(0), m_pos(TinyTabWidget::North)
{

}

void TinyTabWidgetPrivate::updateTabBarPosition()
{
    switch (m_pos) {
    case TinyTabWidget::North:
        m_tabBar->setDirection(TinyTabBar::Horizontal);
        m_tabBar->setSlidePosition(TinyTab::Bottom);
        m_layout->setDirection(QBoxLayout::TopToBottom);
        break;
    case TinyTabWidget::South:
        m_tabBar->setDirection(TinyTabBar::Horizontal);
        m_tabBar->setSlidePosition(TinyTab::Bottom);
        m_layout->setDirection(QBoxLayout::BottomToTop);
        break;
    case TinyTabWidget::West:
        m_tabBar->setDirection(TinyTabBar::Vertical);
        m_tabBar->setSlidePosition(TinyTab::Left);
        m_layout->setDirection(QBoxLayout::LeftToRight);
        break;
    case TinyTabWidget::East:
        m_tabBar->setDirection(TinyTabBar::Vertical);
        m_tabBar->setSlidePosition(TinyTab::Right);
        m_layout->setDirection(QBoxLayout::RightToLeft);
        break;
    default:
        m_tabBar->setDirection(TinyTabBar::Horizontal);
        m_tabBar->setSlidePosition(TinyTab::Bottom);
        m_layout->setDirection(QBoxLayout::TopToBottom);
        break;
    }
}

void TinyTabWidgetPrivate::init()
{
    m_tabBar = new TinyTabBar(q);
    m_stack = new QStackedWidget(q);

    connect(m_tabBar, SIGNAL(currentChanged(int)), m_stack, SLOT(setCurrentIndex(int)));
    connect(m_stack, SIGNAL(currentChanged(int)), q, SIGNAL(currentChanged(int)));
    connect(m_stack, SIGNAL(currentChanged(int)), m_tabBar, SLOT(setCurrentIndex(int)));

    m_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_tabBar);
    m_layout->addWidget(m_stack);
    q->setLayout(m_layout);
}

//////////////////////////////////////////////////////////////////////////////////////////
/// \brief TinyTabWidget::TinyTabWidget
/// \param parent
///
TinyTabWidget::TinyTabWidget(QWidget *parent)
    : QWidget(parent), d(new TinyTabWidgetPrivate(this))
{
    d->q = this;
    d->init();
}

TinyTabWidget::~TinyTabWidget()
{

}

int TinyTabWidget::addTab(QWidget *page, const QString &label)
{
    d->m_tabBar->addTab(label);
    d->m_stack->addWidget(page);
    return (d->m_stack->count()-1);
}

int TinyTabWidget::addTab(QWidget *page, const QIcon &icon, const QString &label)
{
    d->m_tabBar->addTab(icon, label);
    d->m_stack->addWidget(page);
    return (d->m_stack->count()-1);
}

TinyTabWidget::TabPosition TinyTabWidget::tabPosition() const
{
    return d->m_pos;
}

void TinyTabWidget::setTabPosition(TinyTabWidget::TabPosition pos)
{
    if(d->m_pos == pos)
        return;
    d->m_pos = pos;
    d->updateTabBarPosition();
}

void TinyTabWidget::setHoverColor(const QColor &color)
{
    d->m_tabBar->setHoverColor(color);
}

QColor TinyTabWidget::hoverColor() const
{
    return d->m_tabBar->hoverColor();
}

void TinyTabWidget::setPressColor(const QColor &color)
{
    d->m_tabBar->setPressColor(color);
}

QColor TinyTabWidget::pressColor() const
{
    return d->m_tabBar->pressColor();
}

void TinyTabWidget::setTextColor(const QColor &color)
{
    d->m_tabBar->setTextColor(color);
}

void TinyTabWidget::setSelectedTextColor(const QColor &color)
{
    d->m_tabBar->setSelectedTextColor(color);
}

void TinyTabWidget::setBackgroundColor(const QColor &color)
{
    QPalette palette;
    palette.setColor(QPalette::Background, color);
    d->m_tabBar->setPalette(palette);
    d->m_tabBar->setAutoFillBackground(true);
}

void TinyTabWidget::setNormalColor(const QColor &color)
{
    d->m_tabBar->setNormalColor(color);
}

void TinyTabWidget::setSlideColor(const QColor &color)
{
    d->m_tabBar->setSlideColor(color);
}

void TinyTabWidget::setTabSpace(int space)
{
    d->m_tabBar->setTabSpace(space);
}

int TinyTabWidget::currentIndex() const
{
    return d->m_stack->currentIndex();
}

QWidget *TinyTabWidget::currentWidget() const
{
    return d->m_stack->currentWidget();
}

QWidget *TinyTabWidget::widget(int index)
{
    return d->m_stack->widget(index);
}

void TinyTabWidget::setCurrentIndex(int index)
{
    d->m_stack->setCurrentIndex(index);
}

void TinyTabWidget::setCurrentWidget(QWidget *widget)
{
    d->m_stack->setCurrentWidget(widget);
}

#include "tinytabwidget.moc"
