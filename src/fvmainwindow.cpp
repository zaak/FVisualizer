#include "fvmainwindow.h"

#include <QtGui>
#include "graphwindow2d.h"
#include "graphwindow3d.h"
#include "fvformathandler.h"
#include "helpdialog.h"

FVMainWindow::FVMainWindow()
{
    // Wyśrodkowanie i dobranie rozmiaru głównego okna
    QRect desktopRect = QDesktopWidget().availableGeometry();
    qDebug() << "Starting app\nDesktop size: " << desktopRect.width() << "x" << desktopRect.height();

    QRect appWindowRect;
    int appWinWidth = desktopRect.width() * 0.8;
    int appWinHeight = desktopRect.height() * 0.8;
    appWindowRect.setLeft((desktopRect.width() - appWinWidth) / 2);
    appWindowRect.setTop((desktopRect.height() - appWinHeight) / 2);
    appWindowRect.setWidth(appWinWidth);
    appWindowRect.setHeight(appWinHeight);

    qDebug() << "Calculated application window size: " << appWindowRect;

    setGeometry(appWindowRect);

    move(appWindowRect.left(), appWindowRect.top());

    setWindowTitle("FVisualizer");
    setWindowIcon(QIcon(":/images/fviconclean32.png"));

    mdiArea = new QMdiArea();
    //mdiArea->setBackground(QBrush(QColor(44,44,44)));
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setActivationOrder(QMdiArea::StackingOrder);
    setCentralWidget(mdiArea);

    createActions();
    createMenus();
    createToolBars();

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(graphWindowActivated(QMdiSubWindow*)));


	consoleEdit = new QTextEdit(this);
	QFont f("Consolas", 9);
	f.setStyleHint(QFont::Courier);
	consoleEdit->setFont(f);
	consoleEdit->setText("Not implemented yet...");

	consoleDockWidget = new QDockWidget(tr("FVConsole"), this);
	consoleDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	consoleDockWidget->setWidget(consoleEdit);
	connect(consoleDockWidget, SIGNAL(visibilityChanged(bool)), this->showConsoleAct, SLOT(setChecked(bool)));
	consoleDockWidget->setVisible(false);
	addDockWidget(Qt::BottomDockWidgetArea, consoleDockWidget);
}

void FVMainWindow::createActions()
{
    // Plik

    new2dAct = new QAction(QIcon(":/images/new2d.png"), "Nowe okno wykresu 2d", this);
    new2dAct->setStatusTip("Tworzy nowe okno wykresu funkcji jednej zmiennej");
    connect(new2dAct, SIGNAL(triggered()), this, SLOT(new2d()));

    new3dAct = new QAction(QIcon(":/images/new3d.png"), "Nowe okno wykresu 3d", this);
    new3dAct->setStatusTip("Tworzy nowe okno wykresu funkcji dwóch zmiennych");
    connect(new3dAct, SIGNAL(triggered()), this, SLOT(new3d()));

    openAct = new QAction(QIcon(":/images/open.png"), "Otwórz", this);
    openAct->setStatusTip("Otwórz");
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), "Zapisz", this);
    saveAct->setStatusTip("Zapisz");
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setEnabled(false);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction("Zapisz jako...", this);
    saveAsAct->setStatusTip("Zapisz jako...");
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(QIcon(":/images/exit.png"), "Zakończ", this);
    exitAct->setStatusTip("Zakończ");
    exitAct->setShortcuts(QKeySequence::Close);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(exit()));

    // Edycja

    undoAct = new QAction(QIcon(":/images/undo.png"), "Cofnij", this);
    undoAct->setStatusTip("Cofnij");
    undoAct->setShortcuts(QKeySequence::Undo);
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    redoAct = new QAction(QIcon(":/images/redo.png"), "Powtórz", this);
    redoAct->setStatusTip("Powtórz");
    redoAct->setShortcuts(QKeySequence::Redo);
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

