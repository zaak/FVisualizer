#ifndef GRAPHSETTINGS2DDIALOG_H
#define GRAPHSETTINGS2DDIALOG_H

#include <QDialog>
#include "plotter2d.h"

namespace Ui {
    class GraphSettings2dDialog;
}

class GraphSettings2dDialog : public QDialog {
    Q_OBJECT
public:
    GraphSettings2dDialog(QWidget *parent = 0);
    ~GraphSettings2dDialog();
    void setPlotSettings2d(const PlotSettings2d& p);
    PlotSettings2d& getPlotSettings2d();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GraphSettings2dDialog *ui;
    PlotSettings2d plotSettings;

private slots:
    void checkSettings();
    void updateColorButtonsActivity();
};

#endif // GRAPHSETTINGS2DDIALOG_H
