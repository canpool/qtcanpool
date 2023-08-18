#ifndef EDITABLECONTAINER_H
#define EDITABLECONTAINER_H

#include <QWidget>

class EditableContainer : public QWidget
{
    Q_OBJECT
public:
    explicit EditableContainer(QWidget *parent = nullptr);
    virtual ~EditableContainer();

    void setWidget(QWidget *widget);
    void setContentsMargins(int left, int top, int right, int bottom);

private:
    QWidget *m_widget;
};

#endif // EDITABLECONTAINER_H
