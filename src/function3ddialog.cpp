#include "function3ddialog.h"
#include "ui_function3ddialog.h"
#include <qdebug.h>
#include <QMessageBox>
#include "plotter3d.h"

Function3dDialog::Function3dDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
    ui(new Ui::Function3dDialog)
{
    ui->setupUi(this);
    
    QDoubleValidator* validator = new QDoubleValidator(this);
    ui->intervalXFrom->setValidator(validator);
    ui->intervalXTo->setValidator(validator);
    ui->intervalYFrom->setValidator(validator);
    ui->intervalYTo->setValidator(validator);
//

    connect(ui->okBtn, SIGNAL(clicked()), this, SLOT(checkFunction()));
    connect(ui->alphaSlider, SIGNAL(valueChanged(int)), this, SLOT(updateColorAlpha(int)));
//    connect(ui->drawInIntervalGroupBox, SIGNAL(clicked(bool)), this, SLOT(intervalChecboxStateChanged(bool)));
//
//    ui->fstring->setFocus();
}

Function3dDialog::~Function3dDialog()
{
    delete ui;
}

void Function3dDialog::changeEvent(QEvent *e)
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

void Function3dDialog::attachFunction(Function3d* f)
{
    if(f == NULL) {
        qDebug() << "Function3dDialog, attaching NULL pointer function";
        return;
    }
    function = f;
    ui->fstring->setText(f->expression);
    ui->colorBtn->setColor(f->color);

    ui->intervalXFrom->setText(QString::number(f->intervalXFrom));
    ui->intervalXTo->setText(QString::number(f->intervalXTo));
    ui->intervalYFrom->setText(QString::number(f->intervalYFrom));
    ui->intervalYTo->setText(QString::number(f->intervalYTo));

    if(f->drawInInterval) {
        ui->drawInIntervalGroupBox->setChecked(true);
//        ui->intervalXFrom->setEnabled(true);
//        ui->intervalXTo->setEnabled(true);
//        ui->intervalYFrom->setEnabled(true);
//        ui->intervalYTo->setEnabled(true);
    }

    switch(f->drawingMethod)
    {
        case GL_QUADS:
            ui->quadsRadioBtn->setChecked(true); break;
        case GL_TRIANGLES:
            ui->trianglesRadioBtn->setChecked(true); break;
        case GL_LINES:
            ui->linesRadioBtn->setChecked(true); break;
        case GL_POINTS:
            ui->pointsRadioBtn->setChecked(true); break;
    }

    ui->alphaSlider->setValue(255 - f->color.alpha());

    ui->qualitySlider->setValue(f->pointsXCount);
}

void Function3dDialog::checkFunction()
{

    if(ui->drawInIntervalGroupBox->isChecked() &&
       (ui->intervalXFrom->text().toDouble() > ui->intervalXTo->text().toDouble() ||
        ui->intervalYFrom->text().toDouble() > ui->intervalYTo->text().toDouble())) {
        QMessageBox::critical(this, "Błąd", "Wprowadzono nieprawidłowe granice przedziału kreślenia.\nWartość dolnej granicy musi być niższa od wartości górnej granicy.");
        return;
    }

    try {
        Function3d testFunction(ui->fstring->text());
        //testFunction.evaluator.setExpression(.toStdString());
        testFunction.recalculate(PlotSettings3d());
    }
    catch (ParsingException)
    {
        QMessageBox::critical(this, "Błąd wyrażenia", "Wprowadzone wyrażenie zwiera błędy");
        return;
    };

    updateFunction();
    emit accept();
}

void Function3dDialog::updateFunction()
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
    function->intervalXFrom = ui->intervalXFrom->text().toDouble();
    function->intervalXTo = ui->intervalXTo->text().toDouble();
    function->intervalYFrom = ui->intervalYFrom->text().toDouble();
    function->intervalYTo = ui->intervalYTo->text().toDouble();
    function->drawingMethod = drawingMethod();

    if(function->pointsXCount != ui->qualitySlider->value())
    {
        for(int i = 0; i < function->pointsXCount; ++i)
            delete[] function->curvatureData[i];
        delete[] function->curvatureData;

        function->pointsXCount = function->pointsYCount = ui->qualitySlider->value();

        function->curvatureData = new QVector3D*[function->pointsXCount];
        for(int i = 0; i < function->pointsXCount; ++i)
            function->curvatureData[i] = new QVector3D[function->pointsYCount];
    }
}

//void Function3dDialog::intervalChecboxStateChanged(bool checked)
//{
//    qDebug() << "intervalChecboxState: " << checked;
//
//    ui->intervalXFrom->setEnabled(checked);
//    ui->intervalXTo->setEnabled(checked);
//    ui->intervalYFrom->setEnabled(checked);
//    ui->intervalYTo->setEnabled(checked);
//}

int Function3dDialog::drawingMethod()
{
    if(ui->quadsRadioBtn->isChecked()) return GL_QUADS;
    if(ui->trianglesRadioBtn->isChecked()) return GL_TRIANGLES;
    if(ui->linesRadioBtn->isChecked()) return GL_LINES;
    if(ui->pointsRadioBtn->isChecked()) return GL_POINTS;
    return GL_QUADS;
}

void Function3dDialog::updateColorAlpha(int alpha)
{
    QColor c = ui->colorBtn->color();
    c.setAlpha(255 - alpha);
    ui->colorBtn->setColor(c);
}
