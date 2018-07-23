#include "mainwindow.h"
#include <QHBoxLayout>
#include <qrencode.h>
#include <QLabel>
#include "qrcodegenerator.h"
#include "qtqrcode.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QLabel *label = new QLabel();
    QtQrcode qrcode;
    QPixmap code = qrcode.createQRCode("maminjie", 200);
    label->setPixmap(code);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), QString(),
                                                    tr("Image file (*.png)"));
    if(!fileName.isEmpty()){
        code.save(fileName);
    }
//    m_generator = new QrcodeGenerator();
//    m_generator->generateString("maminjie");
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);

    QWidget *w = new QWidget();
    w->setLayout(layout);
    setCentralWidget(w);
    resize(200,200);
}

MainWindow::~MainWindow()
{

}

