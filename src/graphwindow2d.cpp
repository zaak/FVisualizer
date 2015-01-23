#include "graphwindow2d.h"
#include "plotter2d.h"
#include "functionslistview.h"
#include "fvmainwindow.h"
#include "derivativedialog.h"
#include "function2ddialog.h"
#include "fvformathandler.h"
#include "graphsettings2ddialog.h"

GraphWindow2d::GraphWindow2d(QWidget *parentMdi, FVMainWindow *mainWindow)
    : GraphWindow(parentMdi, mainWindow)/*, uStackIndex(0), rStackIndex(0)*/
{
    expressionLabel->setText("f(x) =");
    plotter2d = new Plotter2d(functionsList);
    bgLayout->insertWidget(0, plotter2d);
    setWindowTitle("Wykres 2d");
    setWindowIcon(QIcon(":/images/new2d.png"));

    connect(expressionInput, SIGNAL(returnPressed()), this, SLOT(addLineEditFunction()));
    connect(plotter2d, SIGNAL(zoomSelectionFinished()), mainWindow, SLOT(zoomInSelectionFinished()));
    connect(plotter2d, SIGNAL(zoomSelectionFinished()), this, SLOT(zoomSelectionFinished()));
//    connect(plotter2d, SIGNAL(zoomSelectionFinished()), this, SLOT(updateURStack()));
    connect(functionsListView->functionsListItemModel, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(functionOnListViewChanged(QStandardItem*)));
}

void GraphWindow2d::newGraph()
{
    static int sequence = 1;
    curFile = QString("wykres2d%1.fv2d").arg(sequence++);
    isUntitled = true;
    setWindowTitle(curFile + "[*]");
}

GraphWindow2d::~GraphWindow2d()
{
    if(!functionsList.empty())
    {
        for(int i = 0; i < functionsList.size(); ++i)
        {
            if(functionsList[i])
                delete functionsList[i];
        }
    }
}

void GraphWindow2d::addFunction(Function2d *f)
{
    QUndoCommand *addFunction2dCommand = new AddFunction2dCommand(f, functionsList, *functionsListView);
    undoStack->push(addFunction2dCommand);
    plotter2d->refreshPixmap();

    emit windowModified(true);
}

void GraphWindow2d::loadFunctions(QVector<Function2dInfo>& v, PlotSettings2d& p, QString filepath)
{
    curFile = QFileInfo(filepath).canonicalFilePath();
    qDebug() << "canonicalFilePath " << curFile;
    isUntitled = false;
    setWindowTitle(QFileInfo(filepath).fileName() + "[*]");

    plotter2d->setPlotSettings(p);

    for(int i = 0; i < v.size(); ++i)
    {
        addFunction(v[i]);
    }
}

void GraphWindow2d::addFunction(Function2dInfo &fi)
{
    Function2d* f = new Function2d(fi);
    functionsList.append(f);
    plotter2d->refreshPixmap();
    functionsListView->addFunction(f->expression, f->color, f->active);
}

void GraphWindow2d::addLineEditFunction()
{
    static const QColor graphColors[10] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta,
        Qt::yellow, Qt::gray, Qt::black, Qt::darkGreen, Qt::darkRed
    };

    qDebug("GraphWindow2d::addFunction()");
    if(!expressionInput->text().isEmpty())
    {
        Function2d *f = NULL;
        QColor c(graphColors[plotter2d->functionsList.count() % 10]);
        try {
            //plotter2d->addFunction(expressionInput->text(), c);
            f = new Function2d(expressionInput->text());
            f->color = c;
            f->recalculate(plotter2d->getPlotsettings());

            addFunction(f);
//            updateURStack();

            expressionInput->clear();
        }
        catch (WrongNumberOfBracketsException&) {
            delete f;
            errorMsgBox("Błąd wyrażenia", "Wprowadzone wyrażenie zwiera błędy\nSprawdź liczbę nawiasów");
        }
        catch (ParsingException&) {
            delete f;
            errorMsgBox("Błąd wyrażenia", "Wprowadzone wyrażenie zwiera błędy");
        }
    }
}



void GraphWindow2d::zoomIn()
{
//    plotter2d->zoomIn();
    QUndoCommand *zoomPlotter2dCommand = new ZoomPlotter2dCommand(*plotter2d, false);
    undoStack->push(zoomPlotter2dCommand);
//    updateURStack();
    emit windowModified(true);
}

void GraphWindow2d::zoomOut()
{
//    plotter2d->zoomOut();
    QUndoCommand *zoomPlotter2dCommand = new ZoomPlotter2dCommand(*plotter2d, true);
    undoStack->push(zoomPlotter2dCommand);
//    updateURStack();
    emit windowModified(true);
}

