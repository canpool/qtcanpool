#ifndef LICENSEDIALOG_H
#define LICENSEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class LicenseDialog :public QDialog
{
    Q_OBJECT

public:
    LicenseDialog(QWidget* parent = 0);
    ~LicenseDialog();

    void initUi();

public slots:
    void on_registerButton_clicked();
    void on_cancelButton_clicked();
    void on_open_email_url(const QString& url);

private:
    QLabel* hostLabel;
    QLabel* licenseLabel;
    QLineEdit* hostEdit;
    QLineEdit* licenseEdit;
    QLabel* emailLabel;
    QLabel* emailTo;
    QLabel* phoneLabel;
    QPushButton* registerButton;
    QPushButton* cancelButton;

};

#endif // LICENSEDIALOG_H
