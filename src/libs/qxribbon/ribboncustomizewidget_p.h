/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribboncustomizedata.h"

#include <QWidget>

class QXmlStreamWriter;
class QXmlStreamReader;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonBar;
class RibbonActionsManager;

/**
 * @brief 转换为xml
 *
 * 此函数仅会写element，不会写document相关内容，因此如果需要写document，
 * 需要在此函数前调用QXmlStreamWriter::writeStartDocument(),在此函数后调用QXmlStreamWriter::writeEndDocument()
 * @param xml QXmlStreamWriter指针
 * @note 注意，在传入QXmlStreamWriter之前，需要设置编码为utf-8:xml->setCodec("utf-8");
 * @note 由于QXmlStreamWriter在QString作为io时，是不支持编码的，而此又无法保证自定义过程不出现中文字符，
 * 因此，QXmlStreamWriter不应该通过QString进行构造，如果需要用到string，也需要通过QByteArray构造，如：
 * @param cds 基于QList<RibbonCustomizeData>生成的步骤
 * @return 如果出现异常，返回false,如果没有自定义数据也会返回false
 */
bool QxRibbonCustomizeDataSetToXml(QXmlStreamWriter *xml, const QList<RibbonCustomizeData> &cds);

QList<RibbonCustomizeData> QxRibbonCustomizeDataGetFromXml(QXmlStreamReader *xml, RibbonActionsManager *mgr);

/**
 * @brief 应用QList<RibbonCustomizeData>
 * @param cds
 * @param bar
 * @return 成功应用的个数
 */
int QxRibbonCustomizeDataApply(QList<RibbonCustomizeData> &cds, RibbonBar *bar);

QX_RIBBON_END_NAMESPACE
