#include "editablecontainer.h"

#include <QHBoxLayout>

EditableContainer::EditableContainer(QWidget *parent)
    : QWidget(parent)
    , m_widget(Q_NULLPTR)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setSpacing(0);
    lay->setContentsMargins(2, 0, 2, 0);
}

EditableContainer::~EditableContainer()
{

}

void EditableContainer::setWidget(QWidget *widget)
{
    if (m_widget) {
        layout()->removeWidget(m_widget);
    }
    m_widget = widget;
    layout()->addWidget(widget);
}

void EditableContainer::setContentsMargins(int left, int top, int right, int bottom)
{
    layout()->setContentsMargins(left, top, right, bottom);
}