//    programSettingsAct = new QAction(QIcon(":/images/preferences.png"), "Ustawienia programu", this);
//    programSettingsAct->setStatusTip("Ustawienia programu");
//    connect(programSettingsAct, SIGNAL(triggered()), this, SLOT(programSettings()));

    graphSettingsAct = new QAction(QIcon(":/images/graph_window_settings.png"), "Ustawienia okna wykresu", this);
    graphSettingsAct->setStatusTip("Ustawienia okna wykresu");
    connect(graphSettingsAct, SIGNAL(triggered()), this, SLOT(graphSettings()));

    // Widok

    zoomInAct = new QAction(QIcon(":/images/zoom_in.png"), "Powiększ", this);
    zoomInAct->setStatusTip("Powiększ");
    zoomInAct->setShortcut(QKeySequence(Qt::Key_Plus));
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(QIcon(":/images/zoom_out.png"), "Pomniejsz", this);
    zoomOutAct->setStatusTip("Pomniejsz");
    zoomOutAct->setShortcut(QKeySequence(Qt::Key_Minus));
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    zoomInSelectionAct = new QAction(QIcon(":/images/zoom_selection.png"), "Powiększ zaznaczenie", this);
    zoomInSelectionAct->setStatusTip("Powiększ zaznaczenie");
    zoomInSelectionAct->setCheckable(true);
    connect(zoomInSelectionAct, SIGNAL(triggered()), this, SLOT(zoomInSelection()));

    moveLeftAct = new QAction(QIcon(":/images/left.png"), "Przesuń w lewo", this);
    moveLeftAct->setStatusTip("Przesuń w lewo");
    moveLeftAct->setShortcut(QKeySequence(tr("Left")));
    connect(moveLeftAct, SIGNAL(triggered()), this, SLOT(moveLeft()));

    moveRightAct = new QAction(QIcon(":/images/right.png"), "Przesuń w prawo", this);
    moveRightAct->setStatusTip("Przesuń w prawo");
    moveRightAct->setShortcut(QKeySequence(tr("Right")));
    connect(moveRightAct, SIGNAL(triggered()), this, SLOT(moveRight()));

    moveUpAct = new QAction(QIcon(":/images/up.png"), "Przesuń w górę", this);
    moveUpAct->setStatusTip("Przesuń w górę");
    moveUpAct->setShortcut(QKeySequence(tr("Up")));
    connect(moveUpAct, SIGNAL(triggered()), this, SLOT(moveUp()));

    moveDownAct = new QAction(QIcon(":/images/down.png"), "Przesuń w dół‚", this);
    moveDownAct->setStatusTip("Przesuń w dół‚");
    moveDownAct->setShortcut(QKeySequence(tr("Down")));
    connect(moveDownAct, SIGNAL(triggered()), this, SLOT(moveDown()));

    moveNAct = new QAction(QIcon(":/images/moveN.png"), "Przesuń w tył", this);
    moveNAct->setStatusTip("Przesuń w tył");
    moveNAct->setShortcut(QKeySequence(tr("Ctrl+Up")));
    connect(moveNAct, SIGNAL(triggered()), this, SLOT(moveN()));

    moveSAct = new QAction(QIcon(":/images/moveS.png"), "Przesuń w przód‚", this);
    moveSAct->setStatusTip("Przesuń w przód‚");
    moveSAct->setShortcut(QKeySequence(tr("Ctrl+Down")));
    connect(moveSAct, SIGNAL(triggered()), this, SLOT(moveS()));

    showGraphListAct = new QAction("Pokaż listę wykresów", this);
    showGraphListAct->setStatusTip("Pokaż listę wykresów");
    showGraphListAct->setCheckable(true);
    showGraphListAct->setChecked(true);
    connect(showGraphListAct, SIGNAL(triggered()), this, SLOT(showGraphList()));

	showConsoleAct = new QAction("Pokaż konsolę", this);
	showConsoleAct->setStatusTip("Pokaż konsolę");
	showConsoleAct->setCheckable(true);
	showConsoleAct->setChecked(false);
	connect(showConsoleAct, SIGNAL(triggered()), this, SLOT(showConsole()));

    // Wykres

    graphAddAct = new QAction(QIcon(":/images/graph_add.png"), "Dodaj wykres", this);
    graphAddAct->setStatusTip("Dodaj wykres");
    connect(graphAddAct, SIGNAL(triggered()), this, SLOT(graphAdd()));

    graphEditAct = new QAction(QIcon(":/images/graph_edit.png"), "Edytuj wykres", this);
    graphEditAct->setStatusTip("Edytuj wykres");
    connect(graphEditAct, SIGNAL(triggered()), this, SLOT(graphEdit()));

    graphDeleteAct = new QAction(QIcon(":/images/graph_delete.png"), "Usuń wykres", this);
    graphDeleteAct->setStatusTip("Usuń wykres");
    connect(graphDeleteAct, SIGNAL(triggered()), this, SLOT(graphDelete()));

    derivativeAct = new QAction(QIcon(":/images/derivative.png"), "Pochodna", this);
    derivativeAct->setStatusTip("Pochodna");
    connect(derivativeAct, SIGNAL(triggered()), this, SLOT(derivative()));

    rotateGraphAct = new QAction(QIcon(":/images/rotate.png"), "Rotacja wykresu", this);
    rotateGraphAct->setStatusTip("Rotacja wykresu");
    rotateGraphAct->setCheckable(true);
    rotateGraphAct->setChecked(false);
    connect(rotateGraphAct, SIGNAL(triggered()), this, SLOT(rotateGraph()));

    exportToImageAct = new QAction(QIcon(":/images/image.png"), "Eksportuj do obrazu", this);
    exportToImageAct->setStatusTip("Eksportuj do obrazu");
    connect(exportToImageAct, SIGNAL(triggered()), this, SLOT(exportToImage()));

    // Pomoc
    programHelpAct = new QAction(QIcon(":/images/help.png"), "Pomoc programu FVisualizer", this);
    programHelpAct->setShortcut(QKeySequence::HelpContents);
    connect(programHelpAct, SIGNAL(triggered()), this, SLOT(programHelp()));

    aboutAct = new QAction(QIcon(":/images/fviconclean32.png"), "O programie FVisualizer", this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    resetActions();
}

