/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialautocomplete.h"

#include "materialtokens.h"

#include "qxcontrols/qxpopupsurface.h"
#include "qxcontrols/qxstatelayer.h"

#include <QtCore/QEvent>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <functional>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The list is the family's rows: forty-eight tall, sixteen either side, a wash
// over the row the pointer is on and a tint under the one the keyboard is on.
constexpr qreal kRowHeight = 48.0;
constexpr qreal kRowPadding = 16.0;
constexpr qreal kHighlightOpacity = 0.08;
constexpr qreal kHoverOpacity = 0.04;
constexpr qreal kSurfaceRadius = 4.0;

QPointF eventPosOf(QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class MaterialAutocompletePrivate;

/*! The surface the answers open into: the shared surface carrying this
 *  family's rows, as the select's list does. */
class AutocompletePopup : public QxControls::QxPopupSurface
{
public:
    explicit AutocompletePopup(MaterialAutocomplete *field)
        : QxControls::QxPopupSurface(field)
        , m_field(field)
    {
        setSurfaceRadius(kSurfaceRadius);
        setMouseTracking(true);
    }

    std::function<void(int)> onChoose;

    void setMatches(const QStringList &matches)
    {
        m_matches = matches;
        m_highlight = matches.isEmpty() ? -1 : 0;
        m_hover = -1;
        m_offset = 0;
    }

    int count() const
    {
        return m_matches.count();
    }
    QString matchAt(int row) const
    {
        return m_matches.value(row);
    }
    int highlight() const
    {
        return m_highlight;
    }

    /*! Moves the keyboard's row by \a delta, within the answers there are. */
    void moveHighlight(int delta)
    {
        if (m_matches.isEmpty()) {
            return;
        }
        m_highlight = qBound(0, m_highlight + delta, m_matches.count() - 1);
        ensureVisible();
        update();
    }

    QSize contentSizeHint() const override
    {
        const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
        int widest = 0;
        for (const QString &match : m_matches) {
            widest = qMax(widest, metrics.horizontalAdvance(match));
        }
        const int room = int(2 * kRowPadding + widest);
        // No narrower than the field it belongs to: a list of answers that does
        // not reach across the question looks like it is answering something
        // else.
        const int width = qMax(room, m_field->width());
        return QSize(width, int(qMin(count(), visibleRows()) * kRowHeight));
    }

    void paintContent(QPainter *painter, const QRectF &content) override
    {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setFont(MaterialType::font(MaterialType::Scale::Body));
        const int rows = qMin(count() - m_offset, visibleRows());
        for (int row = 0; row < rows; ++row) {
            const int index = m_offset + row;
            const QRectF room(content.left(), content.top() + row * kRowHeight, content.width(), kRowHeight);
            if (index == m_highlight) {
                // The keyboard's row is tinted rather than marked: what the
                // keyboard is on is where it would answer from, and the tint
                // says so without claiming the answer has been given.
                painter->setPen(Qt::NoPen);
                painter->setBrush(QxControls::QxStateLayer::layered(
                    palette().color(QPalette::Window), palette().color(QPalette::WindowText), kHighlightOpacity));
                painter->drawRect(room);
            } else if (index == m_hover) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(QxControls::QxStateLayer::layered(
                    palette().color(QPalette::Window), palette().color(QPalette::WindowText), kHoverOpacity));
                painter->drawRect(room);
            }
            painter->setPen(palette().color(QPalette::WindowText));
            painter->drawText(room.adjusted(kRowPadding, 0.0, -kRowPadding, 0.0), Qt::AlignVCenter | Qt::TextSingleLine,
                              matchAt(index));
        }
    }
protected:
    void mouseMoveEvent(QMouseEvent *event) override
    {
        const int row = rowAt(eventPosOf(event));
        if (row != m_hover) {
            m_hover = row;
            update();
        }
        QxControls::QxPopupSurface::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        const int row = rowAt(eventPosOf(event));
        if (row >= 0 && onChoose) {
            onChoose(row);
            return;
        }
        QxControls::QxPopupSurface::mouseReleaseEvent(event);
    }
private:
    int visibleRows() const
    {
        // A screen holds what it holds; the list does not promise a number.
        const QScreen *screen = QxControls::QxPopupSurface::screen();
        const int room = screen ? int(screen->availableGeometry().height() / 2 / kRowHeight) : 8;
        return qMax(1, room);
    }

    void ensureVisible()
    {
        if (m_highlight < m_offset) {
            m_offset = m_highlight;
        } else if (m_highlight >= m_offset + visibleRows()) {
            m_offset = m_highlight - visibleRows() + 1;
        }
        m_offset = qBound(0, m_offset, qMax(0, count() - visibleRows()));
    }

    int rowAt(const QPointF &pos) const
    {
        const QRectF content = contentRect();
        if (!content.contains(pos)) {
            return -1;
        }
        const int row = m_offset + int((pos.y() - content.top()) / kRowHeight);
        return row >= 0 && row < count() ? row : -1;
    }

    MaterialAutocomplete *m_field = Q_NULLPTR;
    QStringList m_matches;
    int m_highlight = -1;
    int m_hover = -1;
    int m_offset = 0;
};

class MaterialAutocompletePrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialAutocomplete)

    QStringList options;
    QStringList matches;
    QString answered;
    bool freeSolo = false;
    bool openOnFocus = true;
    AutocompletePopup *popup = Q_NULLPTR;
};

MaterialAutocomplete::MaterialAutocomplete(QWidget *parent)
    : MaterialTextField(parent)
    , d_ptr(new MaterialAutocompletePrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->popup = new AutocompletePopup(this);
    d_ptr->popup->onChoose = [this](int row) {
        takeRow(row);
    };

    QLineEdit *edit = lineEdit();
    edit->installEventFilter(this);
    // Typing is a question about what there is, so it asks it: the list is
    // filtered as the writing goes in, and it offers what is left - or nothing,
    // which is an answer too.
    connect(edit, &QLineEdit::textEdited, this, [this](const QString &text) {
        // A value that no longer matches what the field says is not a value.
        answer(QString());
        refilter();
        Q_EMIT textEdited(text);
        if (d_ptr->matches.isEmpty() && !d_ptr->options.isEmpty()) {
            hideList();
        } else if (!d_ptr->options.isEmpty()) {
            d_ptr->popup->open();
        }
    });
}

MaterialAutocomplete::~MaterialAutocomplete(){QX_FINI_PRIVATE()}

QStringList MaterialAutocomplete::options() const
{
    return d_ptr->options;
}

void MaterialAutocomplete::setOptions(const QStringList &options)
{
    if (d_ptr->options == options) {
        return;
    }
    d_ptr->options = options;
    refilter();
    if (d_ptr->popup->isVisible()) {
        if (d_ptr->matches.isEmpty()) {
            hideList();
        } else {
            d_ptr->popup->open();
        }
    }
}

bool MaterialAutocomplete::isFreeSolo() const
{
    return d_ptr->freeSolo;
}

void MaterialAutocomplete::setFreeSolo(bool on)
{
    d_ptr->freeSolo = on;
}

bool MaterialAutocomplete::opensOnFocus() const
{
    return d_ptr->openOnFocus;
}

void MaterialAutocomplete::setOpenOnFocus(bool on)
{
    d_ptr->openOnFocus = on;
}

QString MaterialAutocomplete::text() const
{
    return lineEdit()->text();
}

void MaterialAutocomplete::setText(const QString &text)
{
    if (lineEdit()->text() == text) {
        return;
    }
    lineEdit()->setText(text);
    answer(QString());
    refilter();
}

