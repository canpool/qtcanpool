/****************************************************************************
**
** Qtitan Library by Developer Machines
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
** Copyright (c) 2019 MaMinJie <canpool@163.com>
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


#ifndef QTITAN_DEF_H
#define QTITAN_DEF_H

#include <QtCore/QProcess>

#define QTITAN_NAMESPACE Qtitan

#if !defined(QTITAN_NAMESPACE) /*|| defined(Q_MOC_RUN)*/
#  define QTITAN_BEGIN_NAMESPACE
#  define QTITAN_END_NAMESPACE
#  define QTITAN_USE_NAMESPACE
#  define QT_PREPEND_NAMESPACE_QTITAN(name) ::name
#else
#  define QTITAN_BEGIN_NAMESPACE namespace QTITAN_NAMESPACE {
#  define QTITAN_END_NAMESPACE }
#  define QTITAN_USE_NAMESPACE using namespace QTITAN_NAMESPACE;
#  define QT_PREPEND_NAMESPACE_QTITAN(name) ::QTITAN_NAMESPACE::name
#endif


QTITAN_BEGIN_NAMESPACE

#ifndef QTITAN_LIBRARY_STATIC
#ifdef QTITAN_LIBRARY
#define QTITAN_EXPORT Q_DECL_EXPORT
#else
#define QTITAN_EXPORT Q_DECL_IMPORT
#endif
#else
#define QTITAN_EXPORT
#endif

#define QTN_QUOTE_(x) #x
#define QTN_QUOTE(x) QTN_QUOTE_(x)

#ifdef Q_COMPILER_NULLPTR
#define Q_NULL    nullptr
#else
#define Q_NULL    NULL
#endif
#define Q_MAXINT INT_MAX
#define Q_DELETE_AND_NULL(x) delete x; x = Q_NULL;

#ifndef Q_DECL_NOTHROW
#define Q_DECL_NOTHROW throw()
#endif

#define QTN_DECLARE_PRIVATE(Class) friend class Class##Private; \
Class##Private *qtn_d_ptr; \
Class##Private& qtn_d() { return *qtn_d_ptr; } \
const Class##Private& qtn_d() const { return *qtn_d_ptr; }

#define QTN_DECLARE_PUBLIC(Class) \
friend class Class; \
Class *qtn_p_ptr; \
inline void setPublic(Class *ptr) { qtn_p_ptr = ptr; } \
Class& qtn_p() { return *qtn_p_ptr; } \
const Class& qtn_p() const { return *qtn_p_ptr; }
#define QTN_INIT_PRIVATE(Class) \
qtn_d_ptr = new Class##Private(); qtn_d_ptr->setPublic(this);
#define QTN_D(Class) Class##Private& d = qtn_d();
#define QTN_P(Class) Class& p = qtn_p();
#define QTN_FINI_PRIVATE() \
delete qtn_d_ptr; qtn_d_ptr = Q_NULL;

#define QTN_DECLARE_EX_PRIVATE(Class) friend class Class##Private; \
Class##Private& qtn_d() { return reinterpret_cast<Class##Private &>(*qtn_d_ptr); } \
const Class##Private& qtn_d() const { return reinterpret_cast<const Class##Private &>(*qtn_d_ptr); }

#define QTN_DECLARE_EX_PUBLIC(Class) \
friend class Class; \
Class& qtn_p() { return static_cast<Class &>(*qtn_p_ptr); } \
const Class& qtn_p() const { return static_cast<Class &>(*qtn_p_ptr); }

/* BaseEventArgs */
class QTITAN_EXPORT BaseEventArgs : public QObject
{
public:
    BaseEventArgs() : m_handled(false) {}
    bool isHandled() const { return m_handled; }
    void setHandled(bool handled) { m_handled = handled; }
private:
    bool m_handled;
};

inline QString getSystemEnvironmentVariable(const QString& key)
{
    QStringList all = QProcess::systemEnvironment();
    for (QStringList::const_iterator it = all.constBegin(); it != all.constEnd(); ++it)
    {
        QStringList pair = it->split(QLatin1String("="));
        if (pair.size() != 2)
            continue;
        if (key.compare(pair[0], Qt::CaseInsensitive) == 0)
            return pair[1];
    }
    return QString();
}

inline QString getEnvironmentVariable(QProcess* process, const QString& key)
{
    QStringList all = process->environment();
    for (QStringList::const_iterator it = all.constBegin(); it != all.constEnd(); ++it)
    {
        QStringList pair = it->split(QLatin1String("="));
        if (pair.size() != 2)
            continue;
        if (key.compare(pair[0], Qt::CaseInsensitive) == 0)
            return pair[1];
    }
    return QString();
}

inline void setEnvironmentVariable(QProcess* process, const QString& key, const QString& value)
{
    QStringList all = process->environment();
    QStringList::iterator it = all.begin();
    for (; it != all.end(); ++it)
    {
        QStringList pair = it->split(QLatin1String("="));
        if (pair.size() != 2)
            continue;
        if (key.compare(pair[0], Qt::CaseInsensitive) == 0)
            break;
    }
    if (it != all.end())
        *it = QString::fromLatin1("%1=%2").arg(key).arg(value);
    else
        all.push_back(QString::fromLatin1("%1=%2").arg(key).arg(value));
    process->setEnvironment(all);
}

QTITAN_END_NAMESPACE

#endif //QTITAN_DEF_H

