/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALSELECT_H
#define MATERIALSELECT_H

#include "qxmaterial_global.h"

#include "materialtextfield.h"

class QWidget;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialSelectPrivate;

/*!
 * Material's select: a choice the field has already made for you, with the
 * other answers one open away.
 *
 * It stands on the text field - the same filled or outlined face, the same
 * floating label - and the answer shows where the editor would sit. The
 * field is not editable here: the choices are the application's, and the
 * list of them opens below the field, turns above when below has no room,
 * and keeps inside the screen when neither side has room. The row in force
 * carries the mark at its left; the keyboard walks the list and answers
 * with Enter.
 */
class QX_MATERIAL_EXPORT MaterialSelect : public MaterialTextField
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString currentText READ currentText NOTIFY currentIndexChanged)
public:
    explicit MaterialSelect(QWidget *parent = Q_NULLPTR);
    ~MaterialSelect() override;

    QStringList items() const;
    int count() const;
    int currentIndex() const;
    QString currentText() const;

    /*! The surface the answers open into; applications read it to re-style
     *  it, and the tests read it to know where it stood. */
    QWidget *popup() const;

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
    void paintContent(QPainter *painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(MaterialSelect)
    QX_DECLARE_PRIVATE(MaterialSelect)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALSELECT_H
