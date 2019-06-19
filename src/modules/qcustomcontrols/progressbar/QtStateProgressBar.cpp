#include "QtStateProgressBar.h"
#include <QPainter>
#include <QDebug>

QtStateProgressBar::QtStateProgressBar(QWidget *parent)
    : QWidget(parent),
      m_max(5), m_value(0),
      m_tip(""),
      m_barColor(QColor(255,104,124)),
      m_barBackColor(QColor(100,100,100)),
      m_noteColor(QColor(100,177,243)),
      m_stateColor(QColor(255,104,124)),
      m_tipColor(QColor(255,255,255)),
      m_tipBackColor(QColor(24,189,155)),
      m_type(NACK)
{
    m_innerLineHeight = (m_lineHeight > 2) ? (m_lineHeight - 2) : m_lineHeight;

    QFont font("SimSun", 10);
    setFont(font);
}

void QtStateProgressBar::setValue(int value)
{
    if (value < 0 || value > m_max || value == m_value) {
        return;
    }
    m_value = value;
    update();
}

void QtStateProgressBar::setMaximum(int max)
{
    if (max < 1 || max == m_max) {
        return;
    }
    if (m_value >= max) {
        m_value = max - 1;
    }
    m_max = max;
    update();
}

void QtStateProgressBar::setTip(QString tip)
{
    if (tip != m_tip) {
        m_tip = tip;
        update();
    }
}

void QtStateProgressBar::setNotes(const QStringList &notes)
{
    if (notes != m_notes) {
        m_notes = notes;
        update();
    }
}

void QtStateProgressBar::setStates(const QStringList &states)
{
    if (states != m_states) {
        m_states = states;
        update();
    }
}

void QtStateProgressBar::setType(Type type)
{
    m_type = type;
}

void QtStateProgressBar::setBarColor(const QColor &color)
{
    if (color != m_barColor) {
        m_barColor = color;
        update();
    }
}

void QtStateProgressBar::setBarBackColor(const QColor &color)
{
    if (color != m_barBackColor) {
        m_barBackColor = color;
        update();
    }
}

void QtStateProgressBar::setNoteColor(const QColor &color)
{
    if (color != m_noteColor) {
        m_noteColor = color;
        update();
    }
}

void QtStateProgressBar::setStateColor(const QColor &color)
{
    if (color != m_stateColor) {
        m_stateColor = color;
        update();
    }
}

void QtStateProgressBar::setTipColor(const QColor &color)
{
    if (color != m_tipColor) {
        m_tipColor = color;
        update();
    }
}

void QtStateProgressBar::setTipBackColor(const QColor &color)
{
    if (color != m_tipBackColor) {
        m_tipBackColor = color;
        update();
    }
}

QSize QtStateProgressBar::sizeHint() const
{
    return QSize(width(), width() / 5);
}

QSize QtStateProgressBar::minimumSizeHint() const
{
    int w = 125 * m_max;
    return QSize(w, w / 5);
}

void QtStateProgressBar::drawBackground(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(palette().window().color());
    p.drawRoundRect(rect(), 0, 0);
    p.restore();
}

void QtStateProgressBar::drawBarBackground(QPainter &p)
{
    for (int i = 0; i < m_max; ++i) {
        p.save();
        p.setPen(Qt::NoPen);
        p.setBrush(m_barBackColor);
        p.drawEllipse(QPointF(i*m_lineWidth, 0), m_bigRadius, m_bigRadius);
        if (i > 0) {
            p.drawRect((i-1)*m_lineWidth, -m_lineHeight/2, m_lineWidth, m_lineHeight);
        }
        p.restore();

        drawNote(p, i, m_barBackColor);
        drawState(p, i, m_barBackColor);
    }
}

void QtStateProgressBar::drawBar(QPainter &p)
{
    if (m_value < 1) {
        return;
    }

    for (int i = 0; i < m_value; ++i) {
        p.save();
        p.setPen(Qt::NoPen);
        p.setBrush(m_barColor);
        p.drawEllipse(QPointF(i*m_lineWidth, 0), m_smallRadius, m_smallRadius);
        if (i == m_value - 1) {
            if (m_value != m_max) {
                p.drawRect(i*m_lineWidth, -m_innerLineHeight/2,  m_lineWidth/2, m_innerLineHeight);
            }
        } else {
            p.drawRect(i*m_lineWidth, -m_innerLineHeight/2,  m_lineWidth, m_innerLineHeight);
        }
        p.restore();

        drawNote(p, i, m_noteColor);
        drawState(p, i, m_stateColor);
    }
    drawTip(p, m_value - 1);
}

