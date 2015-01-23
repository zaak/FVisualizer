#ifndef GRAPHWINDOW3D_H
#define GRAPHWINDOW3D_H

#include "graphwindow.h"
#include "function3d.h"
#include "plotter3d.h"

class GraphWindow3d : public GraphWindow
{
    Q_OBJECT
public:
    GraphWindow3d(QWidget *parentMdi, FVMainWindow *mainWindow);
    ~GraphWindow3d();

    QVector<Function3d*> functionsList;
	QVector<Ray> raysList;

    void addFunction(Function3d* f);
    void addFunction(Function3dInfo& fi);

    void loadFunctions(QVector<Function3dInfo>& v, PlotSettings3d& p, QString& filepath);
    void newGraph();

private:
    Plotter3d* plotter3d;

protected:
    void closeEvent(QCloseEvent *event);
    bool maybeSave();

public slots:
    virtual void zoomIn();
    virtual void zoomOut();
    virtual void scrollLeft();
    virtual void scrollRight();
    virtual void scrollUp();
    virtual void scrollDown();
    void scrollN();
    void scrollS();

    virtual void undo();
    virtual void redo();

    virtual void graphAdd();
    virtual void graphEdit();
    virtual void graphDelete();

    virtual void save(bool saveAs = false);

    virtual void exportToImage();

    virtual void graphSettings();

    virtual void updateActions();

    void startRotation();
    void stopRotation();

private slots:
    void addLineEditFunction();

    void functionOnListViewChanged(QStandardItem* item);

};

#endif // GRAPHWINDOW3D_H
