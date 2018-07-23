#ifndef CTABLEWIDGET_H
#define CTABLEWIDGET_H

#include <QTableWidget>

class CTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit CTableWidget(QWidget *parent = 0);

    void setBackgroundColor(int row, QColor color);
    void setTextColor(int row, QColor color);

    QList<int> selectedRows();

    bool swap(int number);
    bool moveto(int number);
    bool moveBlockTo(int number);

private:
    void swap(int aRow, int bRow);
    void moveto(int aRow, int bRow);
    void moveto(int aRow, int bRow, int cnt);
};

#endif // CTABLEWIDGET_H
