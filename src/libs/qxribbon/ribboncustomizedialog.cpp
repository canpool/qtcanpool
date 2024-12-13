﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribboncustomizedialog.h"
#include "ribboncustomizewidget.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QApplication>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

QX_RIBBON_BEGIN_NAMESPACE

/**
 * @brief The RibbonCustomizeDialogUi class
 */
class RibbonCustomizeDialogUi
{
public:
    RibbonCustomizeWidget *customWidget;
    QVBoxLayout *verticalLayoutMain;
    QHBoxLayout *horizontalLayoutButtonGroup;
    QPushButton *pushButtonCancel;
    QPushButton *pushButtonOk;
    QSpacerItem *spacerItemleft;

    void setupUi(RibbonBar *ribbonBar, QWidget *customizeDialog)
    {
        if (customizeDialog->objectName().isEmpty()) {
            customizeDialog->setObjectName(QStringLiteral("RibbonCustomizeDialog"));
        }
        customizeDialog->resize(800, 600);
        verticalLayoutMain = new QVBoxLayout(customizeDialog);
        verticalLayoutMain->setObjectName(QStringLiteral("verticalLayoutMain"));

        customWidget = new RibbonCustomizeWidget(ribbonBar, customizeDialog);
        customWidget->setObjectName(QStringLiteral("customWidget"));
        verticalLayoutMain->addWidget(customWidget);

        horizontalLayoutButtonGroup = new QHBoxLayout();
        horizontalLayoutButtonGroup->setObjectName(QStringLiteral("horizontalLayoutButtonGroup"));

        spacerItemleft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        horizontalLayoutButtonGroup->addItem(spacerItemleft);

        pushButtonOk = new QPushButton(customizeDialog);
        pushButtonOk->setObjectName(QStringLiteral("pushButtonOk"));
        horizontalLayoutButtonGroup->addWidget(pushButtonOk);

        pushButtonCancel = new QPushButton(customizeDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        horizontalLayoutButtonGroup->addWidget(pushButtonCancel);

        verticalLayoutMain->addItem(horizontalLayoutButtonGroup);
        retranslateUi(customizeDialog);
    }

    void retranslateUi(QWidget *customizeDialog)
    {
        customizeDialog->setWindowTitle(
            QApplication::translate("RibbonCustomizeDialog", "Customize Dialog", Q_NULLPTR));
        pushButtonOk->setText(QApplication::translate("RibbonCustomizeDialog", "OK", Q_NULLPTR));
        pushButtonCancel->setText(QApplication::translate("RibbonCustomizeDialog", "Cancel", Q_NULLPTR));
    }
};

////////////////////////////////////////////////////////////////////

RibbonCustomizeDialog::RibbonCustomizeDialog(RibbonBar *bar, QWidget *p, Qt::WindowFlags f)
    : QDialog(p, f)
    , ui(new RibbonCustomizeDialogUi)
{
    ui->setupUi(bar, this);

    connect(ui->pushButtonOk, &QPushButton::clicked, this, [this]() {
        ui->customWidget->apply();
        accept();
    });
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void RibbonCustomizeDialog::setupActionsManager(RibbonActionsManager *mgr)
{
    ui->customWidget->setupActionsManager(mgr);
}

bool RibbonCustomizeDialog::apply()
{
    // FIXME: 打开对话框，新建页后，应用，然后关闭对话框，不写文件，定制临时数据丢失
    // 当再次打开对话框，删除新建的页后，应用，然后选择ok，定制的临时数据都是删除数据，实际上要删除的对象已经不存在了，但是还会写到文件中
    // TODO: 先去掉应用按钮，未来应该私有化该接口
    return ui->customWidget->apply();
}

void RibbonCustomizeDialog::clear()
{
    ui->customWidget->clear();
}

bool RibbonCustomizeDialog::toXml(QXmlStreamWriter *xml) const
{
    return ui->customWidget->toXml(xml);
}

bool RibbonCustomizeDialog::toXml(const QString &xmlpath) const
{
    return ui->customWidget->toXml(xmlpath);
}

void RibbonCustomizeDialog::fromXml(QXmlStreamReader *xml)
{
    ui->customWidget->fromXml(xml);
}

void RibbonCustomizeDialog::fromXml(const QString &xmlpath)
{
    ui->customWidget->fromXml(xmlpath);
}

void RibbonCustomizeDialog::showEvent(QShowEvent *e)
{
    Q_UNUSED(e);
    bool isModal = this->isModal();
    ui->pushButtonCancel->setVisible(isModal);
    ui->pushButtonOk->setVisible(isModal);
}

bool QxRibbonCustomizeApplyFromXmlFile(const QString &filePath, RibbonBar *bar, RibbonActionsManager *mgr)
{
    QFile f(filePath);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    f.seek(0);
    QXmlStreamReader xml(&f);

    return RibbonCustomizeWidget::fromXml(&xml, bar, mgr);
}

QX_RIBBON_END_NAMESPACE
