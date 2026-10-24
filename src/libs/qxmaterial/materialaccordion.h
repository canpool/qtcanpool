/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALACCORDION_H
#define MATERIALACCORDION_H

#include "qxmaterial_global.h"

#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialAccordionPrivate;

/*!
 * Material's accordion: a column of headers, each keeping a body it can open.
 *
 * Every header is a name and a chevron; opening one gives its body room on the
 * family's own duration, and the body travels out from under the header the
 * way the expander's does rather than appearing. Whether a second header may
 * be open at the same time is the column's answer: \c Mode::Single keeps one
 * open and closes the one before, \c Mode::Multiple lets every header answer
 * for itself.
 *
 * The body is the application's widget: the accordion gives it room and
 * geometry and does not own it.
 */
class QX_MATERIAL_EXPORT MaterialAccordion : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
public:
    /*! How many headers the column lets open at once. */
    enum class Mode {
        Single,  /*!< One at a time: opening a header closes the one before. */
        Multiple /*!< Every header answers for itself. */
    };
    Q_ENUM(Mode)

    explicit MaterialAccordion(QWidget *parent = Q_NULLPTR);
    ~MaterialAccordion() override;

    /*! Adds a header named \a title keeping \a content, and answers with its
     *  place in the column. */
    int addSection(const QString &title, QWidget *content = Q_NULLPTR);
    /*! How many headers the column has. */
    int sectionCount() const;
    /*! The name of the header at \a index, empty when there is none. */
    QString sectionTitle(int index) const;
    /*! The body the header at \a index keeps, null when there is none. */
    QWidget *sectionContent(int index) const;
    /*! Whether the header at \a index has its body out. */
    bool isSectionExpanded(int index) const;
    /*! How many headers may be open at once. */
    Mode mode() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setSectionTitle(int index, const QString &title);
    /*! Opens or closes the header at \a index. Opening one under \c
     *  Mode::Single closes whichever was open. */
    void setSectionExpanded(int index, bool expanded);
    void setMode(Mode mode);
Q_SIGNALS:
    /*! A header opened or closed, whether by the pointer, the keyboard, the
     *  application, or by another header opening under \c Mode::Single. */
    void sectionExpandedChanged(int index, bool expanded);
    void modeChanged(Mode mode);
private:
    Q_DISABLE_COPY(MaterialAccordion)
    QX_DECLARE_PRIVATE(MaterialAccordion)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALACCORDION_H
