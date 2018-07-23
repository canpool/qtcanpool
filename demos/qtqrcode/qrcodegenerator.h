#ifndef QRCODEGENERATOR_H
#define QRCODEGENERATOR_H

#include <QWidget>
#include "qrencode.h"
#include <QPainter>

class QrcodeGenerator : public QWidget
{
    Q_OBJECT
public:
    explicit QrcodeGenerator(QWidget *parent = 0);
    ~QrcodeGenerator();

    void generateString(QString str);
    bool saveImage(QString fileName, int size=250);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

private:
    void draw(QPainter &painter, int width, int height);
    QString string;
    QRcode *qr;
};

#endif // QRCODEGENERATOR_H
