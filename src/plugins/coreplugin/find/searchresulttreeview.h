/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "searchresultwindow.h"

#include <utils/itemviews.h>

namespace Core {
class SearchResultColor;

namespace Internal {

class SearchResultFilterModel;

class SearchResultTreeView : public Utils::TreeView
{
    Q_OBJECT

public:
    explicit SearchResultTreeView(QWidget *parent = nullptr);

    void setAutoExpandResults(bool expand);
    void setTextEditorFont(const QFont &font, const SearchResultColors &colors);
    void setTabWidth(int tabWidth);

    SearchResultFilterModel *model() const;
    void addResults(const QList<SearchResultItem> &items, SearchResult::AddMode mode);
    void setFilter(SearchResultFilter *filter);
    bool hasFilter() const;
    void showFilterWidget(QWidget *parent);

    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *e) override;

signals:
    void jumpToSearchResult(const SearchResultItem &item);
    void filterInvalidated();
    void filterChanged();

public slots:
    void clear();
    void emitJumpToSearchResult(const QModelIndex &index);

protected:
    SearchResultFilterModel *m_model;
    SearchResultFilter *m_filter = nullptr;
    bool m_autoExpandResults;
};

} // namespace Internal
} // namespace Core