void GraphWindow2d::scrollLeft()
{
    QUndoCommand *scrollPlotter2dCommand = new ScrollPlotter2dCommand(*plotter2d, PLOTTER2D_SCROLL_LEFT);
    undoStack->push(scrollPlotter2dCommand);
    emit windowModified(true);
}

void GraphWindow2d::scrollRight()
{
//    plotter2d->scrollRight();
    QUndoCommand *scrollPlotter2dCommand = new ScrollPlotter2dCommand(*plotter2d, PLOTTER2D_SCROLL_RIGHT);
    undoStack->push(scrollPlotter2dCommand);
//    updateURStack();
    emit windowModified(true);
}

void GraphWindow2d::scrollUp()
{
//    plotter2d->scrollUp();
    QUndoCommand *scrollPlotter2dCommand = new ScrollPlotter2dCommand(*plotter2d, PLOTTER2D_SCROLL_UP);
    undoStack->push(scrollPlotter2dCommand);
//    updateURStack();
    emit windowModified(true);
}

void GraphWindow2d::scrollDown()
{
//   plotter2d->scrollDown();
    QUndoCommand *scrollPlotter2dCommand = new ScrollPlotter2dCommand(*plotter2d, PLOTTER2D_SCROLL_DOWN);
    undoStack->push(scrollPlotter2dCommand);
//    updateURStack();
    emit windowModified(true);
}

void GraphWindow2d::zoomSelectionStart()
{
    beforeSelection = plotter2d->getPlotsettings();
    plotter2d->zoomSelectionStart();
}

void GraphWindow2d::zoomSelectionCancel()
{
    plotter2d->zoomSelectionCancel();
}

void GraphWindow2d::zoomSelectionFinished()
{
    QUndoCommand *zoomSelectionPlotter2dCommand =
        new ZoomSelectionPlotter2dCommand(*plotter2d, beforeSelection);
    undoStack->push(zoomSelectionPlotter2dCommand);
//    plotter2d->zoomSelectionFinished();
    emit windowModified(true);
}

void GraphWindow2d::functionOnListViewChanged(QStandardItem* item)
{
    int i = item->index().row();
    bool state = functionsList.at(i)->active == (item->checkState() == Qt::Checked);
//    functionsList.at(i)->active = item->checkState() == Qt::Checked;
    if(!state)
    {
        Function2dInfo before(functionsList.at(i));
        functionsList.at(i)->active = item->checkState() == Qt::Checked;
        QUndoCommand *editFunction2dCommand = new EditFunction2dCommand(i, before,
            functionsList, *functionsListView);
        undoStack->push(editFunction2dCommand);

        emit windowModified(true);
    }
//        updateURStack();
   plotter2d->refreshPixmap();

//    qDebug() << item->text();
//    qDebug() << functionsList[i]->expression;
//
//    ///
//    functionsListView->functionsListItemModel->removeRow(item->index().row());
//    delete functionsList[i];
//    functionsList.remove(i);
}

void GraphWindow2d::derivative()
{
    //    functionsListView->functionsListItemModel->s
    QModelIndex idx = functionsListView->selectionModel()->currentIndex();
    if(idx.isValid())
    {
        static const QColor graphColors[10] = {
            Qt::magenta, Qt::gray, Qt::black, Qt::darkGreen, Qt::darkRed,
            Qt::yellow, Qt::red, Qt::green, Qt::blue, Qt::cyan
        };

        static int cnt = 0;

        Function2d* fBase = functionsList[idx.row()];
        QString derivative = fBase->getDerivative();

        DerivativeDialog dlg(this);
        dlg.setDFColor(graphColors[cnt++ % 10]);
        dlg.setFunctionsStrings(fBase->expression, derivative);

        if(dlg.exec())
        {
            Function2d *f = NULL;
            try {
                //plotter2d->addFunction(expressionInput->text(), c);
                Function2d *f = new Function2d(derivative);
                f->color = dlg.getDFColor();
                f->recalculate(plotter2d->getPlotsettings());

                addFunction(f);
//                updateURStack();
            }
            catch (ParsingException)
            {
                delete f;
                QMessageBox::critical(this, "Błąd wyrażenia", "Wprowadzone wyrażenie zwiera błędy");
            }
        }
    }
}

void GraphWindow2d::graphEdit()
{
    QModelIndex idx = functionsListView->selectionModel()->currentIndex();
    if(idx.isValid())
    {
        Function2d* f = functionsList[idx.row()];
        Function2dDialog dlg(this);
        dlg.attachFunction(f);

        Function2dInfo before(f);

        if(dlg.exec())
        {
            QUndoCommand *editFunction2dCommand = new EditFunction2dCommand(idx.row(), before,
                functionsList, *functionsListView);
            undoStack->push(editFunction2dCommand);
//            functionsListView->updateFunctionAtIndex(idx, f->expression, f->color);
//            updateURStack();
            plotter2d->refreshPixmap();

        //    setWindowModified(true);
//            emit windowModified(true);
            emit windowModified(true);
        }
    }
}

