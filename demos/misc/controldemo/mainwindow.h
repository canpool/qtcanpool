#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class QTreeWidget;
class QTreeWidgetItem;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createWindow();

    void addWidget(const QString &name, QWidget *w);

private slots:
    void slotItemClicked(QTreeWidgetItem *item, int column);

private:
    QTreeWidget *m_pTreeWidget;
    QStackedWidget *m_pStackedWidget;
    QMap<QTreeWidgetItem *, QWidget *> m_itemMap;
};

#endif // MAINWINDOW_H
