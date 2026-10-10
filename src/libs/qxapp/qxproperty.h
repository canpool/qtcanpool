/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXPROPERTY_H
#define QXPROPERTY_H

#include "qxapp_global.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

QX_APP_BEGIN_NAMESPACE

/*!
 * One setting, described well enough to be edited.
 *
 * A property is a value with a name, not a row of widgets: it says what the
 * setting is called, what kind of value it takes, what it is worth now and what
 * it is worth when nobody has said anything. QxPropertyEditor turns a list of
 * them into a form, and a settings dialog turns them into keys of a
 * configuration file.
 *
 * @code
 * QxProperty theme;
 * theme.key = QStringLiteral("appearance/theme");
 * theme.label = tr("Theme");
 * theme.type = QxProperty::Enum;
 * theme.choices = QStringList{tr("Light"), tr("Dark")};
 * theme.value = QStringLiteral("Light");
 * theme.defaultValue = QStringLiteral("Light");
 * @endcode
 *
 * The key is the part that has to be stable: it is what the value is stored
 * under, so renaming it abandons every value a user ever chose. The label, the
 * tooltip and the choices are presentation and may be translated freely.
 *
 * The struct is plain data on purpose - there is nothing to inherit from and no
 * state to hide, so a caller can build one with aggregate initialisation or
 * keep a static table of them.
 */
struct QX_APP_EXPORT QxProperty {
    Q_GADGET
public:
    /*! The kind of value the property takes, and therefore the control it is edited with. */
    enum Type {
        Bool,   /*!< a check box */
        Int,    /*!< a spin box */
        Double, /*!< a spin box with decimals */
        String, /*!< a single line edit */
        Text,   /*!< a multi line edit */
        Enum,   /*!< a combo box over choices */
        Color,  /*!< a colour button */
        Font,   /*!< a font button */
        Path,   /*!< a line edit with a browse button */
    };
    Q_ENUM(Type)

    /*! What the browse button of a Path property is looking for. */
    enum PathMode {
        ExistingFile, /*!< a file that has to be there */
        AnyFile,      /*!< a file, whether or not it exists yet */
        Directory,    /*!< a folder */
    };
    Q_ENUM(PathMode)

    /*! Stable name the value is stored under. Never translate it. */
    QString key;
    /*! Text shown next to the control; the key is used when it is empty. */
    QString label;
    Type type = String;
    /*! The current value. */
    QVariant value;
    /*! The value the property has when nobody has set it. */
    QVariant defaultValue;
    /*! The choices an Enum offers; unused by every other type. */
    QStringList choices;
    /*! Text shown when the pointer rests on the control. */
    QString tooltip;
    /*! What the browse button picks, for Path. */
    PathMode pathMode = ExistingFile;
    /*! Whether the control is shown disabled. */
    bool readOnly = false;
};

QX_APP_END_NAMESPACE

#endif   // QXPROPERTY_H
