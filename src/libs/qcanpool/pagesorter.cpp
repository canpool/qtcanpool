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
#include "pagesorter.h"

#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QEvent>
#include <QKeyEvent>

PageSorter::PageSorter(int block, QWidget *parent)
    : QWidget(parent)
{
    setBlock(block);
    m_pageLabels.clear();

    createWindow();

    m_maxPage = 0;
    setMaxPage(1);

    // 分页组件的css,这个内容应该放到普通文件中然后加载
    QString qss = QString(".QLabel { color:black; padding: 1px; }")
            + QString(".QLabel[page=\"true\"] { color:black; padding: 1px; }")
            + QString(".QLabel[currentPage=\"true\"] { color: rgb(190, 0, 0);}")
            + QString(".QLabel[page=\"true\"]:hover { color: white; border-radius: 4px; background-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(53, 121, 238, 255), stop:1 rgba(0, 202, 237, 255));}");
    this->setStyleSheet(qss);
}

PageSorter::~PageSorter()
{

}

void PageSorter::setMaxPage(int max)
{
    max = qMax(max, 1);

    if (m_maxPage != max) {
        m_maxPage = max;
        m_currentPage = 1;
        updatePageLabels();
        m_pPageValidator->setRange(1, max);
    }
}

void PageSorter::setCurrentPage(int page, bool emitted)
{
    page = qMax(page, 1);
    page = qMin(page, m_maxPage);

    if (page != m_currentPage) {
        m_currentPage = page;
        updatePageLabels();

        if (emitted) {
            emit currentPageChanged(page);
        }
    }
}

void PageSorter::createWindow()
{
    m_pPreviousPageLabel = new QLabel("<<");
    m_pPreviousPageLabel->setToolTip(tr("previous page"));
    m_pLeftPagesWidget = new QWidget();
    m_pLeftSeparaterLabel = new QLabel("..");
    m_pCenterPagesWidget = new QWidget();
    m_pRightSeparaterLabel = new QLabel("..");
    m_pRightPagesWidget = new QWidget();
    m_pNextPageLabel = new QLabel(">>");
    m_pNextPageLabel->setToolTip(tr("next page"));

    QLabel *pFixedLabel1 = new QLabel(tr("the"));
    QLabel *pFixedLabel2 = new QLabel(tr("page"));
    m_pPageLineEdit = new QLineEdit();
    m_pPageLineEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    m_pPageValidator = new QIntValidator(this);
    m_pPageValidator->setRange(1,10000000);
    m_pPageLineEdit->setValidator(m_pPageValidator);

    // install filter
    m_pPreviousPageLabel->setProperty("page", "true");
    m_pPreviousPageLabel->installEventFilter(this);
    m_pNextPageLabel->setProperty("page", "true");
    m_pNextPageLabel->installEventFilter(this);
    m_pPageLineEdit->installEventFilter(this);

    QHBoxLayout *leftLayout = new QHBoxLayout();
    QHBoxLayout *centerLayout = new QHBoxLayout();
    QHBoxLayout *rightLayout = new QHBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(0);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    for (int i = 0; i < m_block * 3; ++i) {
        QLabel *label = new QLabel(QString::number(i + 1));
        label->setProperty("page", "true");
        label->installEventFilter(this);

        m_pageLabels.append(label);

        if (i < m_block) {
            leftLayout->addWidget(label);
        } else if (i < m_block * 2) {
            centerLayout->addWidget(label);
        } else {
            rightLayout->addWidget(label);
        }
    }
    m_pLeftPagesWidget->setLayout(leftLayout);
    m_pCenterPagesWidget->setLayout(centerLayout);
    m_pRightPagesWidget->setLayout(rightLayout);

    QHBoxLayout *pMainLayout = new QHBoxLayout();
    pMainLayout->setSpacing(2);
    pMainLayout->setContentsMargins(2,2,2,2);
    pMainLayout->addStretch();
    pMainLayout->addWidget(m_pPreviousPageLabel);
    pMainLayout->addWidget(m_pLeftPagesWidget);
    pMainLayout->addWidget(m_pLeftSeparaterLabel);
    pMainLayout->addWidget(m_pCenterPagesWidget);
    pMainLayout->addWidget(m_pRightSeparaterLabel);
    pMainLayout->addWidget(m_pRightPagesWidget);
    pMainLayout->addWidget(m_pNextPageLabel);
    pMainLayout->addWidget(pFixedLabel1);
    pMainLayout->addWidget(m_pPageLineEdit);
    pMainLayout->addWidget(pFixedLabel2);
    pMainLayout->addStretch();

    setLayout(pMainLayout);
}

