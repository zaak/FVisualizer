#ifndef GRAPHSETTINGS3DDIALOG_H
#define GRAPHSETTINGS3DDIALOG_H

#include <QDialog>
#include "plotter3d.h"

namespace Ui {
    class GraphSettings3dDialog;
}

class GraphSettings3dDialog : public QDialog {
    Q_OBJECT
public:
    GraphSettings3dDialog(QWidget *parent = 0);
    ~GraphSettings3dDialog();
    void setPlotSettings3d(const PlotSettings3d& p);
    PlotSettings3d& getPlotSettings3d();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GraphSettings3dDialog *ui;
    PlotSettings3d plotSettings;

private slots:
    void checkSettings();
    void updateColorButtonsActivity();
};

#endif // GRAPHSETTINGS3DDIALOG_H
