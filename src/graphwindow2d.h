#ifndef GRAPHWINDOW2D_H
#define GRAPHWINDOW2D_H

#include "graphwindow.h"
#include "plotter2d.h"
#include "commands.h"

class Function2d;
class GraphWindow2dSnapshot;

class GraphWindow2d : public GraphWindow
{
    Q_OBJECT

private:
    Plotter2d *plotter2d;

    // Lista funkcji
    QVector<Function2d*> functionsList;


//    // Stos stanów dla powtórz / cofnij
//    QVector<GraphWindow2dSnapshot> urStack;/* undoRedoStack;*/
//    int uStackIndex;
//    int rStackIndex;
//
//    void updateFromSnapshot(int index);
    PlotSettings2d beforeSelection;

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

    virtual void graphAdd();
    virtual void graphEdit();
    virtual void graphDelete();

    virtual void undo();
    virtual void redo();

    virtual void save(bool saveAs = false);

    virtual void exportToImage();

    virtual void graphSettings();

    virtual void updateActions();

    void zoomSelectionStart();
    void zoomSelectionCancel();
    void zoomSelectionFinished();

    void derivative();

    void functionOnListViewChanged(QStandardItem* item);

//    void updateURStack();

public:
    GraphWindow2d(QWidget *parentMdi, FVMainWindow *mainWindow);
    ~GraphWindow2d();

    void addFunction(Function2d* f);
    void addFunction(Function2dInfo& fi);

    void loadFunctions(QVector<Function2dInfo>& v, PlotSettings2d& p, QString filepath);
    void newGraph();

private slots:
    void addLineEditFunction();
};

class GraphWindow2dSnapshot
{
public:
    PlotSettings2d plotSettings;
    QVector<Function2dInfo> functionsInfoList;
};


#endif // GRAPHWINDOW2D_H
