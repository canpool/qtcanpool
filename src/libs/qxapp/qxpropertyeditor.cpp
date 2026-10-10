/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxpropertyeditor.h"
#include "qxpropertyeditor_p.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>

QX_APP_BEGIN_NAMESPACE

/*!
 * A button that shows a colour and opens the colour dialog.
 *
 * It exists so that the colour is a value the form can read back, rather than a
 * style sheet nobody owns. The dialog is modal and would block a test forever,
 * so it is only ever reached by a click - filling the control never opens it.
 */
class QxPropertyColorButton : public QPushButton
{
    Q_OBJECT
public:
    explicit QxPropertyColorButton(QWidget *parent = Q_NULLPTR)
        : QPushButton(parent)
    {
        QObject::connect(this, &QAbstractButton::clicked, this, &QxPropertyColorButton::chooseColor);
    }

    QColor color() const
    {
        return m_color;
    }
    void setColor(const QColor &color)
    {
        if (m_color == color) {
            return;
        }
        m_color = color;
        apply();
        Q_EMIT colorChanged(m_color);
    }

Q_SIGNALS:
    void colorChanged(const QColor &color);
private:
    void chooseColor()
    {
        const QColor picked = QColorDialog::getColor(m_color, this, tr("Choose a colour"));
        if (picked.isValid()) {
            setColor(picked);
        }
    }

    void apply()
    {
        QPixmap swatch(20, 12);
        swatch.fill(m_color.isValid() ? m_color : Qt::transparent);
        setIcon(swatch);
        setText(m_color.isValid() ? m_color.name() : tr("(none)"));
    }

    QColor m_color;
};

/*! The same for a font; currentFont() keeps QWidget::font() out of the way. */
class QxPropertyFontButton : public QPushButton
{
    Q_OBJECT
public:
    explicit QxPropertyFontButton(QWidget *parent = Q_NULLPTR)
        : QPushButton(parent)
    {
        QObject::connect(this, &QAbstractButton::clicked, this, &QxPropertyFontButton::chooseFont);
    }

    QFont currentFont() const
    {
        return m_font;
    }
    void setCurrentFont(const QFont &font)
    {
        if (m_font == font) {
            return;
        }
        m_font = font;
        apply();
        Q_EMIT fontChanged(m_font);
    }

Q_SIGNALS:
    void fontChanged(const QFont &font);
private:
    void chooseFont()
    {
        bool accepted = false;
        const QFont picked = QFontDialog::getFont(&accepted, m_font, this, tr("Choose a font"));
        if (accepted) {
            setCurrentFont(picked);
        }
    }

    void apply()
    {
        setText(QStringLiteral("%1, %2").arg(m_font.family()).arg(m_font.pointSize()));
        setFont(m_font);
    }

    QFont m_font;
};

