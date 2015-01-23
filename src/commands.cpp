#include "commands.h"
#include "functionslistview.h"
#include <QVector>
#include <QStandardItemModel>
#include <qdebug.h>

// Dodaj funkcję

AddFunction2dCommand::AddFunction2dCommand(Function2d* f, QVector<Function2d*>& functionsList, FunctionsListView& functionsListView)
    :f(f), functionsList(functionsList), functionsListView(functionsListView)
{
    fi = Function2dInfo(f);
}

void AddFunction2dCommand::undo()
{
    int idx = functionsList.size()-1;
    delete functionsList.at(idx);
    f = NULL;
    functionsList.remove(idx);
    functionsListView.removeFunction(idx);
}

void AddFunction2dCommand::redo()
{
    if(f == NULL)
        f = new Function2d(fi);
    functionsList.append(f);
    functionsListView.addFunction(f->expression, f->color);
}

// Usuń funkcję
RemoveFunction2dCommand::RemoveFunction2dCommand(int index, QVector<Function2d*>& functionsList, FunctionsListView& functionsListView)
    :index(index), functionsList(functionsList), functionsListView(functionsListView)
{
    fi = Function2dInfo(functionsList.at(index));
}

void RemoveFunction2dCommand::undo()
{
    Function2d* f = new Function2d(fi);
    functionsList.insert(index, f);
    functionsListView.addFunction(index, f->expression, f->color, f->active);
}

void RemoveFunction2dCommand::redo()
{
    delete functionsList.at(index);
    functionsList.remove(index);
    functionsListView.removeFunction(index);
}

// ZoomIn / ZoomOut
ZoomPlotter2dCommand::ZoomPlotter2dCommand(Plotter2d &plotter2d, bool zoomOut)
    : plotter2d(plotter2d), zoomOut(zoomOut) {}

void ZoomPlotter2dCommand::undo()
{
    if(zoomOut)
        plotter2d.zoomIn();
    else
        plotter2d.zoomOut();
}

void ZoomPlotter2dCommand::redo()
{
    if(zoomOut)
        plotter2d.zoomOut();
    else
        plotter2d.zoomIn();
}


// Przesuń 1-góra, 2-prawo, 3-dół, 4-lewo
ScrollPlotter2dCommand::ScrollPlotter2dCommand(Plotter2d &plotter2d, int direction)
    : plotter2d(plotter2d), direction(direction) {}

void ScrollPlotter2dCommand::undo()
{
    switch(direction)
    {
        case PLOTTER2D_SCROLL_UP:
            plotter2d.scrollDown(); break;
        case PLOTTER2D_SCROLL_RIGHT:
            plotter2d.scrollLeft(); break;
        case PLOTTER2D_SCROLL_DOWN:
            plotter2d.scrollUp(); break;
        case PLOTTER2D_SCROLL_LEFT:
            plotter2d.scrollRight(); break;
    }
}

void ScrollPlotter2dCommand::redo()
{
    switch(direction)
    {
        case PLOTTER2D_SCROLL_UP:
            plotter2d.scrollUp(); break;
        case PLOTTER2D_SCROLL_RIGHT:
            plotter2d.scrollRight(); break;
        case PLOTTER2D_SCROLL_DOWN:
            plotter2d.scrollDown(); break;
        case PLOTTER2D_SCROLL_LEFT:
            plotter2d.scrollLeft(); break;
    }
}

// Powiększ zaznaczenie / edycja okna wykresu
ZoomSelectionPlotter2dCommand::ZoomSelectionPlotter2dCommand(Plotter2d& plotter2d, PlotSettings2d before)
    : plotter2d(plotter2d), before(before)
{
    after = plotter2d.getPlotsettings();
}

void ZoomSelectionPlotter2dCommand::undo()
{
    plotter2d.setPlotSettings(before);
}

void ZoomSelectionPlotter2dCommand::redo()
{
    plotter2d.setPlotSettings(after);
}


// Edycja funkcji 2d
EditFunction2dCommand::EditFunction2dCommand(int index, Function2dInfo& before,
    QVector<Function2d*>& functionsList, FunctionsListView& functionsListView)
        : index(index), before(before), functionsList(functionsList), functionsListView(functionsListView)
{
    after = Function2dInfo(functionsList.at(index));
}

void EditFunction2dCommand::undo()
{
    delete functionsList.at(index);
    functionsList[index] = new Function2d(before);
    functionsListView.updateFunctionAtIndex(index, before.expression, before.color, before.active);
}

void EditFunction2dCommand::redo()
{
    delete functionsList.at(index);
    functionsList[index] = new Function2d(after);
    functionsListView.updateFunctionAtIndex(index, after.expression, after.color, after.active);
}

// ---- 3d --------------------------------------------------


// Dodaj funkcję 3d

AddFunction3dCommand::AddFunction3dCommand(Function3d* f, QVector<Function3d*>& functionsList,
                                           FunctionsListView& functionsListView, Plotter3d& plotter)
    :f(f), functionsList(functionsList), functionsListView(functionsListView), plotter(plotter)
{
    fi = Function3dInfo(f);
}