void PageSorter::setBlock(int size)
{
    // 为了便于计算, block size 必须是奇数, 且最小为3
    size = qMax(size, 3);
    if (size % 2 == 0) {
        ++size;
    }
    m_block = size;
}

void PageSorter::updatePageLabels()
{
    m_pLeftSeparaterLabel->hide();
    m_pRightSeparaterLabel->hide();

    if (m_maxPage <= m_block * 3) {
        for (int i = 0; i < m_pageLabels.count(); i += 1) {
            QLabel *label = m_pageLabels.at(i);

            if (i < m_maxPage) {
                label->setText(QString::number(i + 1));
                label->show();
            } else {
                label->hide();
            }

            if (m_currentPage - 1 == i) {
                label->setProperty("currentPage", "true");
            } else {
                label->setProperty("currentPage", "false");
            }

            label->setStyleSheet("/**/");
        }
        return;
    }

    // 以下情况为maxPageNumber大于blockSize * 3, 所有的页码label都要显示
    // c 为 currentPage
    // n 为 block size
    // m 为 maxPage

    // 1. c ∈ [1, n + n/2 + 1]: 显示前 n * 2 个, 后 n 个: 只显示右边的分隔符
    // 2. c ∈ [m - n - n/2, m]: 显示前 n 个, 后 n * 2 个: 只显示左边的分隔符
    // 3. 显示[1, n], [c - n/2, c + n/2], [m - 2*n + 1, m]: 两个分隔符都显示

    int c = m_currentPage;
    int n = m_block;
    int m = m_maxPage;
    int centerStartPage = 0;

    if (c >= 1 && c <= n + n / 2 + 1) {
        // 1. c ∈ [1, n + n/2 + 1]: 显示前 n * 2 个, 后 n 个: 只显示右边的分隔符
        centerStartPage = n + 1;
        m_pRightSeparaterLabel->show();
    } else if (c >= m - n - n / 2 && c <= m) {
        // 2. c ∈ [m - n - n/2, m]: 显示前 n 个, 后 n * 2 个: 只显示左边的分隔符
        centerStartPage = m - n - n + 1;
        m_pLeftSeparaterLabel->show();
    } else {
        // 3. 显示[1, n], [c - n/2, c + n/2], [m - n + 1, m]: 两个分隔符都显示
        centerStartPage = c - n / 2;
        m_pRightSeparaterLabel->show();
        m_pLeftSeparaterLabel->show();
    }

    for (int i = 0; i < n; ++i) {
        m_pageLabels.at(i)->setText(QString::number(i + 1));                     // 前面 n 个
        m_pageLabels.at(n + i)->setText(QString::number(centerStartPage + i));   // 中间 n 个
        m_pageLabels.at(3 * n - i - 1)->setText(QString::number(m - i));         // 后面 n 个
    }

    for (int i = 0; i < m_pageLabels.count(); ++i) {
        QLabel *label = m_pageLabels.at(i);
        int page = label->text().toInt();
        if (page == m_currentPage) {
            label->setProperty("currentPage", "true");
        } else {
            label->setProperty("currentPage", "false");
        }

        label->setStyleSheet("/**/");
        label->show();
    }
}

bool PageSorter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        int page = -1;
        if (watched == m_pPreviousPageLabel) { page = currentPage() - 1; }

        if (watched == m_pNextPageLabel) { page = currentPage() + 1; }

        for (int i = 0; i < m_pageLabels.count(); ++i) {
            if (watched == m_pageLabels.at(i)) {
                page = m_pageLabels.at(i)->text().toInt();
                break;
            }
        }

        if (-1 != page) {
            setCurrentPage(page, true);
            return true;
        }
    }

    if (watched == m_pPageLineEdit && event->type() == QEvent::KeyRelease) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return) {
            setCurrentPage(m_pPageLineEdit->text().toInt(), true);
            //setMaxPage(ui->pageLineEdit->text().toInt()); // 测试生成多个页码
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}