namespace
{

/*! Range of the two spin boxes: wide enough that a stored value is never clamped. */
const int kIntMinimum = -1000000000;
const int kIntMaximum = 1000000000;
const double kDoubleMinimum = -1.0e12;
const double kDoubleMaximum = 1.0e12;
/*! Decimals of a double: enough to show a stored value without inventing precision. */
const int kDoubleDecimals = 3;

/*! The text of a row's label; the key stands in when there is none. */
QString labelFor(const QxProperty &property)
{
    return property.label.isEmpty() ? property.key : property.label;
}

/*! Asks the file dialog for whatever the property describes. */
QString browsePath(const QString &start, QxProperty::PathMode mode, QWidget *parent)
{
    switch (mode) {
    case QxProperty::Directory:
        return QFileDialog::getExistingDirectory(parent, QObject::tr("Choose a folder"), start);
    case QxProperty::AnyFile:
        return QFileDialog::getSaveFileName(parent, QObject::tr("Choose a file"), start);
    case QxProperty::ExistingFile:
        break;
    }
    return QFileDialog::getOpenFileName(parent, QObject::tr("Choose a file"), start);
}

/*!
 * The value a property of \a type holds, in the type it is stored as.
 *
 * What comes out of a configuration file is a QVariant of whatever the format
 * produced, and what the dialog writes back has to be comparable with what the
 * controls hold - an int stored as a string would never match the spin box's
 * value. Converting on the way in is what keeps isModified() honest.
 */
QVariant normalize(const QVariant &value, QxProperty::Type type)
{
    switch (type) {
    case QxProperty::Bool:
        return value.isValid() ? QVariant(value.toBool()) : QVariant();
    case QxProperty::Int: {
        bool ok = false;
        const int number = value.toInt(&ok);
        return ok ? QVariant(number) : QVariant();
    }
    case QxProperty::Double: {
        bool ok = false;
        const double number = value.toDouble(&ok);
        return ok ? QVariant(number) : QVariant();
    }
    case QxProperty::String:
    case QxProperty::Text:
    case QxProperty::Enum:
    case QxProperty::Path:
        return value.isValid() ? QVariant(value.toString()) : QVariant();
    case QxProperty::Color: {
        // Either the value already is a colour, or it is a string the way a
        // configuration file writes one - "#rrggbb", or a name.
        QColor color = value.canConvert<QColor>() ? qvariant_cast<QColor>(value) : QColor();
        if (!color.isValid()) {
            color = QColor(value.toString());
        }
        return color.isValid() ? QVariant::fromValue(color) : QVariant();
    }
    case QxProperty::Font: {
        if (value.canConvert<QFont>()) {
            return QVariant::fromValue(qvariant_cast<QFont>(value));
        }
        QFont font;
        if (value.isValid() && font.fromString(value.toString())) {
            return QVariant::fromValue(font);
        }
        return QVariant();
    }
    }
    return value;
}

}   // namespace

QxPropertyEditorPrivate::QxPropertyEditorPrivate() = default;

void QxPropertyEditorPrivate::init()
{
    Q_Q(QxPropertyEditor);

    m_layout = new QVBoxLayout(q);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_form = new QFormLayout();
    m_layout->addLayout(m_form);
    // Everything is packed at the top: an editor that stretches its rows over
    // the whole dialog is harder to read than one that leaves room below.
    m_layout->addStretch(1);
}

int QxPropertyEditorPrivate::indexOf(const QString &key) const
{
    for (int i = 0; i < m_rows.count(); ++i) {
        if (m_rows.at(i).property.key == key) {
            return i;
        }
    }
    return -1;
}

QWidget *QxPropertyEditorPrivate::createField(const QxProperty &property, QWidget **input)
{
    Q_Q(QxPropertyEditor);

    switch (property.type) {
    case QxProperty::Bool: {
        // A check box carries its own text, so it spans both columns rather than
        // getting a label it would only repeat.
        QCheckBox *check = new QCheckBox(labelFor(property), q);
        *input = check;
        return check;
    }
    case QxProperty::Int: {
        QSpinBox *spin = new QSpinBox(q);
        spin->setRange(kIntMinimum, kIntMaximum);
        *input = spin;
        return spin;
    }
    case QxProperty::Double: {
        QDoubleSpinBox *spin = new QDoubleSpinBox(q);
        spin->setRange(kDoubleMinimum, kDoubleMaximum);
        spin->setDecimals(kDoubleDecimals);
        *input = spin;
        return spin;
    }
    case QxProperty::String: {
        QLineEdit *line = new QLineEdit(q);
        if (property.readOnly) {
            line->setReadOnly(true);
        }
        *input = line;
        return line;
    }
    case QxProperty::Text: {
        QTextEdit *edit = new QTextEdit(q);
        if (property.readOnly) {
            edit->setReadOnly(true);
        }
        *input = edit;
        return edit;
    }
    case QxProperty::Enum: {
        QComboBox *combo = new QComboBox(q);
        combo->addItems(property.choices);
        *input = combo;
        return combo;
    }
    case QxProperty::Color: {
        QxPropertyColorButton *button = new QxPropertyColorButton(q);
        *input = button;
        return button;
    }
    case QxProperty::Font: {
        QxPropertyFontButton *button = new QxPropertyFontButton(q);
        *input = button;
        return button;
    }
    case QxProperty::Path: {
        // The only type that needs a container: the line edit is the input, the
        // button beside it only fills it in.
        QWidget *field = new QWidget(q);
        QHBoxLayout *layout = new QHBoxLayout(field);
        layout->setContentsMargins(0, 0, 0, 0);
        QLineEdit *line = new QLineEdit(field);
        layout->addWidget(line, 1);
        QPushButton *browse = new QPushButton(QStringLiteral("..."), field);
        browse->setFixedWidth(browse->fontMetrics().boundingRect(QStringLiteral("...")).width() + 24);
        layout->addWidget(browse);
        QObject::connect(browse, &QAbstractButton::clicked, q, [line, property]() {
            const QString path = browsePath(line->text(), property.pathMode, line);
            if (!path.isEmpty()) {
                line->setText(path);
            }
        });
        *input = line;
        return field;
    }
    }
    return Q_NULLPTR;
}

