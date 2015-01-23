#ifndef FUNCTION2DDIALOG_H
#define FUNCTION2DDIALOG_H

#include <QDialog>
#include "function2d.h"

namespace Ui {
    class Function2dDialog;
}

class Function2dDialog : public QDialog {
    Q_OBJECT
public:
    Function2dDialog(QWidget *parent = 0);
    ~Function2dDialog();

    void attachFunction(Function2d* f);
    void updateFunction();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Function2dDialog *ui;
    Function2d* function;

private slots:
    void checkFunction();
//    void intervalChecboxStateChanged(bool checked);
};

#endif // FUNCTION2DDIALOG_H
