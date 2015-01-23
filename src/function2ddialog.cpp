#include "function2ddialog.h"
#include "ui_function2ddialog.h"
#include <QDebug>
#include <QMessageBox>
#include "plotter2d.h"

Function2dDialog::Function2dDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
    ui(new Ui::Function2dDialog),
    function(NULL)
{
    ui->setupUi(this);

    QDoubleValidator* validator = new QDoubleValidator(this);
    ui->intervalFrom->setValidator(validator);
    ui->intervalTo->setValidator(validator);

    connect(ui->okBtn, SIGNAL(clicked()), this, SLOT(checkFunction()));
//    connect(ui->drawInIntervalGroupBox, SIGNAL(clicked(bool)), this, SLOT(intervalChecboxStateChanged(bool)));

    ui->fstring->setFocus();
}

Function2dDialog::~Function2dDialog()
{
    delete ui;
}

void Function2dDialog::attachFunction(Function2d* f)
{
    if(f == NULL) {
        qDebug() << "Function2dDialog, attaching NULL pointer function";
        return;
    }
    function = f;
    ui->fstring->setText(f->expression);
    ui->colorBtn->setColor(f->color);

    ui->intervalFrom->setText(QString::number(f->intervalFrom));
    ui->intervalTo->setText(QString::number(f->intervalTo));

    if(f->drawInInterval) {
        ui->drawInIntervalGroupBox->setChecked(true);
//        ui->intervalFrom->setEnabled(true);
//        ui->intervalTo->setEnabled(true);
    }
}

void Function2dDialog::updateFunction()
{
    if(function == NULL) {
        qDebug() << "Function2dDialog::updateFunction(), updating NULL pointer function";
        return;
    }

//    function->expression = ui->fstring->text();
//    function->evaluator.setExpression(function->expression.toStdString());
    function->setExpression(ui->fstring->text());
    function->color = ui->colorBtn->color();
    function->drawInInterval = ui->drawInIntervalGroupBox->isChecked();
    function->intervalFrom = ui->intervalFrom->text().toDouble();
    function->intervalTo = ui->intervalTo->text().toDouble();
}

void Function2dDialog::changeEvent(QEvent *e)
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

void Function2dDialog::checkFunction()
{

    if(ui->drawInIntervalGroupBox->isChecked() &&
       ui->intervalFrom->text().toDouble() > ui->intervalTo->text().toDouble()) {
        QMessageBox::critical(this, "Błąd", "Wprowadzono nieprawidłowe granice przedziału kreślenia.\nWartość dolnej granicy musi być niższa od wartości górnej granicy.");
        return;
    }

    try {
        Function2d testFunction(ui->fstring->text());
        //testFunction.evaluator.setExpression(.toStdString());
        testFunction.recalculate(PlotSettings2d());

    }
    catch (ParsingException)
    {
        QMessageBox::critical(this, "Błąd wyrażenia", "Wprowadzone wyrażenie zwiera błędy");
        return;
    };

    updateFunction();
    emit accept();
}

//void Function2dDialog::intervalChecboxStateChanged(bool checked)
//{
//    qDebug() << "intervalChecboxState: " << checked;
//
//    ui->intervalFrom->setEnabled(checked);
//    ui->intervalTo->setEnabled(checked);
//}
