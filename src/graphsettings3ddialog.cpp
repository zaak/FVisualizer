#include "graphsettings3ddialog.h"
#include "ui_graphsettings3ddialog.h"
#include <QMessageBox>

GraphSettings3dDialog::GraphSettings3dDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
    ui(new Ui::GraphSettings3dDialog)
{
    ui->setupUi(this);

    connect(ui->showEdges, SIGNAL(stateChanged(int)), this, SLOT(updateColorButtonsActivity()));
    connect(ui->showLabels, SIGNAL(stateChanged(int)), this, SLOT(updateColorButtonsActivity()));
}

GraphSettings3dDialog::~GraphSettings3dDialog()
{
    delete ui;
}

void GraphSettings3dDialog::changeEvent(QEvent *e)
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

void GraphSettings3dDialog::updateColorButtonsActivity()
{
    ui->edgesColorBtn->setEnabled(ui->showEdges->isChecked());
    ui->labelsColorBtn->setEnabled(ui->showLabels->isChecked());
}


void GraphSettings3dDialog::setPlotSettings3d(const PlotSettings3d& p)
{
    plotSettings = p;

    ui->showEdges->setChecked(p.showEdges);
    ui->edgesColorBtn->setColor(p.edgesColor);
    ui->showLabels->setChecked(p.showLabels);
    ui->labelsColorBtn->setColor(p.labelsColor);
    ui->bgColorBtn->setColor(p.bgColor);

    ui->rotateACW->setChecked(p.acwRotation);

    ui->intervalXFrom->setText(QString::number(p.minX));
    ui->intervalXTo->setText(QString::number(p.maxX));
    ui->intervalYFrom->setText(QString::number(p.minY));
    ui->intervalYTo->setText(QString::number(p.maxY));
    ui->intervalZFrom->setText(QString::number(p.minZ));
    ui->intervalZTo->setText(QString::number(p.maxZ));


    ui->showCSMiniature->setChecked(p.showCSMiniature);
    ui->showDirectionsMarkers->setChecked(p.showDirectionsMarkers);
    ui->showLegend->setChecked(p.showLegend);

    updateColorButtonsActivity();
}

PlotSettings3d& GraphSettings3dDialog::getPlotSettings3d()
{
    plotSettings.showEdges = ui->showEdges->isChecked();
    if(plotSettings.showEdges) plotSettings.edgesColor = ui->edgesColorBtn->color();
    plotSettings.showLabels = ui->showLabels->isChecked();
    if(plotSettings.showLabels) plotSettings.labelsColor = ui->labelsColorBtn->color();

    plotSettings.bgColor = ui->bgColorBtn->color();

    plotSettings.acwRotation = ui->rotateACW->isChecked();

    if(ui->drawInInterval)
    {
        plotSettings.minX = ui->intervalXFrom->text().toDouble();
        plotSettings.maxX = ui->intervalXTo->text().toDouble();
        plotSettings.minY = ui->intervalYFrom->text().toDouble();
        plotSettings.maxY = ui->intervalYTo->text().toDouble();
        plotSettings.minZ = ui->intervalZFrom->text().toDouble();
        plotSettings.maxZ = ui->intervalZTo->text().toDouble();
    }

    plotSettings.showCSMiniature = ui->showCSMiniature->isChecked();
    plotSettings.showDirectionsMarkers = ui->showDirectionsMarkers->isChecked();
    plotSettings.showLegend = ui->showLegend->isChecked();

    return plotSettings;
}

void GraphSettings3dDialog::checkSettings()
{
    if(ui->intervalXFrom->text().toDouble() >= ui->intervalXTo->text().toDouble() ||
       ui->intervalYFrom->text().toDouble() >= ui->intervalYTo->text().toDouble() ||
       ui->intervalZFrom->text().toDouble() >= ui->intervalZTo->text().toDouble())
    {
        QMessageBox::critical(this, "Błąd", "Wprowadzono nieprawidłowe granice przedziału kreślenia.\nDla każdej  osi wartość dolnej granicy musi być niższa od wartości górnej granicy.");
        return;
    }

    emit accept();
}
