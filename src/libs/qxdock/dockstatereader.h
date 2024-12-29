/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QXmlStreamReader>

QX_DOCK_BEGIN_NAMESPACE

/**
 * Extends QXmlStreamReader with file version information
 */
class DockStateReader : public QXmlStreamReader
{
public:
	using QXmlStreamReader::QXmlStreamReader;

	void setFileVersion(int fileVersion);
	int fileVersion() const;

private:
	int m_fileVersion;
};

QX_DOCK_END_NAMESPACE
