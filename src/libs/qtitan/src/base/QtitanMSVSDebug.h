/****************************************************************************
**
** Qtitan Library by Developer Machines
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/

#ifndef QTITAN_MSVSDEBUG_H
#define QTITAN_MSVSDEBUG_H

#ifdef _MSC_VER
    #ifdef __AFX_H__
        #ifdef _DEBUG
        #define new DEBUG_NEW
        #undef THIS_FILE
        #define THIS_FILE __FILE__
        #endif
    #else // __AFX_H__
        #define _CRTDBG_MAP_ALLOC
        #include <stdlib.h>
        #include <crtdbg.h>
        #ifdef _DEBUG
        #define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
        #endif
    #endif // __AFX_H__
#endif // _MSC_VER

#endif //QTITAN_MSVSDEBUG_H

