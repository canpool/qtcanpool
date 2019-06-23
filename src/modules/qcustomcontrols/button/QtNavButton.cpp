/***************************************************************************
 **
 **  Copyright (C) 2019 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **  Reference: http://www.qtcn.org
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
#include "QtNavButton.h"

#include <QPainter>

QtNavButton::QtNavButton(QWidget *parent)
    : QPushButton(parent),
      m_paddingLeft(20),
      m_paddingRight(5),
      m_paddingTop(5),
      m_paddingBottom(5),
      m_textAlign(TextAlign_Left),
      m_bShowTriangle(false),
      m_triangleLen(5),
      m_triangleColor(QColor(255, 255, 255)),
      m_trianglePosition(TrianglePosition_Right),
      m_bShowIcon(true),
      m_iconSpace(10),
      m_iconSize(QSize(16, 16)),
      m_iconNormal(QPixmap()),
      m_iconHover(QPixmap()),
      m_iconChecked(QPixmap()),
      m_bShowLine(true),
      m_lineSpace(0),
      m_lineWidth(5),
      m_lineColor(QColor(0, 187, 158)),
      m_normalBackColor(QColor(230, 230, 230)),
      m_hoverBackColor(QColor(130, 130, 130)),
      m_checkedBackColor(QColor(80, 80, 80)),
      m_normalTextColor(QColor(100, 100, 100)),
      m_hoverTextColor(QColor(255, 255, 255)),
      m_checkedTextColor(QColor(255, 255, 255)),
      m_normalBackBrush(Qt::NoBrush),
      m_hoverBackBrush(Qt::NoBrush),
      m_checkedBackBrush(Qt::NoBrush),
      m_bMouseHover(false)
{
    setCheckable(true);
}

void QtNavButton::setPaddingLeft(int padding)
{
    if (padding != m_paddingLeft) {
        m_paddingLeft = padding;
        update();
    }
}

void QtNavButton::setPaddingRight(int padding)
{
    if (padding != m_paddingRight) {
        m_paddingRight = padding;
        update();
    }
}

void QtNavButton::setPaddingTop(int padding)
{
    if (padding != m_paddingTop) {
        m_paddingTop = padding;
        update();
    }
}

void QtNavButton::setPaddingBottom(int padding)
{
    if (padding != m_paddingBottom) {
        m_paddingBottom = padding;
        update();
    }
}

void QtNavButton::setPadding(int padding)
{
    setPadding(padding, padding, padding, padding);
}

void QtNavButton::setPadding(int left, int top, int right, int bottom)
{
    m_paddingLeft = left;
    m_paddingTop = top;
    m_paddingRight = right;
    m_paddingBottom = bottom;
    update();
}

void QtNavButton::setTextAlign(QtNavButton::TextAlign align)
{
    if (align != m_textAlign) {
        m_textAlign = align;
        update();
    }
}

void QtNavButton::setShowTriangle(bool show)
{
    if (show != m_bShowTriangle) {
        m_bShowTriangle = show;
        update();
    }
}

void QtNavButton::setTriangleLen(int len)
{
    if (len >= 0 && len != m_triangleLen) {
        m_triangleLen = len;
        update();
    }
}

void QtNavButton::setTrianglePosition(QtNavButton::TrianglePosition position)
{
    if (position != m_trianglePosition) {
        m_trianglePosition = position;
        update();
    }
}

void QtNavButton::setTriangleColor(const QColor &color)
{
    if (color != m_triangleColor) {
        m_triangleColor = color;
        update();
    }
}

void QtNavButton::setShowIcon(bool show)
{
    if (show != m_bShowIcon) {
        m_bShowIcon = show;
        update();
    }
}

void QtNavButton::setIconSpace(int space)
{
    if (space >= 0 && space != m_iconSpace) {
        m_iconSpace = space;
        update();
    }
}

void QtNavButton::setIconSize(const QSize &size)
{
    if (size != m_iconSize) {
        m_iconSize = size;
        update();
    }
}

void QtNavButton::setIconNormal(const QPixmap &pixmap)
{
    m_iconNormal = pixmap;
    update();
}

void QtNavButton::setIconHover(const QPixmap &pixmap)
{
    m_iconHover = pixmap;
    update();
}

void QtNavButton::setIconChecked(const QPixmap &pixmap)
{
    m_iconChecked = pixmap;
    update();
}

void QtNavButton::setShowLine(bool show)
{
    if (show != m_bShowLine) {
        m_bShowLine = show;
        update();
    }
}

void QtNavButton::setLineSpace(int space)
{
    if (space >= 0 && space != m_lineSpace) {
        m_lineSpace = space;
        update();
    }
}

void QtNavButton::setLineWidth(int width)
{
    if (width >= 0 && width != m_lineWidth) {
        m_lineWidth = width;
        update();
    }
}

void QtNavButton::setLinePosition(QtNavButton::LinePosition position)
{
    if (position != m_linePosition) {
        m_linePosition = position;
        update();
    }
}

void QtNavButton::setLineColor(const QColor &color)
{
    if (color != m_lineColor) {
        m_lineColor = color;
        update();
    }
}

void QtNavButton::setNormalBackColor(const QColor &color)
{
    if (color != m_normalBackColor) {
        m_normalBackColor = color;
        update();
    }
}

void QtNavButton::setHoverBackColor(const QColor &color)
{
    if (color != m_hoverBackColor) {
        m_hoverBackColor = color;
        update();
    }
}

void QtNavButton::setCheckedBackColor(const QColor &color)
{
    if (color != m_checkedBackColor) {
        m_checkedBackColor = color;
        update();
    }
}

void QtNavButton::setNormalTextColor(const QColor &color)
{
    if (color != m_normalTextColor) {
        m_normalTextColor = color;
        update();
    }
}

void QtNavButton::setHoverTextColor(const QColor &color)
{
    if (color != m_hoverTextColor) {
        m_hoverTextColor = color;
        update();
    }
}

void QtNavButton::setCheckedTextColor(const QColor &color)
{
    if (color != m_checkedTextColor) {
        m_checkedTextColor = color;
        update();
    }
}

void QtNavButton::setNormalBackBrush(const QBrush &brush)
{
    if (brush != m_normalBackBrush) {
        m_normalBackBrush = brush;
        update();
    }
}

void QtNavButton::setHoverBackBrush(const QBrush &brush)
{
    if (brush != m_hoverBackBrush) {
        m_hoverBackBrush = brush;
        update();
    }
}

void QtNavButton::setCheckedBackBrush(const QBrush &brush)
{
    if (brush != m_checkedBackBrush) {
        m_checkedBackBrush = brush;
        update();
    }
}

void QtNavButton::enterEvent(QEvent *)
{
    m_bMouseHover = true;
    update();
}

void QtNavButton::leaveEvent(QEvent *)
{
    m_bMouseHover = false;
    update();
}

void QtNavButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    drawBackground(p);
    drawText(p);

    if (m_bShowIcon) {
        drawIcon(p);
    }
    if (m_bShowLine) {
        drawLine(p);
    }
    if (m_bShowTriangle) {
        drawTriangle(p);
    }
}

void QtNavButton::drawBackground(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);

    int w = this->width();
    int h = this->height();

    QRect bgRect;
    if (m_linePosition == LinePosition_Left) {
        bgRect = QRect(m_lineSpace, 0, w - m_lineSpace, h);
    } else if (m_linePosition == LinePosition_Right) {
        bgRect = QRect(0, 0, w - m_lineSpace, h);
    } else if (m_linePosition == LinePosition_Top) {
        bgRect = QRect(0, m_lineSpace, w, h - m_lineSpace);
    } else if (m_linePosition == LinePosition_Bottom) {
        bgRect = QRect(0, 0, w, h - m_lineSpace);
    }

    QBrush bgBrush;
    if (isChecked()) {
        bgBrush = m_checkedBackBrush;
    } else if (m_bMouseHover) {
        bgBrush = m_hoverBackBrush;
    } else {
        bgBrush = m_normalBackBrush;
    }

    if (bgBrush != Qt::NoBrush) {
        p.setBrush(bgBrush);
    } else {
        QColor bgColor;
        if (isChecked()) {
            bgColor = m_checkedBackColor;
        } else if (m_bMouseHover) {
            bgColor = m_hoverBackColor;
        } else {
            bgColor = m_normalBackColor;
        }

        p.setBrush(bgColor);
    }

    p.drawRect(bgRect);

    p.restore();
}

void QtNavButton::drawText(QPainter &p)
{
    p.save();
    p.setBrush(Qt::NoBrush);

    QColor textColor;
    if (isChecked()) {
        textColor = m_checkedTextColor;
    } else if (m_bMouseHover) {
        textColor = m_hoverTextColor;
    } else {
        textColor = m_normalTextColor;
    }

    QRect textRect = QRect(m_paddingLeft, m_paddingTop, width() - m_paddingLeft - m_paddingRight,
                           height() - m_paddingTop - m_paddingBottom);
    p.setPen(textColor);
    p.drawText(textRect, m_textAlign | Qt::AlignVCenter, text());

    p.restore();
}

void QtNavButton::drawIcon(QPainter &p)
{
    p.save();

    QPixmap pix;
    if (isChecked()) {
        pix = m_iconChecked;
    } else if (m_bMouseHover) {
        pix = m_iconHover;
    } else {
        pix = m_iconNormal;
    }

    if (!pix.isNull()) {
        pix = pix.scaled(m_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        p.drawPixmap(m_iconSpace, (height() - m_iconSize.height()) / 2, pix);
    }

    p.restore();
}

void QtNavButton::drawLine(QPainter &p)
{
    p.save();

    QPen pen;
    pen.setWidth(m_lineWidth);
    pen.setColor(m_lineColor);
    p.setPen(pen);

    QPoint pointStart, pointEnd;
    if (m_linePosition == LinePosition_Left) {
        pointStart = QPoint(0, 0);
        pointEnd = QPoint(0, height());
    } else if (m_linePosition == LinePosition_Right) {
        pointStart = QPoint(width(), 0);
        pointEnd = QPoint(width(), height());
    } else if (m_linePosition == LinePosition_Top) {
        pointStart = QPoint(0, 0);
        pointEnd = QPoint(width(), 0);
    } else if (m_linePosition == LinePosition_Bottom) {
        pointStart = QPoint(0, height());
        pointEnd = QPoint(width(), height());
    }

    p.drawLine(pointStart, pointEnd);

    p.restore();
}

void QtNavButton::drawTriangle(QPainter &p)
{
    if (!m_bMouseHover && !isChecked()) {
        return;
    }

    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(m_triangleColor);

    int width = this->width();
    int height = this->height();
    int midWidth = width / 2;
    int midHeight = height / 2;

    QPolygon pts;
    if (m_trianglePosition == TrianglePosition_Left) {
        pts.setPoints(3, m_triangleLen, midHeight, 0, midHeight - m_triangleLen,
                      0, midHeight + m_triangleLen);
    } else if (m_trianglePosition == TrianglePosition_Right) {
        pts.setPoints(3, width - m_triangleLen, midHeight, width, midHeight - m_triangleLen,
                      width, midHeight + m_triangleLen);
    } else if (m_trianglePosition == TrianglePosition_Top) {
        pts.setPoints(3, midWidth, m_triangleLen, midWidth - m_triangleLen,
                      0, midWidth + m_triangleLen, 0);
    } else if (m_trianglePosition == TrianglePosition_Bottom) {
        pts.setPoints(3, midWidth, height - m_triangleLen, midWidth - m_triangleLen, height,
                      midWidth + m_triangleLen, height);
    }

    p.drawPolygon(pts);

    p.restore();
}