void GraphWindow2d::graphDelete()
{
    QModelIndex idx = functionsListView->selectionModel()->currentIndex();
    if(idx.isValid())
    {
//        delete functionsList.at(idx.row());
//        functionsList.remove(idx.row());
//        functionsListView->removeFunction(idx.row());
        QUndoCommand *removeFunction2dCommand = new RemoveFunction2dCommand(idx.row(), functionsList, *functionsListView);
        undoStack->push(removeFunction2dCommand);
        qDebug() << "GraphWindow2d::graphDelete() " << idx.row();
//        updateURStack();
        plotter2d->refreshPixmap(false);

//        setWindowModified(true);
//        emit windowModified(true);
        emit windowModified(true);
    }
}

void GraphWindow2d::graphAdd()
{
    Function2dDialog dlg(this);

    Function2d* f = new Function2d();

    if(!f) {
        qDebug() << "GraphWindow2d::addGraph(): f iz NULL";
        return;
    }

    dlg.attachFunction(f);

    if(dlg.exec())
    {
        addFunction(f);
//        updateURStack();
    }
    else
        delete f;
}
//
//void GraphWindow2d::updateURStack()
//{
//    if(rStackIndex < urStack.size())
//    {
//        qDebug() << "urStack.size(): " << urStack.size() << " rStackIndex" << rStackIndex;
//        qDebug() << "removing: " << rStackIndex << " - " << urStack.size()-rStackIndex;
//        urStack.remove(rStackIndex, urStack.size()-rStackIndex);
//    }
//    GraphWindow2dSnapshot snapshot;
//    snapshot.plotSettings = plotter2d->getPlotsettings();
//    QVectorIterator<Function2d*> it(functionsList);
//    while(it.hasNext())
//    {
//        snapshot.functionsInfoList.append(Function2dInfo(it.next()));
//    }
//
//    urStack.append(snapshot);
//    uStackIndex = urStack.size() - 2;
//    rStackIndex = urStack.size();
//
//    qDebug() << "urStack.size(): " << urStack.size() << " uStackIndex" << uStackIndex << " rStackIndex" << rStackIndex;
//}
//
//
//void GraphWindow2d::updateFromSnapshot(int index)
//{
//    GraphWindow2dSnapshot snapshot = urStack.at(index);
//
//    plotter2d->plotSettings = snapshot.plotSettings;
//
//    for(int i = 0; i < functionsList.size(); ++i)
//    {
//        if(functionsList[i])
//            delete functionsList[i];
//    }
//
//    functionsList.clear();
//    functionsListView->functionsListItemModel->clear();
//
//    for(int i = 0; i < snapshot.functionsInfoList.size(); ++i)
//    {
//        addFunction(new Function2d(snapshot.functionsInfoList[i]));
//    }
//
//    plotter2d->refreshPixmap();
//}

void GraphWindow2d::undo()
{
//    qDebug() << "urStack.size(): " << urStack.size() << " uStackIndex" << uStackIndex;
//    if(uStackIndex < 0)
//    {
//        functionsList.clear();
//        functionsListView->functionsListItemModel->clear();
//        plotter2d->plotSettings = PlotSettings();
//        plotter2d->refreshPixmap();
//        rStackIndex = 0;
//        return;
//    }
//    rStackIndex = uStackIndex+1;
//    updateFromSnapshot(uStackIndex--);
//    qDebug() << "urStack.size(): " << urStack.size() << " uStackIndex" << uStackIndex;
    if(undoStack->canUndo()) {
        undoStack->undo();
        plotter2d->refreshPixmap();
        emit windowModified(undoStack->index() != lastSaveUndoStackIndex, true);
    }

}

void GraphWindow2d::redo()
{
//    qDebug() << "urStack.size(): " << urStack.size() << " rStackIndex" << rStackIndex;
//    if(rStackIndex >= urStack.size()) return;
//   // else if(urStack < urStackIndex-1) ++urStackIndex;
//    uStackIndex = rStackIndex-1;
//    updateFromSnapshot(rStackIndex++);
//    qDebug() << "urStack.size(): " << urStack.size() << " rStackIndex" << rStackIndex;
    if(undoStack->canRedo()) {
        undoStack->redo();
        plotter2d->refreshPixmap();
        emit windowModified(undoStack->index() != lastSaveUndoStackIndex, true);
    }



    qDebug() << "undoStack->index()" << undoStack->index() << "lastSaveUndoStackIndex" << lastSaveUndoStackIndex;
}

