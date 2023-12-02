/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef QGOODTITLEBAR_H
#define QGOODTITLEBAR_H

#include "qgoodcaptionbutton.h"
#include "qgoodiconwidget.h"
#include "qgoodstateholder.h"
#include "qgoodtitlewidget.h"
#include "qgood/qgoodwindow.h"

#include <QFrame>

class QSpacerItem;

class QGoodTitleBar : public QFrame
{
    Q_OBJECT
public:
    explicit QGoodTitleBar(QGoodWindow *gw, QWidget *parent = nullptr);

Q_SIGNALS:
    void showMinimized();
    void showNormal();
    void showMaximized();
    void closeWindow();

public Q_SLOTS:
    void setTitle(const QString &title);
    void setIcon(const QPixmap &icon);
    void setActive(bool active);
    void setTitleAlignment(const Qt::Alignment &alignment);
    void setMaximized(bool maximized);
    void setTheme();
    void setLeftTitleBarWidget(QWidget *widget);
    void setRightTitleBarWidget(QWidget *widget);
    void setCenterTitleBarWidget(QWidget *widget);
    void setCaptionButtonWidth(int width);
    Qt::Alignment titleAlignment();
    int captionButtonsWidth();
    int leftWidth();
    int rightWidth();
    int layoutSpacing();
    bool isMinimizedButtonEnabled();
    bool isMaximizeButtonEnabled();
    bool isCloseButtonEnabled();
    QRect minimizeButtonRect();
    QRect maximizeButtonRect();
    QRect closeButtonRect();
    void updateWindow();
    void captionButtonStateChanged(const QGoodWindow::CaptionButtonState &state);
private:
    QPointer<QGoodWindow> m_gw;
    QGoodIconWidget *m_iconWidget;
    QGoodTitleWidget *m_titleWidget;
    QWidget *m_captionButtons;
    QGoodCaptionButton *m_minBtn;
    QGoodCaptionButton *m_restoreBtn;
    QGoodCaptionButton *m_maxBtn;
    QGoodCaptionButton *m_clsBtn;
    QPointer<QWidget> m_leftWidget;
    QPointer<QWidget> m_rightWidget;
    QPointer<QWidget> m_centerWidget;
    QPointer<QWidget> m_leftWidgetPlaceHolder;
    QPointer<QWidget> m_rightWidgetPlaceHolder;
    QPointer<QWidget> m_centerWidgetPlaceHolder;
    QSpacerItem *m_centerSpacerItemLeft;
    QSpacerItem *m_centerSpacerItemRight;
    int m_layoutSpacing;
    QString style;
    bool m_active;
    bool m_isMaximized;
    QColor m_titleBarColor;

    friend class QGoodCentralWidget;
    friend class QGoodTitleWidget;
};

#endif   // QGOODTITLEBAR_H