void FVMainWindow::resetActions()
{
    saveAct->setEnabled(false);
    saveAsAct->setEnabled(false);

    undoAct->setEnabled(false);
    redoAct->setEnabled(false);
    graphSettingsAct->setEnabled(true);

    zoomInAct->setEnabled(false);
    zoomOutAct->setEnabled(false);
    zoomInSelectionAct->setEnabled(false);

    moveLeftAct->setEnabled(false);
    moveRightAct->setEnabled(false);
    moveUpAct->setEnabled(false);
    moveDownAct->setEnabled(false);
    moveNAct->setEnabled(false);
    moveSAct->setEnabled(false);
    showGraphListAct->setEnabled(false);

    graphAddAct->setEnabled(false);
    graphEditAct->setEnabled(false);
    graphDeleteAct->setEnabled(false);
    derivativeAct->setEnabled(false);
    exportToImageAct->setEnabled(false);
    rotateGraphAct->setEnabled(false);
}

void FVMainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("Plik"));
    newGraphMenu = fileMenu->addMenu(QIcon(":/images/graph_window_new.png"), tr("Nowe okno wykresu"));
    newGraphMenu->addAction(new2dAct);
    newGraphMenu->addAction(new3dAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);


    editMenu = menuBar()->addMenu(tr("Edycja"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
//    editMenu->addAction(programSettingsAct);
    editMenu->addAction(graphSettingsAct);

    viewMenu = menuBar()->addMenu(tr("Widok"));
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(zoomInSelectionAct);
    viewMenu->addSeparator();
    viewMenu->addAction(moveLeftAct);
    viewMenu->addAction(moveRightAct);
    viewMenu->addAction(moveUpAct);
    viewMenu->addAction(moveDownAct);
    viewMenu->addAction(moveNAct);
    viewMenu->addAction(moveSAct);
    viewMenu->addSeparator();
    viewMenu->addAction(showGraphListAct);
	viewMenu->addAction(showConsoleAct);

    graphMenu = menuBar()->addMenu(tr("Wykres"));
    graphMenu->addAction(graphAddAct);
    graphMenu->addAction(graphEditAct);
    graphMenu->addAction(graphDeleteAct);
    graphMenu->addSeparator();
    graphMenu->addAction(derivativeAct);
    graphMenu->addSeparator();
    graphMenu->addAction(exportToImageAct);
    graphMenu->addAction(rotateGraphAct);

    helpMenu = menuBar()->addMenu(tr("Pomoc"));
    helpMenu->addAction(programHelpAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
}

void FVMainWindow::createToolBars()
{
    stdToolBar = addToolBar("Opcje ogólne");
    stdToolBar->addAction(new2dAct);
    stdToolBar->addAction(new3dAct);
    stdToolBar->addAction(openAct);
    stdToolBar->addAction(saveAct);
    stdToolBar->addSeparator();
    stdToolBar->addAction(undoAct);
    stdToolBar->addAction(redoAct);

    fv2dToolBar = addToolBar("Opcje wykresu 2d");
    fv2dToolBar->addAction(zoomInAct);
    fv2dToolBar->addAction(zoomOutAct);
    fv2dToolBar->addAction(zoomInSelectionAct);
    fv2dToolBar->addSeparator();
    fv2dToolBar->addAction(moveLeftAct);
    fv2dToolBar->addAction(moveRightAct);
    fv2dToolBar->addAction(moveUpAct);
    fv2dToolBar->addAction(moveDownAct);
    fv2dToolBar->addSeparator();
    fv2dToolBar->addAction(derivativeAct);
    fv2dToolBar->hide();

    fv3dToolBar = addToolBar("Opcje wykresu 3d");
    fv3dToolBar->addAction(zoomInAct);
    fv3dToolBar->addAction(zoomOutAct);
    fv3dToolBar->addSeparator();
    fv3dToolBar->addAction(moveLeftAct);
    fv3dToolBar->addAction(moveRightAct);
    fv3dToolBar->addAction(moveUpAct);
    fv3dToolBar->addAction(moveDownAct);
    fv3dToolBar->addAction(moveNAct);
    fv3dToolBar->addAction(moveSAct);
    fv3dToolBar->addSeparator();
    fv3dToolBar->addAction(rotateGraphAct);
    fv3dToolBar->hide();

}

GraphWindow* FVMainWindow::addGraph(GraphWindow* gw)
{
    mdiArea->addSubWindow(gw);
    gw->show();

    if(mdiArea->subWindowList().size() == 1)
        gw->showMaximized();
    else
        mdiArea->cascadeSubWindows();

    return gw;
}

GraphWindow2d* FVMainWindow::new2d()
{
    qDebug("FVMainWindow::new2d()");

    GraphWindow2d* w2d = new GraphWindow2d(mdiArea, this);
    w2d->newGraph();
    addGraph(w2d);

    return w2d;
}

GraphWindow3d* FVMainWindow::new3d()
{
    qDebug("FVMainWindow::new3d()");

    GraphWindow3d* w3d = new GraphWindow3d(mdiArea, this);
    w3d->newGraph();
    addGraph(w3d);

    return w3d;
}

void FVMainWindow::open()
{
    QString fileName =
        QFileDialog::getOpenFileName(this, "Otwórz plik",
                                     QDir::currentPath(),
                                     tr("*.fv2d *.fv3d"));
    if (fileName.isEmpty())
        return;

    qDebug() << fileName;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("FVisualzer"),
             tr("Błąd odczytu pliku %1:\n%2.")
             .arg(fileName)
             .arg(file.errorString()));
        return;
    }

    FVFormatHandler fh;
    if(!fh.read(&file)) return;

    bool error = false;

    qDebug() << "Graph type: " << fh.getGraphType();
    if(fh.getGraphType() == "2d")
    {
        QVector<Function2dInfo> v = fh.parseFunctions2d();

        for(int i = 0; i < v.size(); ++i)
        {
//            qDebug() << v[i].expression << " " << v[i].color << " " << v[i].active << " "
//                     << v[i].drawInInterval << " " << v[i].intervalFrom << " " << v[i].intervalTo;

            if(!v[i].hasValidInfo())
            {
                error = true;
                break;
            }
        }

        PlotSettings2d p = fh.parsePlotsettings2d();

        if(p.minX >= p.maxX || p.minY >= p.maxY)
        {
            error = true;
        }

        if(!error)
        {
            GraphWindow2d* w2d = new GraphWindow2d(mdiArea, this);
            w2d->loadFunctions(v, p, fileName);
            addGraph(w2d);
        }
    }
    else if (fh.getGraphType() == "3d")
    {
        QVector<Function3dInfo> v = fh.parseFunctions3d();

        for(int i = 0; i < v.size(); ++i)
        {
//            qDebug() << v[i].expression << " " << v[i].color << " " << v[i].active << " "
//                     << v[i].drawInInterval << " " << v[i].intervalXFrom << " " << v[i].intervalXTo
//                     << v[i].intervalYFrom << " " << v[i].intervalYTo;
            if(!v[i].hasValidInfo())
            {
                error = true;
                break;
            }
        }

        PlotSettings3d p = fh.parsePlotsettings3d();

        if(p.minX >= p.maxX || p.minY >= p.maxY || p.minZ >= p.maxZ)
        {
            error = true;
        }

        if(!error)
        {
            GraphWindow3d* w3d = new3d();
            w3d->loadFunctions(v, p, fileName);
            addGraph(w3d);
        }
    }

    if(error) QMessageBox::critical(this, "Fvisualizer", "Błąd parsowania pliku.");

    qDebug("FVMainWindow::open()");
}

