/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTCOMBOBOX_H
#define FLUENTCOMBOBOX_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

class QWidget;

QX_FLUENT_BEGIN_NAMESPACE

class FluentComboBoxPrivate;

/*!
 * Fluent's combo box: a choice the page has already made for you, with the
 * other answers one open away.
 *
 * It is not editable - the choices are the application's, and an application
 * that wants a free answer wants the text box. The face shows what is in
 * force; opening it lays the answers out below, turns above when there is no
 * room below, and keeps inside the screen when there is no room either way.
 * The keyboard walks the list and answers with Enter, and leaves it alone
 * with Escape.
 */
class QX_FLUENT_EXPORT FluentComboBox : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString currentText READ currentText NOTIFY currentIndexChanged)
public:
    explicit FluentComboBox(QWidget *parent = Q_NULLPTR);
    ~FluentComboBox() override;

    QStringList items() const;
    int count() const;
    int currentIndex() const;
    QString currentText() const;

    /*! The surface the answers open into. It is the combo's child in spirit
     *  and nobody's on the screen; applications read it to re-style it, and
     *  the tests read it to know where it stood. */
    QWidget *popup() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setItems(const QStringList &items);
    void addItem(const QString &item);
    void clear();
    void setCurrentIndex(int index);
    void showPopup();
    void hidePopup();
Q_SIGNALS:
    /*! The choice in force moved to \a index, and only when it moved. */
    void currentIndexChanged(int index);
protected:
    qreal cornerRadius() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(FluentComboBox)
    QX_DECLARE_PRIVATE(FluentComboBox)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTCOMBOBOX_H