void GraphWindow2d::save(bool saveAs)
{
    qDebug("GraphWindow2d::save()");

//    QDomDocument domDocument("fvisualizer");
//    QDomNode xmlNode = domDocument.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
//    domDocument.insertBefore(xmlNode, domDocument.firstChild());
//
//    QDomElement fvisualizerElem = domDocument.createElement("fvisualizer");
//    fvisualizerElem.setAttribute("version", "1.0");
//    fvisualizerElem.setAttribute("graphtype", "2d");
//    domDocument.appendChild(fvisualizerElem);
//
//    fvisualizerElem.appendChild(plotter2d->getPlotsettings().toXml(domDocument));
//
//    QDomElement functionsElem = domDocument.createElement("functions");
//    fvisualizerElem.appendChild(functionsElem);
//
//    for(int i = 0; i < functionsList.size(); ++i)
//    {
//        functionsElem.appendChild(functionsList[i]->toXml(domDocument));
//    }

    QString filepath;

    if(isUntitled || saveAs)
    {
        filepath = QFileDialog::getSaveFileName(
            this,
            tr("Zapisz wykres funkcji 2d"),
            curFile,
            "*.fv2d"
        );
    }
    else
        filepath = curFile;

    if(filepath.isEmpty()) return;

    FVFormatHandler fh;
    fh.saveGraph2d(filepath, functionsList, plotter2d->getPlotsettings());

//    QFile file(filepath);
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
//    {
//        QMessageBox::warning(this, tr("FVisualzer"), "Błąd zapisu pliku.");
//        return;
//    }
//
//    QTextStream out(&file);
//
//    const int indentSize = 4;
//
//    domDocument.save(out, indentSize);

    if(isUntitled)
    {
        isUntitled = false;
        curFile = filepath;
        setWindowTitle(QFileInfo(filepath).fileName() + "[*]");
    }
//    setWindowModified(false);
    lastSaveUndoStackIndex = undoStack->index();
    emit windowModified(false);
}

void GraphWindow2d::exportToImage()
{
    QFileInfo fi(curFile);
    savePixmapAsImage(plotter2d->getPixmap(), fi.completeBaseName());
}

bool GraphWindow2d::maybeSave()
{
    if (isWindowModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this,
            tr("FVisualizer"), QString("Plik '%1' został zmodyfikowany.\nCzy chcesz zachować zmiany?")
                                .arg(QFileInfo(curFile).fileName()),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save)
        {
            save();
            return true;
        }
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void GraphWindow2d::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void GraphWindow2d::graphSettings()
{
    GraphSettings2dDialog dlg(this);
    PlotSettings2d before = plotter2d->getPlotsettings();
    dlg.setPlotSettings2d(plotter2d->getPlotsettings());

    if(dlg.exec())
    {
        plotter2d->setPlotSettings(dlg.getPlotSettings2d());
        QUndoCommand *zoomSelectionPlotter2dCommand =
            new ZoomSelectionPlotter2dCommand(*plotter2d, before);
        undoStack->push(zoomSelectionPlotter2dCommand);
        emit windowModified(true);
    }
}

void GraphWindow2d::updateActions()
{
    mainWindow->saveAct->setEnabled(isWindowModified());
    mainWindow->saveAsAct->setEnabled(true);

    mainWindow->undoAct->setEnabled(undoStack->canUndo());
    mainWindow->redoAct->setEnabled(undoStack->canRedo());
    mainWindow->graphSettingsAct->setEnabled(true);

    mainWindow->zoomInAct->setEnabled(true);
    mainWindow->zoomOutAct->setEnabled(true);
    mainWindow->zoomInSelectionAct->setEnabled(true);

    mainWindow->moveLeftAct->setEnabled(true);
    mainWindow->moveRightAct->setEnabled(true);
    mainWindow->moveUpAct->setEnabled(true);
    mainWindow->moveDownAct->setEnabled(true);
    mainWindow->moveNAct->setEnabled(false);
    mainWindow->moveSAct->setEnabled(false);
    mainWindow->showGraphListAct->setEnabled(true);
    mainWindow->showGraphListAct->setChecked(functionsListView->isVisible());

    mainWindow->graphAddAct->setEnabled(true);
    mainWindow->graphEditAct->setEnabled(true);
    mainWindow->graphDeleteAct->setEnabled(true);
    mainWindow->derivativeAct->setEnabled(true);
    mainWindow->exportToImageAct->setEnabled(true);
    mainWindow->rotateGraphAct->setEnabled(false);
}













