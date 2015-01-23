#ifndef FVMAINWINDOW_H
#define FVMAINWINDOW_H

#include <QMainWindow>

class QMdiArea;
class QMdiSubWindow;
class QDockWidget;
class QTextEdit;
class GraphWindow;
class GraphWindow2d;
class GraphWindow3d;

/*********************************
 **
 ** Klasa głównego okna programu.
 **
 *********************************/

class FVMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    FVMainWindow();

private:
    void closeEvent(QCloseEvent *event);

    // Funkcje pomocnicze (ui)
    void createActions();
    void resetActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    // ---
    GraphWindow* addGraph(GraphWindow* graphWindow);

    GraphWindow* getActiveSubwindow();

    // Centralny obszar okna głównego
    QMdiArea *mdiArea;

    // Menu
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *graphMenu;
    QMenu *helpMenu;

    // Podmenu
    QMenu *newGraphMenu;

    // Toolbary
    QToolBar *stdToolBar;
    QToolBar *fv2dToolBar;
    QToolBar *fv3dToolBar;

	// Konsola
	QDockWidget *consoleDockWidget;
	QTextEdit *consoleEdit;

public:
    // Akcje
    QAction *new2dAct;
    QAction *new3dAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;

    QAction *undoAct;
    QAction *redoAct;
//    QAction *programSettingsAct;
    QAction *graphSettingsAct;

    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *zoomInSelectionAct;
    QAction *moveLeftAct;
    QAction *moveRightAct;
    QAction *moveUpAct;
    QAction *moveDownAct;
    QAction *moveNAct;
    QAction *moveSAct;
    QAction *showGraphListAct;
	QAction *showConsoleAct;

    QAction *graphAddAct;
    QAction *graphEditAct;
    QAction *graphDeleteAct;
    QAction *derivativeAct;
    QAction *rotateGraphAct;
    QAction *exportToImageAct;

    QAction *programHelpAct;
    QAction *aboutAct;

    // Sloty
private slots:
    GraphWindow2d* new2d();
    GraphWindow3d* new3d();
    void open();
    void save();
    void saveAs();
    void exit();

    void undo();
    void redo();
    void programSettings();

    void zoomIn();
    void zoomOut();
    void zoomInSelection();
    void zoomInSelectionFinished();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void moveN();
    void moveS();
    void showGraphList();
	void showConsole();

    void graphAdd();
    void graphEdit();
    void graphDelete();
    void graphSettings();
    void derivative();
    void rotateGraph();
    void exportToImage();

    void programHelp();
    void about();

    void graphWindowActivated(QMdiSubWindow *window);

};

#endif // FVMAINWINDOW_H
