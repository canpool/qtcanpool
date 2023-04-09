#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QtQrcode;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_genButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::MainWindow *ui;
    QtQrcode *qrcode;
};
#endif // MAINWINDOW_H
