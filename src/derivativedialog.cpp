#include "derivativedialog.h"
#include "ui_derivativedialog.h"

DerivativeDialog::DerivativeDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
    ui(new Ui::DerivativeDialog)
{
    ui->setupUi(this);
}

DerivativeDialog::~DerivativeDialog()
{
    delete ui;
}

void DerivativeDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QColor DerivativeDialog::getDFColor()
{
    return ui->graphColorBtn->color();
}


void DerivativeDialog::setDFColor(QColor c)
{
    ui->graphColorBtn->setColor(c);
}

void DerivativeDialog::setFunctionsStrings(QString &f, QString &df)
{
    ui->fstring->setText(f);
    ui->dfstring->setText(df);
}
