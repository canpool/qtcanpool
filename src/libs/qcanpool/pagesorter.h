/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#ifndef PAGESORTER_H
#define PAGESORTER_H

#include <QWidget>
#include "qcanpool_global.h"

class QLabel;
class QLineEdit;
class QIntValidator;

class QCANPOOL_SHARED_EXPORT PageSorter : public QWidget
{
    Q_OBJECT
public:
    explicit PageSorter(int block = 3, QWidget *parent = 0);
    ~PageSorter();

    int maxPage() const { return m_maxPage; }
    void setMaxPage(int max);

    int block() const { return m_block; }

    int currentPage() const { return m_currentPage; }
    void setCurrentPage(int page, bool emitted = false);

private:
    void createWindow();

    void setBlock(int size);
    void updatePageLabels();

signals:
    void currentPageChanged(int page);

public slots:

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    QLabel      *m_pPreviousPageLabel;
    QWidget     *m_pLeftPagesWidget;
    QLabel      *m_pLeftSeparaterLabel;
    QWidget     *m_pCenterPagesWidget;
    QLabel      *m_pRightSeparaterLabel;
    QWidget     *m_pRightPagesWidget;
    QLabel      *m_pNextPageLabel;
    QLineEdit   *m_pPageLineEdit;

    QIntValidator *m_pPageValidator;

    QList<QLabel *> m_pageLabels;

    int m_maxPage;
    int m_currentPage;
    int m_block;
};

#endif // PAGESORTER_H