void FVMainWindow::save()
{
    qDebug("FVMainWindow::save()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->save();
}

void FVMainWindow::saveAs()
{
    qDebug("FVMainWindow::saveAs()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->save(true);
}

void FVMainWindow::exit()
{
    qDebug("FVMainWindow::exit()");
    close();
}

void FVMainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    event->accept();
}

void FVMainWindow::undo()
{
    qDebug("FVMainWindow::undo()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->undo();
}

void FVMainWindow::redo()
{
    qDebug("FVMainWindow::redo()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->redo();
}

void FVMainWindow::programSettings()
{
    qDebug("FVMainWindow::programSettings()");

}

// Zwraca aktywne okno, przedtem następuje rzutowanie w górę
GraphWindow* FVMainWindow::getActiveSubwindow()
{
    return qobject_cast<GraphWindow*>(mdiArea->activeSubWindow());
}

void FVMainWindow::zoomIn()
{
    qDebug("FVMainWindow::zoomIn()");
    //qDebug() << getActiveSubwindow();
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->zoomIn();
}

void FVMainWindow::zoomOut()
{
    qDebug("FVMainWindow::zoomOut()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->zoomOut();
}

void FVMainWindow::zoomInSelection()
{
    qDebug() << "FVMainWindow::zoomInSelection() [checked: " << zoomInSelectionAct->isChecked() << "] (checked is " << Qt::Checked << ")";

    GraphWindow2d* gw2d = qobject_cast<GraphWindow2d*>(getActiveSubwindow());
    if(gw2d != NULL) {
        if(zoomInSelectionAct->isChecked())
            gw2d->zoomSelectionStart();
        else
            gw2d->zoomSelectionCancel();
    }
}

void FVMainWindow::zoomInSelectionFinished()
{
    zoomInSelectionAct->setChecked(false);
}

void FVMainWindow::moveLeft()
{
    qDebug("FVMainWindow::moveLeft()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->scrollLeft();
}

void FVMainWindow::moveRight()
{
    qDebug("FVMainWindow::moveRight()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->scrollRight();
}

void FVMainWindow::moveUp()
{
    qDebug("FVMainWindow::moveUp()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->scrollUp();
}

void FVMainWindow::moveDown()
{
    qDebug("FVMainWindow::moveDown()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->scrollDown();
}

void FVMainWindow::moveN()
{
    qDebug("FVMainWindow::moveN()");
    GraphWindow3d *gw3d = qobject_cast<GraphWindow3d*>(getActiveSubwindow());
    if(gw3d != NULL)
    {
        gw3d->scrollN();
    }
}

void FVMainWindow::moveS()
{
    qDebug("FVMainWindow::moveS()");
    GraphWindow3d *gw3d = qobject_cast<GraphWindow3d*>(getActiveSubwindow());
    if(gw3d != NULL)
    {
        gw3d->scrollS();
    }
}

void FVMainWindow::showGraphList()
{
    qDebug() << "FVMainWindow::showGraphList() [checked: " << showGraphListAct->isChecked() << "]";
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->showGraphList(showGraphListAct->isChecked());
}

void FVMainWindow::showConsole()
{
	qDebug() << "FVMainWindow::showConsole() [checked: " << showConsoleAct->isChecked() << "]";
	if(this->showConsoleAct->isChecked())
		this->consoleDockWidget->show();
	else
		this->consoleDockWidget->hide();
	
}

void FVMainWindow::graphAdd()
{
    qDebug("FVMainWindow::graphAdd()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->graphAdd();
}

void FVMainWindow::graphEdit()
{
    qDebug("FVMainWindow::graphEdit()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->graphEdit();
}

void FVMainWindow::graphDelete()
{
    qDebug("FVMainWindow::graphDelete()");
    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->graphDelete();
}


void FVMainWindow::graphSettings()
{
    qDebug("FVMainWindow::graphSettings()");

    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->graphSettings();
}

void FVMainWindow::derivative()
{
    qDebug("FVMainWindow::derivative()");

    GraphWindow2d* gw2d = qobject_cast<GraphWindow2d*>(getActiveSubwindow());
    if(gw2d != NULL)
    {
        gw2d->derivative();
    }
}

void FVMainWindow::rotateGraph()
{
    qDebug() << "FVMainWindow::rotateGraph() [checked: " << rotateGraphAct->isChecked() << "]";

    GraphWindow3d *gw3d = qobject_cast<GraphWindow3d*>(getActiveSubwindow());
    if(gw3d != NULL)
    {
        if(rotateGraphAct->isChecked())
            gw3d->startRotation();
        else
            gw3d->stopRotation();
    }
}

void FVMainWindow::exportToImage()
{
    qDebug("FVMainWindow::exportToImage()");

    GraphWindow *gw = getActiveSubwindow();
    if(gw != NULL) gw->exportToImage();
}


void FVMainWindow::programHelp()
{
    qDebug("FVMainWindow::programHelp()");

    HelpDialog *dlg = new HelpDialog();
    dlg->show();
}

void FVMainWindow::about()
{
    qDebug("FVMainWindow::about()");

    QPixmap pixmap(":/images/splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();
}

void FVMainWindow::graphWindowActivated(QMdiSubWindow *window)
{
    qDebug() << "graphWindow activated: " << window;

    if(window == NULL) {
        fv2dToolBar->setDisabled(true);
        fv2dToolBar->setHidden(true);
        fv3dToolBar->setDisabled(true);
        fv3dToolBar->setHidden(true);

        resetActions();
    }

    GraphWindow2d* gw2d = NULL;
    GraphWindow3d* gw3d = NULL;

    if((gw2d = qobject_cast<GraphWindow2d*>(window)) != NULL)
    {
        fv3dToolBar->setDisabled(true);
        fv3dToolBar->setHidden(true);
        fv2dToolBar->setDisabled(false);
        fv2dToolBar->setHidden(false);

        gw2d->updateActions();
    }
    else if((gw3d = qobject_cast<GraphWindow3d*>(window)) != NULL)
    {
        fv2dToolBar->setDisabled(true);
        fv2dToolBar->setHidden(true);
        fv3dToolBar->setDisabled(false);
        fv3dToolBar->setHidden(false);

        gw3d->updateActions();
    }
}
