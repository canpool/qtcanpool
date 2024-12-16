#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qxwindow/qxwindow_global.h>
#include <QtWidgets/QMainWindow>

QX_WINDOW_BEGIN_NAMESPACE
class WindowAgentWidget;
class WindowStyleAgent;
QX_WINDOW_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    enum Theme {
        Dark,
        Light,
    };
    Q_ENUM(Theme)

Q_SIGNALS:
    void themeChanged();

protected:
    bool event(QEvent *event) override;

private:
    void installWindowAgent();
    void loadStyleSheet(Theme theme);

    Theme currentTheme{};

    QX_WINDOW_PREPEND_NAMESPACE(WindowAgentWidget) *windowAgent;
};

#endif // MAINWINDOW_H