void QxPropertyEditorPrivate::insertRow(QFormLayout *form, const QxProperty &property)
{
    if (!form) {
        return;
    }

    QWidget *input = Q_NULLPTR;
    QWidget *field = createField(property, &input);
    if (!field) {
        qWarning("QxPropertyEditor: '%s' has a type there is no control for", qPrintable(property.key));
        return;
    }

    if (property.type == QxProperty::Bool) {
        form->addRow(field);
    } else {
        form->addRow(labelFor(property), field);
    }

    Row row;
    row.property = property;
    row.property.value = normalize(property.value.isValid() ? property.value : property.defaultValue, property.type);
    row.field = field;
    row.input = input;
    m_rows.append(row);

    field->setEnabled(!property.readOnly);
    field->setToolTip(property.tooltip);

    // Filled before the control is listened to, so the starting value is not
    // reported as an edit.
    m_updating = true;
    writeField(row, row.property.value);
    m_updating = false;

    if (!m_initial.contains(property.key)) {
        m_initial.insert(property.key, row.property.value);
    }
    connectField(m_rows.count() - 1);
}

void QxPropertyEditorPrivate::connectField(int index)
{
    Q_Q(QxPropertyEditor);

    const Row row = m_rows.at(index);
    if (!row.input) {
        return;
    }

    // Whatever the control reports, the row is re-read from the control and
    // stored. The guard keeps the form from hearing the value it just wrote.
    auto commit = [this, index]() {
        if (m_updating) {
            return;
        }
        setRowValue(index, readField(m_rows.at(index)));
    };

    switch (row.property.type) {
    case QxProperty::Bool:
        QObject::connect(qobject_cast<QCheckBox *>(row.input), &QCheckBox::toggled, q, commit);
        break;
    case QxProperty::Int:
        QObject::connect(qobject_cast<QSpinBox *>(row.input), QOverload<int>::of(&QSpinBox::valueChanged), q, commit);
        break;
    case QxProperty::Double:
        QObject::connect(qobject_cast<QDoubleSpinBox *>(row.input),
                         QOverload<double>::of(&QDoubleSpinBox::valueChanged), q, commit);
        break;
    case QxProperty::String:
    case QxProperty::Path:
        QObject::connect(qobject_cast<QLineEdit *>(row.input), &QLineEdit::textChanged, q, commit);
        break;
    case QxProperty::Text:
        QObject::connect(qobject_cast<QTextEdit *>(row.input), &QTextEdit::textChanged, q, commit);
        break;
    case QxProperty::Enum:
        QObject::connect(qobject_cast<QComboBox *>(row.input), &QComboBox::currentTextChanged, q, commit);
        break;
    case QxProperty::Color:
        // Not clicked(): the dialog runs after the click, so the colour only
        // exists once the button says it changed.
        QObject::connect(qobject_cast<QxPropertyColorButton *>(row.input), &QxPropertyColorButton::colorChanged, q,
                         commit);
        break;
    case QxProperty::Font:
        QObject::connect(qobject_cast<QxPropertyFontButton *>(row.input), &QxPropertyFontButton::fontChanged, q,
                         commit);
        break;
    }
}