void AddFunction3dCommand::undo()
{
    int idx = functionsList.size()-1;
    delete functionsList.at(idx);
    f = NULL;
    functionsList.remove(idx);
    functionsListView.removeFunction(idx);
}

void AddFunction3dCommand::redo()
{
    if(f == NULL)
        f = new Function3d(fi);
    functionsList.append(f);
    f->recalculate(plotter.getPlotsettings());
    functionsListView.addFunction(f->expression, f->color);
}


// Usuń funkcję 3d
RemoveFunction3dCommand::RemoveFunction3dCommand(int index, QVector<Function3d*>& functionsList,
                                                 FunctionsListView& functionsListView, Plotter3d& plotter)
    :index(index), functionsList(functionsList), functionsListView(functionsListView), plotter(plotter)
{
    fi = Function3dInfo(functionsList.at(index));
}

void RemoveFunction3dCommand::undo()
{
    Function3d* f = new Function3d(fi);
    f->recalculate(plotter.getPlotsettings());
    functionsList.insert(index, f);
    functionsListView.addFunction(index, f->expression, f->color, f->active);
}

void RemoveFunction3dCommand::redo()
{
    delete functionsList.at(index);
    functionsList.remove(index);
    functionsListView.removeFunction(index);
}

// ZoomIn / ZoomOut 3d
ZoomPlotter3dCommand::ZoomPlotter3dCommand(Plotter3d &plotter3d, bool zoomOut)
    : plotter3d(plotter3d), zoomOut(zoomOut) {}

void ZoomPlotter3dCommand::undo()
{
    if(zoomOut)
        plotter3d.zoomIn();
    else
        plotter3d.zoomOut();
}

void ZoomPlotter3dCommand::redo()
{
    if(zoomOut)
        plotter3d.zoomOut();
    else
        plotter3d.zoomIn();
}



// Przesuń 1-góra, 2-prawo, 3-dół, 4-lewo, 5-N, 6-S
ScrollPlotter3dCommand::ScrollPlotter3dCommand(Plotter3d &plotter3d, int direction)
    : plotter3d(plotter3d), direction(direction) {}

void ScrollPlotter3dCommand::undo()
{
    switch(direction)
    {
        case 1:
            plotter3d.scrollDown(); break;
        case 2:
            plotter3d.scrollLeft(); break;
        case 3:
            plotter3d.scrollUp(); break;
        case 4:
            plotter3d.scrollRight(); break;
        case 5:
            plotter3d.scrollS(); break;
        case 6:
            plotter3d.scrollN(); break;
    }
}

void ScrollPlotter3dCommand::redo()
{
    switch(direction)
    {
        case 1:
            plotter3d.scrollUp(); break;
        case 2:
            plotter3d.scrollRight(); break;
        case 3:
            plotter3d.scrollDown(); break;
        case 4:
            plotter3d.scrollLeft(); break;
        case 5:
            plotter3d.scrollN();; break;
        case 6:
            plotter3d.scrollS(); break;
    }
}


// Edycja funkcji 3d
EditFunction3dCommand::EditFunction3dCommand(int index, Function3dInfo& before,
    QVector<Function3d*>& functionsList, FunctionsListView& functionsListView, Plotter3d& plotter)
        : index(index), before(before), functionsList(functionsList), functionsListView(functionsListView), plotter(plotter)
{
    after = Function3dInfo(functionsList.at(index));
}

void EditFunction3dCommand::undo()
{
    delete functionsList.at(index);
    functionsList[index] = new Function3d(before);
    functionsList[index]->recalculate(plotter.getPlotsettings());
    functionsListView.updateFunctionAtIndex(index, before.expression, before.color, before.active);
}

void EditFunction3dCommand::redo()
{
    delete functionsList.at(index);
    functionsList[index] = new Function3d(after);
    functionsList[index]->recalculate(plotter.getPlotsettings());
    functionsListView.updateFunctionAtIndex(index, after.expression, after.color, after.active);
}

StateChangedFunction3dCommand::StateChangedFunction3dCommand
        (int index, QVector<Function3d *> &functionsList, FunctionsListView &functionsListView)
            : index(index), functionsList(functionsList), functionsListView(functionsListView)
{}

void StateChangedFunction3dCommand::redo()
{
    bool state = !functionsList[index]->active;
    functionsList[index]->active = state;
    functionsListView.updateFunctionAtIndex(
            index, functionsList[index]->expression, functionsList[index]->color, functionsList[index]->active);
}

void StateChangedFunction3dCommand::undo()
{
    redo();
}


PlotSettings3dChangedCommand::PlotSettings3dChangedCommand(Plotter3d &plotter3d, PlotSettings3d before)
    : plotter3d(plotter3d), before(before)
{
    after = plotter3d.getPlotsettings();
}

void PlotSettings3dChangedCommand::undo()
{
    plotter3d.setPlotSettings(before);
}

void PlotSettings3dChangedCommand::redo()
{
    plotter3d.setPlotSettings(after);
}
