/***************************************************************************
 **
 **  Copyright (C) 2019-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **  Reference: http://www.qtcn.org
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#ifndef QTNAVBUTTON_H
#define QTNAVBUTTON_H

#include <QPushButton>

class QtNavButton : public QPushButton
{
    Q_OBJECT
public:
    enum TextAlign {
        TextAlign_Left = 0x0001,
        TextAlign_Right = 0x0002,
        TextAlign_Top = 0x0020,
        TextAlign_Bottom = 0x0040,
        TextAlign_Center = 0x0004
    };

    enum TrianglePosition {
        TrianglePosition_Left = 0,
        TrianglePosition_Right = 1,
        TrianglePosition_Top = 2,
        TrianglePosition_Bottom = 3
    };

    enum IconPosition {
        IconPosition_Left = 0,
        IconPosition_Right = 1,
        IconPosition_Top = 2,
        IconPosition_Bottom = 3
    };

    enum LinePosition {
        LinePosition_Left = 0,
        LinePosition_Right = 1,
        LinePosition_Top = 2,
        LinePosition_Bottom = 3
    };

    explicit QtNavButton(QWidget *parent = nullptr);

    void setPaddingLeft(int padding);
    void setPaddingRight(int padding);
    void setPaddingTop(int padding);
    void setPaddingBottom(int padding);
    void setPadding(int padding);
    void setPadding(int left, int top, int right, int bottom);

    void setTextAlign(TextAlign align);

    void setShowTriangle(bool show);
    void setTriangleLen(int len);
    void setTrianglePosition(TrianglePosition position);
    void setTriangleColor(const QColor &color);

    void setShowIcon(bool show);
    void setIconSpace(int space);
    void setIconSize(const QSize &size);
    void setIconNormal(const QPixmap &pixmap);
    void setIconHover(const QPixmap &pixmap);
    void setIconChecked(const QPixmap &pixmap);

    void setShowLine(bool show);
    void setLineSpace(int space);
    void setLineWidth(int width);
    void setLinePosition(LinePosition position);
    void setLineColor(const QColor &color);

    void setNormalBackColor(const QColor &color);
    void setHoverBackColor(const QColor &color);
    void setCheckedBackColor(const QColor &color);

    void setNormalTextColor(const QColor &color);
    void setHoverTextColor(const QColor &color);
    void setCheckedTextColor(const QColor &color);

    void setNormalBackBrush(const QBrush &brush);
    void setHoverBackBrush(const QBrush &brush);
    void setCheckedBackBrush(const QBrush &brush);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);

private:
    void drawBackground(QPainter &p);
    void drawText(QPainter &p);
    void drawIcon(QPainter &p);
    void drawLine(QPainter &p);
    void drawTriangle(QPainter &p);

private:
    int m_paddingLeft;
    int m_paddingRight;
    int m_paddingTop;
    int m_paddingBottom;
    TextAlign m_textAlign;

    bool m_bShowTriangle;
    int m_triangleLen;
    QColor m_triangleColor;
    TrianglePosition m_trianglePosition;

    bool m_bShowIcon;
    int m_iconSpace;
    QSize m_iconSize;
    QPixmap m_iconNormal;
    QPixmap m_iconHover;
    QPixmap m_iconChecked;

    bool m_bShowLine;
    int m_lineSpace;
    int m_lineWidth;
    QColor m_lineColor;
    LinePosition m_linePosition;

    QColor m_normalBackColor;
    QColor m_hoverBackColor;
    QColor m_checkedBackColor;
    QColor m_normalTextColor;
    QColor m_hoverTextColor;
    QColor m_checkedTextColor;

    QBrush m_normalBackBrush;
    QBrush m_hoverBackBrush;
    QBrush m_checkedBackBrush;

    bool m_bMouseHover;
};

#endif // QTNAVBUTTON_H