QString MaterialAutocomplete::currentText() const
{
    if (!d_ptr->answered.isEmpty()) {
        return d_ptr->answered;
    }
    // A free field answers with the writing itself; a closed one has no answer
    // until one of its rows has been taken.
    return d_ptr->freeSolo ? lineEdit()->text() : QString();
}

bool MaterialAutocomplete::isListOpen() const
{
    return d_ptr->popup->isVisible();
}

QString MaterialAutocomplete::highlightedOption() const
{
    return d_ptr->popup->matchAt(d_ptr->popup->highlight());
}

QWidget *MaterialAutocomplete::popup() const
{
    return d_ptr->popup;
}

void MaterialAutocomplete::showAll()
{
    if (d_ptr->options.isEmpty()) {
        return;
    }
    d_ptr->popup->setMatches(d_ptr->options);
    d_ptr->matches = d_ptr->options;
    d_ptr->popup->open();
}

void MaterialAutocomplete::hideList()
{
    d_ptr->popup->hide();
}

void MaterialAutocomplete::acceptCurrent()
{
    const int row = d_ptr->popup->highlight();
    if (row >= 0) {
        takeRow(row);
        return;
    }
    if (d_ptr->freeSolo) {
        answer(lineEdit()->text());
        hideList();
    }
}

void MaterialAutocomplete::clear()
{
    lineEdit()->clear();
    answer(QString());
    hideList();
}

void MaterialAutocomplete::takeRow(int row)
{
    const QString option = d_ptr->popup->matchAt(row);
    if (option.isEmpty()) {
        return;
    }
    lineEdit()->setText(option);
    answer(option);
    hideList();
    Q_EMIT optionChosen(option);
}

void MaterialAutocomplete::refilter()
{
    const QString needle = lineEdit()->text();
    d_ptr->matches.clear();
    for (const QString &option : d_ptr->options) {
        if (needle.isEmpty() || option.contains(needle, Qt::CaseInsensitive)) {
            d_ptr->matches.append(option);
        }
    }
    d_ptr->popup->setMatches(d_ptr->matches);
}

void MaterialAutocomplete::answer(const QString &text)
{
    if (d_ptr->answered == text) {
        return;
    }
    d_ptr->answered = text;
    Q_EMIT currentTextChanged(currentText());
}

void MaterialAutocomplete::focusInEvent(QFocusEvent *event)
{
    MaterialTextField::focusInEvent(event);
    if (d_ptr->openOnFocus && !d_ptr->popup->isVisible()) {
        showAll();
    }
}

bool MaterialAutocomplete::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != lineEdit()) {
        return MaterialTextField::eventFilter(watched, event);
    }
    if (event->type() == QEvent::FocusIn && d_ptr->openOnFocus && !d_ptr->popup->isVisible()) {
        // The keyboard arriving is the field being asked what there is.
        showAll();
        return MaterialTextField::eventFilter(watched, event);
    }
    if (event->type() != QEvent::KeyPress) {
        return MaterialTextField::eventFilter(watched, event);
    }
    auto *key = static_cast<QKeyEvent *>(event);
    switch (key->key()) {
    case Qt::Key_Down:
        // The arrows walk the answers rather than the text: a field that moved
        // its cursor down has done nothing a reader asked for.
        if (!d_ptr->popup->isVisible()) {
            showAll();
        }
        d_ptr->popup->moveHighlight(1);
        return true;
    case Qt::Key_Up:
        if (d_ptr->popup->isVisible()) {
            d_ptr->popup->moveHighlight(-1);
            return true;
        }
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (d_ptr->popup->isVisible()) {
            acceptCurrent();
            return true;
        }
        if (d_ptr->freeSolo) {
            answer(lineEdit()->text());
            return true;
        }
        break;
    case Qt::Key_Escape:
        if (d_ptr->popup->isVisible()) {
            hideList();
            return true;
        }
        break;
    default:
        break;
    }
    return MaterialTextField::eventFilter(watched, event);
}

QX_MATERIAL_END_NAMESPACE
