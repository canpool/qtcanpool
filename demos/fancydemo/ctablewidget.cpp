/*!
 * \file ctablewidget.cpp
 * \brief
 * \author canpool@163.com
 * \version v0.1.0
 * \date
 * \b
 * \li
 *
 */
#include "ctablewidget.h"
#include "cwidgets.h"

#include <QStyledItemDelegate>

class NoFocusDelegate : public QStyledItemDelegate
{
protected:
    void paint(QPainter* painter,
               const QStyleOptionViewItem & option,
               const QModelIndex &index) const;
};

void NoFocusDelegate::paint(QPainter* painter,
                             const QStyleOptionViewItem & option,
                             const QModelIndex &index) const
{
     QStyleOptionViewItem itemOption(option);
     if (itemOption.state & QStyle::State_HasFocus)
     {
        itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
     }

     QStyledItemDelegate::paint(painter, itemOption, index);
}

CTableWidget::CTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
//    this->setIconSize(QSize(24,24));

//    this->horizontalHeader()->setDefaultSectionSize(150); //
//    this->horizontalHeader()->setClickable(false);

    QFont font = this->horizontalHeader()->font();
    font.setBold(true);
    this->horizontalHeader()->setFont(font);

    this->horizontalHeader()->setVisible(true);
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
//    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
//    this->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif
//    this->resizeColumnsToContents();
    this->horizontalHeader()->setStretchLastSection(true);
    this->horizontalHeader()->setHighlightSections(false);
    this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    this->verticalHeader()->setVisible(true);
//    this->verticalHeader()->setDefaultSectionSize(40);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
//    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection); //Ctrl,Shift,Ctrl+A
//    this->resizeRowsToContents();
    this->resizeRowToContents(0);

    this->setShowGrid(false);
    this->setFrameShape(QFrame::NoFrame);
//    this->horizontalHeader()->setStyleSheet("QHeaderView::section{background: rgb(168,208,141);}");
//    this->horizontalHeader()->setStyleSheet("QHeaderView::section{background: rgb(132,189,157);}");
    this->horizontalHeader()->setStyleSheet("QHeaderView::section{background: rgb(211,231,220);}");

    this->setMouseTracking(true);
//    this->setStyleSheet("selection-background-color:pink");

    this->setItemDelegate(new NoFocusDelegate());

    // ctrl
    QTableWidgetSelectionRange range(0,0,this->rowCount()-1,this->columnCount()-1);
    this->setRangeSelected(range,true);
    this->setFocus();

//    connect(this->horizontalHeader(),SIGNAL(sectionClicked(int)),this, SLOT(slotHeaderClicked(int)));
//    connect(this, SIGNAL(cellClicked(int,int)), this, SLOT(slotCellClicked(int,int)));
//    connect(this, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(slotCellDoubleClicked(int,int)));
//    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));

    this->setStyleSheet("QHeaderView::section:checked {background-color: red;}");
}

void CTableWidget::setBackgroundColor(int row, QColor color)
{
    for (int col = 0; col < this->columnCount(); col++)
    {
        QTableWidgetItem *item = this->item(row, col);
        if(item)
            item->setBackgroundColor(color);
    }
}

void CTableWidget::setTextColor(int row, QColor color)
{
    for (int col = 0; col < this->columnCount(); col++)
    {
        QTableWidgetItem *item = this->item(row, col);
        if(item)
            item->setTextColor(color);
    }
}

QList<int> CTableWidget::selectedRows()
{
    QList<QTableWidgetSelectionRange> ranges = this->selectedRanges();
    int count = ranges.count();

    QList<int> rows;
    for(int i = 0;i < count; i++){
        int topRow = ranges.at(i).topRow();
        int bottomRow = ranges.at(i).bottomRow();
        for(int j = topRow; j <= bottomRow; j++){
            rows.append(j);
        }
    }
    std::sort(rows.begin(), rows.end()); // qt5 (qt4 qSort)
    return rows;
}

bool CTableWidget::swap(int number)
{
    if(number <= 0 || number >= this->rowCount())
        return false;
    int curRow = this->currentRow();
    int swapRow = number - 1;
    if(curRow == swapRow)
        return false;
    swap(curRow, swapRow);
    return true;
}

bool CTableWidget::moveto(int number)
{
    if(number <= 0 || number >= this->rowCount())
        return false;
    int curRow = this->currentRow();
    int movetoRow = number - 1;
    if(curRow == movetoRow)
        return false;
    if(curRow < movetoRow){ // move down, 1)insert moveto row; 2) moveto row move down one row.
        moveto(curRow, movetoRow+1);
    }else if(curRow > movetoRow){ // move up, 1)insert moveto row; 2)current row move down one row.
        moveto(curRow+1, movetoRow);
    }
    return true;
}

bool CTableWidget::moveBlockTo(int number)
{
    if(number <= 0 || number >= this->rowCount())
        return false;
    QList<int> rows = selectedRows();
    if(rows.count() == 0)
        return false;

    int movetoRow = number - 1;
    if(rows.contains(movetoRow))
        return false;

    int rowCnt = rows.count();
    if(rows.at(rowCnt-1) < movetoRow){ // move down
        moveto(rows.at(0), movetoRow+1, rowCnt);
        QTableWidgetSelectionRange range(movetoRow-rowCnt+1, 0, movetoRow, this->columnCount()-1);
        this->setRangeSelected(range, true);
    }else if(rows.at(0) > movetoRow){ // move up
        moveto(rows.at(0)+rowCnt, movetoRow, rowCnt);
        QTableWidgetSelectionRange range(movetoRow, 0, movetoRow+rowCnt-1, this->columnCount()-1);
        this->setRangeSelected(range, true);
    }
    return true;
}

void CTableWidget::swap(int aRow, int bRow)
{
    int columns = this->columnCount();
    for (int i = 0; i < columns; i++)
    {
        QTableWidgetItem *aItem = this->takeItem(aRow, i);
        QTableWidgetItem *bItem = this->takeItem(bRow, i);

        this->setItem(aRow,i, bItem);
        this->setItem(bRow,i, aItem);
    }
    this->selectRow(bRow);
}

void CTableWidget::moveto(int aRow, int bRow)
{
    int columns = this->columnCount();
    this->insertRow(bRow);

    for (int i = 0; i < columns; i++)
    {
        QTableWidgetItem *aItem = this->takeItem(aRow, i);
        this->setItem(bRow,i, aItem);
    }
    this->selectRow(bRow);
    this->removeRow(aRow);
}

void CTableWidget::moveto(int aRow, int bRow, int cnt)
{
    int columns = this->columnCount();
    for(int i = 0; i < cnt; i++){
        this->insertRow(bRow);
    }

    for(int i = 0; i < cnt; i++){
        for (int j = 0; j < columns; j++)
        {
            QTableWidgetItem *aItem = this->takeItem(aRow+i, j);
            this->setItem(bRow+i, j, aItem);
        }
    }
    for(int i = 0; i < cnt; i++){
        this->removeRow(aRow);
    }
}
