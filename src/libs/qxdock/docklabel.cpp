/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docklabel.h"
#include "dockutils.h"

QX_DOCK_BEGIN_NAMESPACE

class DockLabelPrivate
{
public:
    QX_DECLARE_PUBLIC(DockLabel)
public:
    DockLabelPrivate();

    void elideText(int width);
    bool isModeElideNone() const;
public:
    QString m_text;
    Qt::TextElideMode m_elideMode = Qt::ElideNone;
    bool m_isElided = false;
};

DockLabelPrivate::DockLabelPrivate()
{
}

void DockLabelPrivate::elideText(int width)
{
    if (isModeElideNone()) {
        return;
    }
    Q_Q(DockLabel);
    QFontMetrics fm = q->fontMetrics();
    QString str = fm.elidedText(m_text, m_elideMode, width - q->margin() * 2 - q->indent());
    if (str == u'\x2026') {
        str = m_text.at(0);
    }
    bool wasElided = m_isElided;
    m_isElided = str != m_text;
    if (m_isElided != wasElided) {
        Q_EMIT q->elidedChanged(m_isElided);
    }
    q->QLabel::setText(str);
}

bool DockLabelPrivate::isModeElideNone() const
{
    return Qt::ElideNone == m_elideMode;
}

QxDock::DockLabel::DockLabel(QWidget *parent, Qt::WindowFlags f)
    : DockLabel("", parent, f)
{
}

QxDock::DockLabel::DockLabel(const QString &text, QWidget *parent, Qt::WindowFlags f)
    : QLabel(text, parent, f)
{
    QX_INIT_PRIVATE(DockLabel);

    Q_D(DockLabel);
    if (!text.isEmpty()) {
        d->m_text = text;
        internal::setToolTip(this, text);
    }
}

DockLabel::~DockLabel()
{
    QX_FINI_PRIVATE();
}

Qt::TextElideMode DockLabel::elideMode() const
{
    Q_D(const DockLabel);
    return d->m_elideMode;
}

void DockLabel::setElideMode(Qt::TextElideMode mode)
{
    Q_D(DockLabel);
    d->m_elideMode = mode;
    d->elideText(size().width());
}

bool DockLabel::isElided() const
{
    Q_D(const DockLabel);
    return d->m_isElided;
}

QSize DockLabel::minimumSizeHint() const
{
    Q_D(const DockLabel);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    bool hasPixmap = !pixmap(Qt::ReturnByValue).isNull();
#else
    bool hasPixmap = (pixmap() != nullptr);
#endif
    if (hasPixmap || d->isModeElideNone()) {
        return QLabel::minimumSizeHint();
    }
    const QFontMetrics &fm = fontMetrics();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    QSize size(fm.horizontalAdvance(d->m_text.left(2) + u'\x2026'), fm.height());
#else
    QSize size(fm.width(d->m_text.left(2) + u'\x2026'), fm.height());
#endif
    return size;
}

QSize DockLabel::sizeHint() const
{
    Q_D(const DockLabel);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    bool hasPixmap = !pixmap(Qt::ReturnByValue).isNull();
#else
    bool hasPixmap = (pixmap() != nullptr);
#endif
    if (hasPixmap || d->isModeElideNone()) {
        return QLabel::sizeHint();
    }
    const QFontMetrics &fm = fontMetrics();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    QSize size(fm.horizontalAdvance(d->m_text), QLabel::sizeHint().height());
#else
    QSize size(fm.width(d->m_text), QLabel::sizeHint().height());
#endif
    return size;
}

QString DockLabel::text() const
{
    Q_D(const DockLabel);
    return d->m_text;
}

void DockLabel::setText(const QString &text)
{
    Q_D(DockLabel);
    d->m_text = text;
    if (d->isModeElideNone()) {
        QLabel::setText(text);
    } else {
        internal::setToolTip(this, text);
        d->elideText(this->size().width());
    }
}

void DockLabel::mouseReleaseEvent(QMouseEvent *e)
{
    QLabel::mouseReleaseEvent(e);
    if (e->button() != Qt::LeftButton) {
        return;
    }
    Q_EMIT clicked();
}

void DockLabel::resizeEvent(QResizeEvent *e)
{
    Q_D(DockLabel);
    if (!d->isModeElideNone()) {
        d->elideText(e->size().width());
    }
    QLabel::resizeEvent(e);
}

void DockLabel::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_EMIT doubleClicked();
    QLabel::mouseDoubleClickEvent(e);
}

QX_DOCK_END_NAMESPACE
