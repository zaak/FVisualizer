#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include "function2d.h"
#include "plotter2d.h"
#include "function3d.h"
#include "plotter3d.h"

class FunctionsListView;

class AddFunction2dCommand : public QUndoCommand
{
public:
    AddFunction2dCommand(Function2d* f, QVector<Function2d*>& functionsList, FunctionsListView& functionsListView);
    void undo();
    void redo();

private:
    Function2d* f;
    Function2dInfo fi;
    QVector<Function2d*>& functionsList;
    FunctionsListView& functionsListView;
};

class RemoveFunction2dCommand : public QUndoCommand
{
public:
    RemoveFunction2dCommand(int index, QVector<Function2d*>& functionsList, FunctionsListView& functionsListView);
    void undo();
    void redo();

private:
    int index;
    Function2dInfo fi;
    QVector<Function2d*>& functionsList;
    FunctionsListView& functionsListView;
};

class ZoomPlotter2dCommand : public QUndoCommand
{
public:
    ZoomPlotter2dCommand(Plotter2d& plotter2d, bool zoomOut);
    void undo();
    void redo();

private:
    bool zoomOut;
    Plotter2d& plotter2d;
};


#define PLOTTER2D_SCROLL_UP     1
#define PLOTTER2D_SCROLL_RIGHT  2
#define PLOTTER2D_SCROLL_DOWN   3
#define PLOTTER2D_SCROLL_LEFT   4

class ScrollPlotter2dCommand : public QUndoCommand
{
public:
    ScrollPlotter2dCommand(Plotter2d& plotter2d, int direction);
    void undo();
    void redo();

private:
    int direction;
    Plotter2d& plotter2d;
};


class ZoomSelectionPlotter2dCommand : public QUndoCommand
{
public:
    ZoomSelectionPlotter2dCommand(Plotter2d& plotter2d, PlotSettings2d before);
    void undo();
    void redo();

private:
    PlotSettings2d before;
    PlotSettings2d after;
    Plotter2d& plotter2d;

};

class EditFunction2dCommand : public QUndoCommand
{
public:
    EditFunction2dCommand(int index, Function2dInfo& before,
        QVector<Function2d*>& functionsList, FunctionsListView& functionsListView);
    void undo();
    void redo();

private:
    int index;
    Function2dInfo before;
    Function2dInfo after;
    QVector<Function2d*>& functionsList;
    FunctionsListView& functionsListView;
};

// -- 3d -----------------------------------------------

class AddFunction3dCommand : public QUndoCommand
{
public:
    AddFunction3dCommand(Function3d* f, QVector<Function3d*>& functionsList, FunctionsListView& functionsListView, Plotter3d& plotter);
    void undo();
    void redo();

private:
    Function3d* f;
    Function3dInfo fi;
    QVector<Function3d*>& functionsList;
    FunctionsListView& functionsListView;
    Plotter3d& plotter;
};

class RemoveFunction3dCommand : public QUndoCommand
{
public:
    RemoveFunction3dCommand(int index, QVector<Function3d*>& functionsList, FunctionsListView& functionsListView, Plotter3d& plotter);
    void undo();
    void redo();

private:
    int index;
    Function3dInfo fi;
    QVector<Function3d*>& functionsList;
    FunctionsListView& functionsListView;
    Plotter3d& plotter;
};

class ZoomPlotter3dCommand : public QUndoCommand
{
public:
    ZoomPlotter3dCommand(Plotter3d& plotter3d, bool zoomOut);
    void undo();
    void redo();

private:
    bool zoomOut;
    Plotter3d& plotter3d;
};



class ScrollPlotter3dCommand : public QUndoCommand
{
public:
    ScrollPlotter3dCommand(Plotter3d& plotter3d, int direction);
    void undo();
    void redo();

private:
    int direction;
    Plotter3d& plotter3d;
};



class EditFunction3dCommand : public QUndoCommand
{
public:
    EditFunction3dCommand(int index, Function3dInfo& before,
        QVector<Function3d*>& functionsList, FunctionsListView& functionsListView, Plotter3d& plotter);
    void undo();
    void redo();

private:
    int index;
    Function3dInfo before;
    Function3dInfo after;
    QVector<Function3d*>& functionsList;
    FunctionsListView& functionsListView;
    Plotter3d& plotter;
};

class StateChangedFunction3dCommand : public QUndoCommand
{
public:
    StateChangedFunction3dCommand(int index, QVector<Function3d*>& functionsList, FunctionsListView& functionsListView);
    void undo();
    void redo();

private:
    int index;
    QVector<Function3d*>& functionsList;
    FunctionsListView& functionsListView;
};

class PlotSettings3dChangedCommand : public QUndoCommand
{
public:
    PlotSettings3dChangedCommand(Plotter3d& plotter3d, PlotSettings3d before);
    void undo();
    void redo();

private:
    PlotSettings3d before;
    PlotSettings3d after;
    Plotter3d& plotter3d;

};

#endif // COMMANDS_H
