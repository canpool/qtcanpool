/***************************************************************************
 **
 **  Copyright (C) 2018-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#ifndef PAGESORTER_H
#define PAGESORTER_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QIntValidator;

class PageSorter : public QWidget
{
    Q_OBJECT
public:
    explicit PageSorter(int block = 3, QWidget *parent = nullptr);
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
