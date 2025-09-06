#ifndef ADDNEWAPPDIALOG_H
#define ADDNEWAPPDIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class AddNewAppDialog;
}

class AddNewAppDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewAppDialog(QWidget *parent = nullptr);

    explicit AddNewAppDialog(QString _appName = "", QString _uuid = "", QString _dstport = "", QString _srcport = "", QString _protocol = "", QWidget *parent = nullptr);
    ~AddNewAppDialog();

signals:
    void dataReturned(const QString &appName, const QString &uuid, const int &dstport, const int &srcport, const QString &protocol);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::AddNewAppDialog *ui;
};

#endif // ADDNEWAPPDIALOG_H
