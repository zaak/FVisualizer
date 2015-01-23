#ifndef FUNCTION3DDIALOG_H
#define FUNCTION3DDIALOG_H

#include <QDialog>
#include "function3d.h"

namespace Ui {
    class Function3dDialog;
}

class Function3dDialog : public QDialog {
    Q_OBJECT
public:
    Function3dDialog(QWidget *parent = 0);
    ~Function3dDialog();

    void attachFunction(Function3d* f);
    void updateFunction();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Function3dDialog *ui;
    Function3d* function;
    int drawingMethod();

private slots:
    void checkFunction();
    void updateColorAlpha(int alpha);
    //    void intervalChecboxStateChanged(bool checked);
};

#endif // FUNCTION3DDIALOG_H
