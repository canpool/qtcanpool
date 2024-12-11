﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QDialog>

class QXmlStreamWriter;
class QXmlStreamReader;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonActionsManager;
class RibbonCustomizeDialogUi;
class RibbonBar;

/**
 * @brief RibbonCustomizeWidget 的对话框封装
 *
 * 此功能依赖于 @ref RibbonActionsManager ，RibbonActionsManager 建议作为 mianwindow 的成员变量，
 * RibbonActionsManager 可以快速绑定所有 QAction，详细见 RibbonActionsManager 的说明
 *
 * @note Ribbon 的自定义是基于步骤的，如果在窗口生成前调用了 @ref QxRibbonCustomizeApplyFromXmlFile 类似函数
 * 那么在对话框执行前为了保证同步需要调用 @ref RibbonCustomizeDialog::fromXml
 * 同步配置文件，这样再次修改后的配置文件就一致
 */
class QX_RIBBON_EXPORT RibbonCustomizeDialog : public QDialog
{
    Q_OBJECT
public:
    RibbonCustomizeDialog(RibbonBar *ribbonBar, QWidget *p = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    // 设置action管理器
    void setupActionsManager(RibbonActionsManager *mgr);

    // 应用所有的设定
    bool apply();

    // 清除所有动作
    void clear();

    // 转换为xml
    bool toXml(QXmlStreamWriter *xml) const;
    bool toXml(const QString &xmlpath) const;

    // 从xml中加载QList<RibbonCustomizeData>，对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
    void fromXml(QXmlStreamReader *xml);
    void fromXml(const QString &xmlpath);
protected:
    void showEvent(QShowEvent *e) override;
private:
    RibbonCustomizeDialogUi *ui;
};

/**
 * @brief 直接加载xml自定义ribbon配置文件用于ribbon的自定义显示
 * @param filePath xml配置文件
 * @param bar
 * @param mgr action管理器
 * @return 成功返回true
 * @note 重复加载一个配置文件会发生异常，为了避免此类事件发生，一般通过一个变量保证只加载一次，如：
 * @code
 * static bool has_call = false;
 * if (!has_call) {
 *     has_call = QxRibbonCustomizeApplyFromXmlFile("customize.xml", this->ribbonBar(), m_actMgr);
 * }
 * @endcode
 */
bool QX_RIBBON_EXPORT QxRibbonCustomizeApplyFromXmlFile(const QString &filePath, RibbonBar *bar,
                                                        RibbonActionsManager *mgr);

QX_RIBBON_END_NAMESPACE