void QtStateProgressBar::drawBarNumber(QPainter &p)
{
    p.save();
    p.setPen(Qt::white);

    for (int i = 0; i < m_max; ++i) {
        if (m_type == NACK || i >= m_value) {
            QString str = QString("%1").arg(i+1);
            QRect rect(i*m_lineWidth - m_bigRadius, -m_bigRadius, 2*m_bigRadius, 2*m_bigRadius);
            p.drawText(rect, Qt::AlignCenter, str);
        } else {
            QPen pen = p.pen();
            pen.setWidth(2);
            p.setPen(pen);
            QPointF center(i*m_lineWidth, 0);
            QPointF p1(center.x(), center.y()+m_smallRadius/3);
            QPointF p2(center.x()-m_smallRadius/3, center.y());
            QPointF p3(center.x()+m_smallRadius/2, center.y()-m_smallRadius/3);
            p.drawLine(p1, p2);
            p.drawLine(p1, p3);
        }
    }

    p.restore();
}

void QtStateProgressBar::drawNote(QPainter &p, int index, const QColor &color)
{
    if (index < m_notes.count()) {
        p.save();
        p.setPen(color);
        qreal textWidth = p.fontMetrics().width(m_notes[index]);
        QRectF noteRect(-textWidth*3/4 + index*m_lineWidth, -(m_bigRadius*4),
                        textWidth*3/2, m_bigRadius*2);
        p.drawText(noteRect, Qt::AlignCenter, m_notes[index]);
        p.restore();
    }
}

void QtStateProgressBar::drawState(QPainter &p, int index, const QColor &color)
{
    if (index < m_states.count()) {
        p.save();
        p.setPen(color);
        qreal textWidth = p.fontMetrics().width(m_states[index]);
        QRectF stateRect(-textWidth*3/4 + index*m_lineWidth, m_bigRadius*3/2,
                         textWidth*3/2, m_bigRadius*2);
        p.drawText(stateRect, Qt::AlignCenter, m_states[index]);
        p.restore();
    }
}

void QtStateProgressBar::drawTip(QPainter &p, int index)
{
    if ((m_notes.isEmpty() || index >= m_notes.count() || m_notes[index].isEmpty()) &&
            m_tip.isEmpty()) {
        return;
    }
    QString tip = m_tip;
    if (index < m_notes.count()) {
        tip = m_notes[index];
    }

    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(m_tipBackColor);
    int textWidth = p.fontMetrics().width(tip);
    textWidth = qMax(textWidth, m_bigRadius*2);
    QRectF rect(index*m_lineWidth - textWidth, -(m_bigRadius*4),
                textWidth*2, m_bigRadius*2);
    p.drawRoundedRect(rect, m_bigRadius/2, m_bigRadius/2);

    QPointF pt1(index*m_lineWidth, -m_bigRadius*3/2);
    QPointF pt2(index*m_lineWidth - m_bigRadius/2, -2*m_bigRadius);
    QPointF pt3(index*m_lineWidth + m_bigRadius/2, -2*m_bigRadius);
    QVector<QPointF> vec = {pt1, pt2, pt3};
    p.drawPolygon(vec);
    p.restore();

    p.save();
    p.setPen(m_tipColor);
    QFont font(p.font());
    font.setBold(true);
    p.setFont(font);
    p.drawText(rect, Qt::AlignCenter, tip);
    p.restore();
}

void QtStateProgressBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int diameter = qMin(width()/(m_max*5), height()*2/9);

    m_bigRadius = diameter/2;
    m_smallRadius = m_bigRadius*4/5;
    m_lineWidth = diameter*5;
    m_lineHeight = m_bigRadius/5;
    m_innerLineHeight = m_lineHeight*4/5;

    drawBackground(p);

    QFont font(p.font());
    font.setPixelSize(m_bigRadius);
    p.setFont(font);
    if (m_notes.isEmpty() && m_tip.isEmpty()) {
        p.translate(m_lineWidth/2, height()/4);
    } else {
        p.translate(m_lineWidth/2, height()/2);
    }

    drawBarBackground(p);
    drawBar(p);
    drawBarNumber(p);
}
