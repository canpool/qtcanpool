/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * Copyright (C) 2018, Juergen Skrotzky (https://github.com/Jorgen-VikingGod, JorgenVikingGod@gmail.com)
 * SPDX-License-Identifier: MIT
 **/

#include "qgoodstyle.h"
#include "qgoodstyleHelper.h"
#include <QApplication>
#include <QFile>
#include <QFont>
#include <QStyleFactory>

QGoodStyle::QGoodStyle(StyleType type) : QGoodStyle(styleBase(), type)
{
#ifdef Q_OS_WIN
    m_hashPixmapCache[SP_MessageBoxInformation] = QGoodStyleHelper::winStandardPixmap(SP_MessageBoxInformation);
    m_hashPixmapCache[SP_MessageBoxWarning] = QGoodStyleHelper::winStandardPixmap(SP_MessageBoxWarning);
    m_hashPixmapCache[SP_MessageBoxCritical] = QGoodStyleHelper::winStandardPixmap(SP_MessageBoxCritical);
    m_hashPixmapCache[SP_MessageBoxQuestion] = QGoodStyleHelper::winStandardPixmap(SP_MessageBoxQuestion);
#endif
}

QGoodStyle::QGoodStyle(QStyle *style, StyleType type) : QProxyStyle(style)
{
    m_type = type;
}

QGoodStyle::~QGoodStyle()
{
}

QStyle *QGoodStyle::styleBase() const
{
    QStyle *base = QStyleFactory::create(QStringLiteral("Fusion"));
    return base;
}

QIcon QGoodStyle::standardIcon(StandardPixmap standardPixmap, const QStyleOption *option, const QWidget *widget) const
{
#ifdef Q_OS_WIN
    switch (standardPixmap) {
    case SP_MessageBoxInformation:
    case SP_MessageBoxWarning:
    case SP_MessageBoxCritical:
    case SP_MessageBoxQuestion: {
        QPixmap pixmap = m_hashPixmapCache.value(standardPixmap, QPixmap());

        if (!pixmap.isNull())
            return QIcon(pixmap);

        break;
    }
    default:
        break;
    }
#endif

    return QProxyStyle::standardIcon(standardPixmap, option, widget);
}

void QGoodStyle::polish(QPalette &palette)
{
    if (m_type == LightStyle) {
        palette.setColor(QPalette::Window, QColor(240, 240, 240));
        palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(120, 120, 120));
        palette.setColor(QPalette::Base, QColor(255, 255, 255));
        palette.setColor(QPalette::AlternateBase, QColor(233, 231, 227));
        palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
        palette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
        palette.setColor(QPalette::Text, QColor(0, 0, 0));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(120, 120, 120));
        palette.setColor(QPalette::Dark, QColor(160, 160, 160));
        palette.setColor(QPalette::Shadow, QColor(105, 105, 105));
        palette.setColor(QPalette::Button, QColor(240, 240, 240));
        palette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(120, 120, 120));
        palette.setColor(QPalette::BrightText, QColor(0, 0, 255));
        palette.setColor(QPalette::Link, QColor(51, 153, 255));
        palette.setColor(QPalette::Highlight, QColor(0, 0, 255));
        palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(51, 153, 255));
        palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(255, 255, 255));
    } else if (m_type == DarkStyle) {
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, QColor(255, 255, 255));
        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
        palette.setColor(QPalette::Base, QColor(42, 42, 42));
        palette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
        palette.setColor(QPalette::ToolTipText, QColor(255, 255, 255));
        palette.setColor(QPalette::Text, QColor(255, 255, 255));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        palette.setColor(QPalette::Dark, QColor(35, 35, 35));
        palette.setColor(QPalette::Shadow, QColor(20, 20, 20));
        palette.setColor(QPalette::Button, QColor(53, 53, 53));
        palette.setColor(QPalette::ButtonText, QColor(255, 255, 255));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        palette.setColor(QPalette::BrightText, QColor(255, 0, 0));
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
        palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
    }
}

void QGoodStyle::polish(QApplication *app)
{
    if (!app)
        return;

    QFont defaultFont = app->font();
    defaultFont.setPointSize(defaultFont.pointSize() + 2);
    app->setFont(defaultFont);

    QFile file(QStringLiteral(":/res/lightstyle.qss"));
    if (m_type == DarkStyle) {
        file.setFileName(QStringLiteral(":/res/darkstyle.qss"));
    }
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString style_sheet = QString::fromLatin1(file.readAll());
        app->setStyleSheet(style_sheet);
        file.close();
    }
}

void QGoodStyle::unpolish(QApplication *app)
{
    if (!app)
        return;

    QFont defaultFont = app->font();
    defaultFont.setPointSize(defaultFont.pointSize() - 2);
    app->setFont(defaultFont);
}

void QGoodStyle::setStyleType(StyleType type)
{
    m_type = type;
}

QGoodStyle::StyleType QGoodStyle::styleType() const
{
    return m_type;
}

QStyle *QGoodStyle::baseStyle() const
{
    return styleBase();
}
