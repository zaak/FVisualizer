#ifndef DERIVATIVEDIALOG_H
#define DERIVATIVEDIALOG_H

#include <QDialog>

namespace Ui {
    class DerivativeDialog;
}

class DerivativeDialog : public QDialog {
    
    Q_OBJECT
public:
    DerivativeDialog(QWidget *parent = 0);
    ~DerivativeDialog();
    QColor getDFColor();
    void setDFColor(QColor c);
    void setFunctionsStrings(QString& f, QString& df);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DerivativeDialog *ui;
};

#endif // DERIVATIVEDIALOG_H
