#include "snackbarsettingseditor.h"
#include <QVBoxLayout>
#include <QColorDialog>
#include <qxmaterial/materialsnackbar.h>

QX_MATERIAL_USE_NAMESPACE

SnackbarSettingsEditor::SnackbarSettingsEditor(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::SnackbarSettingsForm),
      m_snackbar(new MaterialSnackbar)
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    QWidget *widget = new QWidget;
    layout->addWidget(widget);

    QWidget *canvas = new QWidget;
    canvas->setStyleSheet("QWidget { background: white; }");
    layout->addWidget(canvas);

    ui->setupUi(widget);
    layout->setContentsMargins(20, 20, 20, 20);

    layout = new QVBoxLayout;
    canvas->setLayout(layout);
    canvas->setMaximumHeight(300);

    m_snackbar->setParent(this);

    setupForm();

    connect(ui->showSnackbarButton, SIGNAL(pressed()), this, SLOT(showSnackbar()));
}

SnackbarSettingsEditor::~SnackbarSettingsEditor()
{
    delete ui;
}

void SnackbarSettingsEditor::setupForm()
{
}

void SnackbarSettingsEditor::updateWidget()
{
}

void SnackbarSettingsEditor::showSnackbar()
{
    m_snackbar->addMessage(QString("Snack attack!"));
}
