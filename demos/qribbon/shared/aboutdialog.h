#ifndef QTC_ABOUTDIALOG_H
#define QTC_ABOUTDIALOG_H

#include <QMessageBox>

namespace QRibbon
{
class AboutDialog : public QMessageBox
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = nullptr);
    AboutDialog(Icon icon, const QString &title, const QString &text, StandardButtons buttons = NoButton,
                QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    virtual ~AboutDialog();
public:
    static void show(QWidget *parent, const QString &title, const QString &component, const QString &version);
private:
    Q_DISABLE_COPY(AboutDialog)
};

};   // namespace QRibbon

#endif   // QTC_ABOUTDIALOG_H
