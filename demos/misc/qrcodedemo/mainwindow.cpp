#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qtqrcode.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qrcode = new QtQrcode();

    ui->lineEdit->setText(tr("qtcanpool"));
    on_genButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete qrcode;
}

void MainWindow::on_genButton_clicked()
{
    const QString &text = ui->lineEdit->text();
    if (text.trimmed().isEmpty()) {
        return;
    }

    int w = qMin(ui->label->width(), ui->label->height());
    w = qBound(100, w, 200);
    QPixmap code = qrcode->createQRCode(text, w);
    ui->label->setPixmap(code);
}

void MainWindow::on_saveButton_clicked()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    const QPixmap *pixmap = ui->label->pixmap();
    if (!pixmap || pixmap->isNull()) {
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), QString(),
                                                    tr("Image file (*.png)"));
    if (!fileName.isEmpty()) {
        pixmap->save(fileName);
    }
#else
    QPixmap pixmap = ui->label->pixmap();
    if (pixmap.isNull()) {
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), QString(),
                                                    tr("Image file (*.png)"));
    if (!fileName.isEmpty()) {
        pixmap.save(fileName);
    }
#endif
}
