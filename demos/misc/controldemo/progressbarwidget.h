#ifndef PROGRESSBARWIDGET_H
#define PROGRESSBARWIDGET_H

#include <QWidget>

class ProgressBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressBarWidget(QWidget *parent = nullptr);

private:
    void createWindow();
};

#endif // PROGRESSBARWIDGET_H
