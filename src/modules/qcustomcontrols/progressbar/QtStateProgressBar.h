#ifndef QTSTATEPROGRESSBAR_H
#define QTSTATEPROGRESSBAR_H

#include <QWidget>

class QtStateProgressBar : public QWidget
{
    Q_OBJECT
public:
    enum Type { NACK, ACK };
    explicit QtStateProgressBar(QWidget *parent = nullptr);

    void setTip(QString tip);
    void setNotes(const QStringList &notes);
    void setStates(const QStringList &states);
    void setType(Type type);

    void setBarColor(const QColor &color);
    void setBarBackColor(const QColor &color);
    void setNoteColor(const QColor &color);
    void setStateColor(const QColor &color);
    void setTipColor(const QColor &color);
    void setTipBackColor(const QColor &color);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public slots:
    void setValue(int value);
    void setMaximum(int max);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    void drawBackground(QPainter &p);
    void drawBarBackground(QPainter &p);
    void drawBar(QPainter &p);
    void drawBarNumber(QPainter &p);
    void drawNote(QPainter &p, int index, const QColor &color);
    void drawState(QPainter &p, int index, const QColor &color);
    void drawTip(QPainter &p, int index);

private:
    int m_max, m_value;
    int m_bigRadius;
    int m_smallRadius;

    int m_lineHeight;
    int m_lineWidth;
    int m_innerLineHeight;

    QString m_tip;
    QStringList m_notes;
    QStringList m_states;

    QColor m_barColor;
    QColor m_barBackColor;
    QColor m_noteColor;
    QColor m_stateColor;
    QColor m_tipColor;
    QColor m_tipBackColor;

    Type m_type;
};

#endif // QTSTATEPROGRESSBAR_H
