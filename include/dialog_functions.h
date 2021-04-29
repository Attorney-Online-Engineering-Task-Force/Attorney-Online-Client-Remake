#ifndef DIALOG_FUNCTIONS_H
#define DIALOG_FUNCTIONS_H

#include <QMessageBox>
#include <QString>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QPushButton>

void call_error(QString message);
void call_notice(QString message);

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(bool username_enabled, QWidget *parent = 0);

private:
    bool enable_username;
    QLabel *labelUsername;
    QLineEdit *editUsername;
    QLabel *labelPassword;
    QLineEdit *editPassword;
    QDialogButtonBox *buttons;
    void setupUi();

signals:
    void sendLogin(QString& username, QString& password);

public slots:
    void slotSendLogin();
};


#endif // DIALOG_FUNCTIONS_H