QVariant QxPropertyEditorPrivate::readField(const Row &row) const
{
    if (!row.input) {
        return QVariant();
    }

    switch (row.property.type) {
    case QxProperty::Bool:
        return qobject_cast<QCheckBox *>(row.input)->isChecked();
    case QxProperty::Int:
        return qobject_cast<QSpinBox *>(row.input)->value();
    case QxProperty::Double:
        return qobject_cast<QDoubleSpinBox *>(row.input)->value();
    case QxProperty::String:
    case QxProperty::Path:
        return qobject_cast<QLineEdit *>(row.input)->text();
    case QxProperty::Text:
        return qobject_cast<QTextEdit *>(row.input)->toPlainText();
    case QxProperty::Enum:
        return qobject_cast<QComboBox *>(row.input)->currentText();
    case QxProperty::Color:
        return QVariant::fromValue(qobject_cast<QxPropertyColorButton *>(row.input)->color());
    case QxProperty::Font:
        return QVariant::fromValue(qobject_cast<QxPropertyFontButton *>(row.input)->currentFont());
    }
    return QVariant();
}

void QxPropertyEditorPrivate::writeField(const Row &row, const QVariant &value)
{
    if (!row.input) {
        return;
    }

    switch (row.property.type) {
    case QxProperty::Bool:
        qobject_cast<QCheckBox *>(row.input)->setChecked(value.toBool());
        break;
    case QxProperty::Int:
        qobject_cast<QSpinBox *>(row.input)->setValue(value.toInt());
        break;
    case QxProperty::Double:
        qobject_cast<QDoubleSpinBox *>(row.input)->setValue(value.toDouble());
        break;
    case QxProperty::String:
    case QxProperty::Path: {
        QLineEdit *line = qobject_cast<QLineEdit *>(row.input);
        if (line->text() != value.toString()) {
            line->setText(value.toString());
        }
        break;
    }
    case QxProperty::Text: {
        QTextEdit *edit = qobject_cast<QTextEdit *>(row.input);
        if (edit->toPlainText() != value.toString()) {
            edit->setPlainText(value.toString());
        }
        break;
    }
    case QxProperty::Enum: {
        QComboBox *combo = qobject_cast<QComboBox *>(row.input);
        const int index = combo->findText(value.toString());
        if (index >= 0) {
            combo->setCurrentIndex(index);
            break;
        }
        // A value that came out of a configuration file is shown even when the
        // list of choices moved on without it: silently dropping it would make
        // the editor lie about what is stored.
        if (!value.toString().isEmpty()) {
            combo->addItem(value.toString());
            combo->setCurrentIndex(combo->count() - 1);
        }
        break;
    }
    case QxProperty::Color:
        qobject_cast<QxPropertyColorButton *>(row.input)->setColor(qvariant_cast<QColor>(value));
        break;
    case QxProperty::Font:
        qobject_cast<QxPropertyFontButton *>(row.input)->setCurrentFont(qvariant_cast<QFont>(value));
        break;
    }
}

void QxPropertyEditorPrivate::setRowValue(int index, const QVariant &value)
{
    Q_Q(QxPropertyEditor);

    if (index < 0 || index >= m_rows.count()) {
        return;
    }

    Row &row = m_rows[index];
    if (row.property.value == value) {
        return;
    }

    row.property.value = value;
    m_updating = true;
    writeField(row, value);
    m_updating = false;

    Q_EMIT q->valueChanged(row.property.key, value);
    updateModified();
}

void QxPropertyEditorPrivate::updateModified()
{
    Q_Q(QxPropertyEditor);

    bool modified = false;
    for (const Row &row : m_rows) {
        if (m_initial.value(row.property.key) != row.property.value) {
            modified = true;
            break;
        }
    }

    if (modified == m_modified) {
        return;
    }
    m_modified = modified;
    Q_EMIT q->modifiedChanged(m_modified);
}

QxPropertyEditor::QxPropertyEditor(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new QxPropertyEditorPrivate())
{
    Q_D(QxPropertyEditor);
    d->setPublic(this);
    d->init();
}

QxPropertyEditor::~QxPropertyEditor()
{
    QX_FINI_PRIVATE();
}

int QxPropertyEditor::count() const
{
    Q_D(const QxPropertyEditor);
    return d->m_rows.count();
}

