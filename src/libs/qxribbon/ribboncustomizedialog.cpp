/**
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
    QPushButton *pushButtonApply;
    QSpacerItem *spacerItemleft;

    void setupUi(RibbonWindow *ribbonWindow, QWidget *customizeDialog)
    {
        if (customizeDialog->objectName().isEmpty()) {
            customizeDialog->setObjectName(QStringLiteral("RibbonCustomizeDialog"));
        }
        customizeDialog->resize(800, 600);
        verticalLayoutMain = new QVBoxLayout(customizeDialog);
        verticalLayoutMain->setObjectName(QStringLiteral("verticalLayoutMain"));

        customWidget = new RibbonCustomizeWidget(ribbonWindow, customizeDialog);
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

RibbonCustomizeDialog::RibbonCustomizeDialog(RibbonWindow *ribbonWindow, QWidget *p, Qt::WindowFlags f)
    : QDialog(p, f), ui(new RibbonCustomizeDialogUi)
{
    ui->setupUi(ribbonWindow, this);
    initConnection();
}

/**
 * @brief 设置action管理器
 *
 * 等同@ref RibbonCustomizeWidget::setupActionsManager
 * @param mgr
 */
void RibbonCustomizeDialog::setupActionsManager(RibbonActionsManager *mgr)
{
    ui->customWidget->setupActionsManager(mgr);
}

void RibbonCustomizeDialog::initConnection()
{
    connect(ui->pushButtonOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &QDialog::reject);
}

/**
 * @brief 等同RibbonCustomizeWidget::applys
 *
 * @ref RibbonCustomizeWidget::applys
 * @return
 */
bool RibbonCustomizeDialog::applys()
{
    // FIXME: 打开对话框，新建页后，应用，然后关闭对话框，不写文件，定制临时数据丢失
    // 当再次打开对话框，删除新建的页后，应用，然后选择ok，定制的临时数据都是删除数据，实际上要删除的对象已经不存在了，但是还会写到文件中
    // TODO: 先去掉应用按钮，未来应该私有化该接口
    return ui->customWidget->applys();
}

/**
 * @brief 清除所有动作
 *
 * @ref RibbonCustomizeWidget::clear
 */
void RibbonCustomizeDialog::clear()
{
    ui->customWidget->clear();
}

/**
 * @brief 转换为xml
 *
 * @ref RibbonCustomizeWidget::toXml
 * @param xml
 * @return
 */
bool RibbonCustomizeDialog::toXml(QXmlStreamWriter *xml) const
{
    return ui->customWidget->toXml(xml);
}

/**
 * @brief 等同RibbonCustomizeWidget::toXml
 * @ref RibbonCustomizeWidget::toXml
 * @param xmlpath
 * @return
 */
bool RibbonCustomizeDialog::toXml(const QString &xmlpath) const
{
    return ui->customWidget->toXml(xmlpath);
}

/**
 * @brief 等同RibbonCustomizeWidget::fromXml
 * @param xml
 */
void RibbonCustomizeDialog::fromXml(QXmlStreamReader *xml)
{
    ui->customWidget->fromXml(xml);
}

/**
 * @brief 等同RibbonCustomizeWidget::fromXml
 * @param xmlpath
 */
void RibbonCustomizeDialog::fromXml(const QString &xmlpath)
{
    ui->customWidget->fromXml(xmlpath);
}

bool QxRibbonCustomizeApplyFromXmlFile(const QString &filePath, RibbonWindow *w, RibbonActionsManager *mgr)
{
    QFile f(filePath);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    f.seek(0);
    QXmlStreamReader xml(&f);

    return RibbonCustomizeWidget::fromXml(&xml, w, mgr);
}

QX_RIBBON_END_NAMESPACE
