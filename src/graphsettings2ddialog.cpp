#include "graphsettings2ddialog.h"
#include "ui_graphsettings2ddialog.h"
#include <QMessageBox>

GraphSettings2dDialog::GraphSettings2dDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
    ui(new Ui::GraphSettings2dDialog)
{
    ui->setupUi(this);

    connect(ui->showAxes, SIGNAL(stateChanged(int)), this, SLOT(updateColorButtonsActivity()));
    connect(ui->showLabels, SIGNAL(stateChanged(int)), this, SLOT(updateColorButtonsActivity()));
    connect(ui->showGrid, SIGNAL(stateChanged(int)), this, SLOT(updateColorButtonsActivity()));

    QDoubleValidator* validator = new QDoubleValidator(this);
    ui->intervalXFrom->setValidator(validator);
    ui->intervalXTo->setValidator(validator);
    ui->intervalYFrom->setValidator(validator);
    ui->intervalYTo->setValidator(validator);
}

GraphSettings2dDialog::~GraphSettings2dDialog()
{
    delete ui;
}

void GraphSettings2dDialog::changeEvent(QEvent *e)
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

void GraphSettings2dDialog::updateColorButtonsActivity()
{
    ui->axesColorBtn->setEnabled(ui->showAxes->isChecked());
    ui->labelsColorBtn->setEnabled(ui->showLabels->isChecked());
    ui->gridColorBtn->setEnabled(ui->showGrid->isChecked());
}

void GraphSettings2dDialog::setPlotSettings2d(const PlotSettings2d& p)
{
    plotSettings = p;

    ui->showAxes->setChecked(p.showAxes);
    ui->axesColorBtn->setColor(p.axesColor);
    ui->showLabels->setChecked(p.showLabels);
    ui->labelsColorBtn->setColor(p.labelsColor);
    ui->showGrid->setChecked(p.showGrid);
    ui->gridColorBtn->setColor(p.gridColor);

    ui->intervalXFrom->setText(QString::number(p.minX));
    ui->intervalXTo->setText(QString::number(p.maxX));
    ui->intervalYFrom->setText(QString::number(p.minY));
    ui->intervalYTo->setText(QString::number(p.maxY));

    ui->bgColorBtn->setColor(p.bgColor);

    ui->showLegend->setChecked(p.showLegend);
    ui->enableAntialiasing->setChecked(p.enableAntialiasing);

    updateColorButtonsActivity();
}

PlotSettings2d& GraphSettings2dDialog::getPlotSettings2d()
{
    plotSettings.showAxes = ui->showAxes->isChecked();
    if(plotSettings.showAxes) plotSettings.axesColor = ui->axesColorBtn->color();
    plotSettings.showLabels = ui->showLabels->isChecked();
    if(plotSettings.showLabels) plotSettings.labelsColor = ui->labelsColorBtn->color();
    plotSettings.showGrid = ui->showGrid->isChecked();
    if(plotSettings.showGrid) plotSettings.gridColor = ui->gridColorBtn->color();

    plotSettings.bgColor = ui->bgColorBtn->color();

    if(ui->drawInInterval)
    {
        plotSettings.minX = ui->intervalXFrom->text().toDouble();
        plotSettings.maxX = ui->intervalXTo->text().toDouble();
        plotSettings.minY = ui->intervalYFrom->text().toDouble();
        plotSettings.maxY = ui->intervalYTo->text().toDouble();
    }

    plotSettings.showLegend = ui->showLegend->isChecked();

    plotSettings.enableAntialiasing = ui->enableAntialiasing->isChecked();

    return plotSettings;
}

void GraphSettings2dDialog::checkSettings()
{
    if(ui->intervalXFrom->text().toDouble() >= ui->intervalXTo->text().toDouble() ||
       ui->intervalYFrom->text().toDouble() >= ui->intervalYTo->text().toDouble())
    {
        QMessageBox::critical(this, "Błąd", "Wprowadzono nieprawidłowe granice przedziału kreślenia.\nDla każdej  osi wartość dolnej granicy musi być niższa od wartości górnej granicy.");
        return;
    }

    emit accept();
}