QStringList QxPropertyEditor::keys() const
{
    Q_D(const QxPropertyEditor);
    QStringList keys;
    for (const QxPropertyEditorPrivate::Row &row : d->m_rows) {
        keys.append(row.property.key);
    }
    return keys;
}

bool QxPropertyEditor::contains(const QString &key) const
{
    Q_D(const QxPropertyEditor);
    return d->indexOf(key) >= 0;
}

void QxPropertyEditor::addProperty(const QxProperty &property)
{
    Q_D(QxPropertyEditor);

    if (property.key.isEmpty()) {
        qWarning("QxPropertyEditor: a property needs a non-empty key");
        return;
    }
    if (d->indexOf(property.key) >= 0) {
        qWarning("QxPropertyEditor: a property named '%s' already exists", qPrintable(property.key));
        return;
    }

    d->insertRow(d->m_form, property);
}

void QxPropertyEditor::addGroup(const QString &title, const QList<QxProperty> &properties)
{
    Q_D(QxPropertyEditor);

    QGroupBox *group = new QGroupBox(title, this);
    QFormLayout *form = new QFormLayout(group);
    // Before the stretch, so the section keeps the room at the bottom free.
    d->m_layout->insertWidget(d->m_layout->count() - 1, group);

    // Everything added from here lands in this section until the next group.
    d->m_form = form;
    for (const QxProperty &property : properties) {
        addProperty(property);
    }
}

QVariant QxPropertyEditor::value(const QString &key) const
{
    Q_D(const QxPropertyEditor);
    const int index = d->indexOf(key);
    if (index < 0) {
        return QVariant();
    }
    return d->m_rows.at(index).property.value;
}

void QxPropertyEditor::setValue(const QString &key, const QVariant &value)
{
    Q_D(QxPropertyEditor);

    const int index = d->indexOf(key);
    if (index < 0) {
        qWarning("QxPropertyEditor: there is no property named '%s'", qPrintable(key));
        return;
    }

    const QVariant normalized = normalize(value, d->m_rows.at(index).property.type);
    if (value.isValid() && !normalized.isValid()) {
        qWarning("QxPropertyEditor: '%s' cannot hold the value it was given", qPrintable(key));
        return;
    }
    d->setRowValue(index, normalized);
}

void QxPropertyEditor::setAllValues(const QHash<QString, QVariant> &values)
{
    Q_D(QxPropertyEditor);

    // A key that is not a property here is a typo or a stale entry. Reporting it
    // beats keeping it: an unknown key would be silently dropped from the form.
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        const int index = d->indexOf(it.key());
        if (index < 0) {
            qWarning("QxPropertyEditor: '%s' is not a property of this editor", qPrintable(it.key()));
            continue;
        }
        // The baseline first: whatever is loaded is what "unchanged" means, so
        // the value has to be seen as modified-free from the start.
        d->m_initial.insert(it.key(), normalize(it.value(), d->m_rows.at(index).property.type));
        d->setRowValue(index, d->m_initial.value(it.key()));
    }
    d->updateModified();
}

QHash<QString, QVariant> QxPropertyEditor::allValues() const
{
    Q_D(const QxPropertyEditor);
    QHash<QString, QVariant> values;
    for (const QxPropertyEditorPrivate::Row &row : d->m_rows) {
        values.insert(row.property.key, row.property.value);
    }
    return values;
}

bool QxPropertyEditor::isModified() const
{
    Q_D(const QxPropertyEditor);
    return d->m_modified;
}

void QxPropertyEditor::reset()
{
    Q_D(QxPropertyEditor);
    for (int i = 0; i < d->m_rows.count(); ++i) {
        const QxProperty &property = d->m_rows.at(i).property;
        d->setRowValue(i, normalize(property.defaultValue, property.type));
    }
}

void QxPropertyEditor::resetToInitial()
{
    Q_D(QxPropertyEditor);
    for (int i = 0; i < d->m_rows.count(); ++i) {
        const QString key = d->m_rows.at(i).property.key;
        d->setRowValue(i, d->m_initial.value(key));
    }
}

QX_APP_END_NAMESPACE

#include "qxpropertyeditor.moc"
