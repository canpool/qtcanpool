#include "tabssettingseditor.h"
#include <QVBoxLayout>
#include <QColorDialog>
#include <qxmaterial/materialtabbar.h>

QX_MATERIAL_USE_NAMESPACE

TabsSettingsEditor::TabsSettingsEditor(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TabsSettingsForm),
      m_tabBar(new MaterialTabBar)
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
    layout->addWidget(m_tabBar);
    layout->setAlignment(m_tabBar, Qt::AlignHCenter);

    m_tabBar->addTab("Media");
    m_tabBar->addTab("Playback");
    m_tabBar->addTab("Audio");
    m_tabBar->addTab("Video");
    m_tabBar->addTab("Tools");

    m_tabBar->setMinimumWidth(700);

    setupForm();
}

TabsSettingsEditor::~TabsSettingsEditor()
{
    delete ui;
}

void TabsSettingsEditor::setupForm()
{
}

void TabsSettingsEditor::updateWidget()
{
}
