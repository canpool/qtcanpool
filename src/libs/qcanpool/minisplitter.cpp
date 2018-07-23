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
#include "minisplitter.h"
#include <QPaintEvent>
#include <QPainter>
#include <QSplitterHandle>

class MiniSplitterHandle : public QSplitterHandle
{
public:
    MiniSplitterHandle(Qt::Orientation orientation, QSplitter *parent, bool lightColored = false)
            : QSplitterHandle(orientation, parent),
              m_lightColored(lightColored)
    {
        setMask(QRegion(contentsRect()));
        setAttribute(Qt::WA_MouseNoMask, true);
    }
protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool m_lightColored;
};

void MiniSplitterHandle::resizeEvent(QResizeEvent *event)
{
    if (orientation() == Qt::Horizontal)
        setContentsMargins(2, 0, 2, 0);
    else
        setContentsMargins(0, 2, 0, 2);
    setMask(QRegion(contentsRect()));
    QSplitterHandle::resizeEvent(event);
}

void MiniSplitterHandle::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
//    const QColor color = Utils::creatorTheme()->color(
//                m_lightColored ? Utils::Theme::FancyToolBarSeparatorColor
//                               : Utils::Theme::SplitterColor);
    bool ok = true;
    QString scolor = m_lightColored ? QString("60ffffff") : QString("ff151515");
    const QRgb rgba = scolor.toLongLong(&ok, 16);
    const QColor color = QColor::fromRgba(rgba);
    painter.fillRect(event->rect(), color);
}

QSplitterHandle *MiniSplitter::createHandle()
{
    return new MiniSplitterHandle(orientation(), this, m_style == Light);
}

MiniSplitter::MiniSplitter(QWidget *parent, SplitterStyle style)
    : QSplitter(parent),
      m_style(style)
{
    setHandleWidth(1);
    setChildrenCollapsible(false);
    setProperty("minisplitter", true);
}

MiniSplitter::MiniSplitter(Qt::Orientation orientation, SplitterStyle style)
    : QSplitter(orientation),
      m_style(style)
{
    setHandleWidth(1);
    setChildrenCollapsible(false);
    setProperty("minisplitter", true);
}
