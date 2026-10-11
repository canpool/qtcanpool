/*!
 * \file
 *
 * The other half of the QX_<LIB>_DEPRECATED_SINCE gate: raising
 * QX_<LIB>_DISABLE_DEPRECATED_BEFORE is how a caller drops an API that is only
 * marked deprecated so far, instead of waiting for the release that removes it.
 *
 * The cutoff has to be defined before the header is included - the header reads
 * it once and then never again - so this case cannot live in the same
 * translation unit as the default behaviour, which is what
 * tst_qxdeprecated.cpp pins.
 */
#define QX_RIBBON_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(0, 11, 0)

#include "tst_global.h"

#include "qxribbon/qxribbon_global.h"
#include "qxwindow/qxwindow_global.h"

class tst_QxDeprecatedCutoff : public QObject
{
    Q_OBJECT

private slots:
    void raisedCutoffDropsTheApi();
    void raisingOneLibraryLeavesTheOthersAlone();
};

void tst_QxDeprecatedCutoff::raisedCutoffDropsTheApi()
{
    // Deprecated at the cutoff or before it: gone. After it: still there.
    QVERIFY(!QX_RIBBON_DEPRECATED_SINCE(0, 10));
    QVERIFY(!QX_RIBBON_DEPRECATED_SINCE(0, 11));
    QVERIFY(QX_RIBBON_DEPRECATED_SINCE(0, 12));
}

void tst_QxDeprecatedCutoff::raisingOneLibraryLeavesTheOthersAlone()
{
    // qxwindow was not given a cutoff in this translation unit, so it keeps the
    // default: raising one library's cutoff must not reach into another.
    QVERIFY(QX_WINDOW_DEPRECATED_SINCE(0, 2));
}

TEST_ADD(tst_QxDeprecatedCutoff)

#include "tst_qxdeprecated_cutoff.moc"
