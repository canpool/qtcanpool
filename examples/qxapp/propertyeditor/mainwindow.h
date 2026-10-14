#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace QxApp
{
class QxSettingsDialog;
}
namespace QxCore
{
class QxSettings;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private Q_SLOTS:
    void openSettings();
private:
    void buildSettingsDialog();
    QxCore::QxSettings *m_settings = nullptr;
    QxApp::QxSettingsDialog *m_dialog = nullptr;
};

#endif   // MAINWINDOW_H
