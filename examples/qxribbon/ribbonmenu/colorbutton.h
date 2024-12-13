#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QToolButton>

class ColorButton : public QToolButton
{
    Q_OBJECT
public:
    explicit ColorButton(const QColor &color, QWidget *parent = nullptr);

    const QColor &color() const;

signals:
    void colorClicked(const QColor &color, bool checked = false);
public slots:
    void setColor(const QColor &color);
private:
    QColor m_color;
};

#endif // COLORBUTTON_H
