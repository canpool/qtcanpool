#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>

class QTabWidget;

class ViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewWidget(QWidget *parent = nullptr);

private:
    void createWindow();
    void createNavListView();
    void createNavTreeView();

private:
    QTabWidget *m_pTabWidget;
};

#endif // VIEWWIDGET_H
