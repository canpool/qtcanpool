/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockstatereader.h"

QX_DOCK_BEGIN_NAMESPACE

void DockStateReader::setFileVersion(int fileVersion)
{
	m_fileVersion = fileVersion;
}

int DockStateReader::fileVersion() const
{
	return m_fileVersion;
}

QX_DOCK_END_NAMESPACE
