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
 * @brief Converts to xml
 *
 * This function only writes elements and does not write document-related content.
 * Therefore, to write a document, call QXmlStreamWriter::writeStartDocument() before this
 * function and QXmlStreamWriter::writeEndDocument() after it.
 * @param xml QXmlStreamWriter pointer
 * @note Before passing in the QXmlStreamWriter, set the encoding to utf-8: xml->setCodec("utf-8");
 * @note Since QXmlStreamWriter does not support encoding when a QString is used as the IO device,
 * and the customization process cannot be guaranteed to be free of Chinese characters,
 * QXmlStreamWriter should not be constructed from a QString; if a string is needed, construct it from a QByteArray,
 * e.g.:
 * @param cds The steps generated from QList<RibbonCustomizeData>
 * @return Returns false on exception; also returns false when there is no customization data
 */
bool QxRibbonCustomizeDataSetToXml(QXmlStreamWriter *xml, const QList<RibbonCustomizeData> &cds);

QList<RibbonCustomizeData> QxRibbonCustomizeDataGetFromXml(QXmlStreamReader *xml, RibbonActionsManager *mgr);

/**
 * @brief Applies QList<RibbonCustomizeData>
 * @param cds
 * @param bar
 * @return The number of successfully applied entries
 */
int QxRibbonCustomizeDataApply(QList<RibbonCustomizeData> &cds, RibbonBar *bar);

QX_RIBBON_END_NAMESPACE
