#include "mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setMinimumSize(45,45);
}

MainWindow::~MainWindow()
{

}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    if(this->width() > 45 && this->height() > 45)
        drawWindowShadow(p);

    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    p.drawRoundedRect(QRect(18,18,this->width() - 36,this->height() - 36),2.0f,2.0f);
}

void MainWindow::drawWindowShadow(QPainter &p)
{
    QList<QPixmap> pixmaps;
    pixmaps.append(QPixmap(":/resources/shadow_left.png"));
    pixmaps.append(QPixmap(":/resources/shadow_right.png"));
    pixmaps.append(QPixmap(":/resources/shadow_top.png"));
    pixmaps.append(QPixmap(":/resources/shadow_bottom.png"));
    pixmaps.append(QPixmap(":/resources/shadow_corner1.png"));
    pixmaps.append(QPixmap(":/resources/shadow_corner2.png"));
    pixmaps.append(QPixmap(":/resources/shadow_corner3.png"));
    pixmaps.append(QPixmap(":/resources/shadow_corner4.png"));

    p.drawPixmap(0,0,20,20,pixmaps[4]);
    p.drawPixmap(this->width() - 20,0,20,20,pixmaps[5]);
    p.drawPixmap(0,this->height() - 20,20,20,pixmaps[6]);
    p.drawPixmap(this->width() - 20,this->height() - 20,20,20,pixmaps[7]);

    p.drawPixmap(0,20,20,this->height() - 40,pixmaps[0].scaled(20,this->height() - 40));
    p.drawPixmap(this->width() - 20,20,20,this->height() - 40,pixmaps[1].scaled(20,this->height() - 40));
    p.drawPixmap(20,0,this->width() - 40,20,pixmaps[2].scaled(this->width() - 40,20));
    p.drawPixmap(20,this->height() - 20,this->width() - 40,20,pixmaps[3].scaled(this->width() - 40,20));
}

bool MainWindow::winEvent(MSG *message, long *result)
{

    switch(message->message)
    {
    case WM_NCHITTEST:
        int xPos = GET_X_LPARAM(message->lParam) - this->frameGeometry().x();
        int yPos = GET_Y_LPARAM(message->lParam) - this->frameGeometry().y();
        if(this->childAt(xPos,yPos) == 0)
        {
            *result = HTCAPTION;
        }else{
            return false;
        }
        if(xPos > 18 && xPos < 22)
            *result = HTLEFT;
        if(xPos > (this->width() - 22) && xPos < (this->width() - 18))
            *result = HTRIGHT;
        if(yPos > 18 && yPos < 22)
            *result = HTTOP;
        if(yPos > (this->height() - 22) && yPos < (this->height() - 18))
            *result = HTBOTTOM;
        if(xPos > 18 && xPos < 22 && yPos > 18 && yPos < 22)
            *result = HTTOPLEFT;
        if(xPos > (this->width() - 22) && xPos < (this->width() - 18) && yPos > 18 && yPos < 22)
            *result = HTTOPRIGHT;
        if(xPos > 18 && xPos < 22 && yPos > (this->height() - 22) && yPos < (this->height() - 18))
            *result = HTBOTTOMLEFT;
        if(xPos > (this->width() - 22) && xPos < (this->width() - 18) && yPos > (this->height() - 22) && yPos < (this->height() - 18))
            *result = HTBOTTOMRIGHT;

        return true;

    }

    return false;
}
