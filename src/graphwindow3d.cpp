#include "graphwindow3d.h"
#include "plotter3d.h"
#include "fvmainwindow.h"
#include "functionslistview.h"
#include "function3ddialog.h"
#include "commands.h"
#include "fvformathandler.h"
#include "graphsettings3ddialog.h"

GraphWindow3d::GraphWindow3d(QWidget *parentMdi, FVMainWindow *mainWindow)
    : GraphWindow(parentMdi, mainWindow)
{
    expressionLabel->setText("f(x,y) =");
	plotter3d = new Plotter3d(functionsList, raysList);
    bgLayout->insertWidget(0, plotter3d);
    setWindowTitle("Wykres 3d");
    setWindowIcon(QIcon(":/images/new3d.png"));

    functionsListView->w3d = true;

    connect(expressionInput, SIGNAL(returnPressed()), this, SLOT(addLineEditFunction()));
    connect(functionsListView->functionsListItemModel, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(functionOnListViewChanged(QStandardItem*)));
}

GraphWindow3d::~GraphWindow3d()
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

void GraphWindow3d::newGraph()
{
    static int sequence = 1;
    curFile = QString("wykres3d%1.fv3d").arg(sequence++);
    isUntitled = true;
    setWindowTitle(curFile + "[*]");
}

void GraphWindow3d::addFunction(Function3d* f)
{
//    functionsList.append(f);
//    plotter3d->updateGL();
//    functionsListView->addFunction(f->expression, f->color);
    QUndoCommand *addFunction3dCommand = new AddFunction3dCommand(
            f, functionsList, *functionsListView, *plotter3d);
    undoStack->push(addFunction3dCommand);
    plotter3d->updateGL();

    emit windowModified(true);
}

void GraphWindow3d::addFunction(Function3dInfo& fi)
{
    Function3d* f = new Function3d(fi);
    f->recalculate(plotter3d->getPlotsettings());
    functionsList.append(f);
    functionsListView->addFunction(f->expression, f->color, f->active);
    plotter3d->updateGL();
}

void GraphWindow3d::loadFunctions(QVector<Function3dInfo>& v, PlotSettings3d& p, QString& filepath)
{
    curFile = QFileInfo(filepath).canonicalFilePath();
    isUntitled = false;
    setWindowTitle(QFileInfo(filepath).fileName() + "[*]");

    plotter3d->setPlotSettings(p);

    for(int i = 0; i < v.size(); ++i)
    {
        addFunction(v[i]);
    }
}

void GraphWindow3d::addLineEditFunction()
{
    qDebug() << "GraphWindow3d::addFunction()";

	
    static const QColor graphColors[] = {
        Qt::gray, Qt::yellow, QColor(0x9999ff), Qt::red, Qt::green, Qt::blue,
        Qt::darkGreen, Qt::darkRed, Qt::cyan, Qt::magenta
    };

	QString s = expressionInput->text().trimmed();

	QRegExp addVectRegex("^\\[\\-?\\d+(\\.\\d+)?,\\s?\\-?\\d+(\\.\\d+)?,\\s?\\-?\\d+(\\.\\d+)?\\]$");

	if(addVectRegex.exactMatch(s))
	{
		qDebug() << "addvec: " << s << "\n";
		s.remove(QChar(' '), Qt::CaseInsensitive); // remove spaces
		s.remove(0, 1); // remove opening bracket
		s.remove(s.length()-1, 1); // remove closing bracket

		QStringList lst = s.split(','); // split by ","

		if(lst.length() == 3)
		{
			QVector3D v;
			v.setX(lst.at(0).toDouble());
			v.setY(lst.at(1).toDouble());
			v.setZ(lst.at(2).toDouble());

			qDebug() << "got vector3d: " << v;
			Ray r;
			r.beginPoint = QVector3D(0.0,0.0,0.0);
			r.endPoint = v;
			r.color = graphColors[raysList.count() % 10];
			raysList.append(r);
			plotter3d->updateGL();
		}

		qDebug() << "addvec prepared string: " << s;

		expressionInput->clear();
		return;
	}


	// Ray (sorry for ugly regex)
	QRegExp addRayRegex("^\\[\\-?\\d+(\\.\\d+)?,\\s?\\-?\\d+(\\.\\d+)?,\\s?\\-?\\d+(\\.\\d+)?\\;\\-?\\d+(\\.\\d+)?,\\s?\\-?\\d+(\\.\\d+)?,\\s?\\-?\\d+(\\.\\d+)?\\]$");

	if(addRayRegex.exactMatch(s))
	{
		qDebug() << "adding ray";

		qDebug() << "addvec: " << s << "\n";
		s.remove(QChar(' '), Qt::CaseInsensitive); // remove spaces
		s.remove(0, 1); // remove opening bracket [
		s.remove(s.length()-1, 1); // remove closing bracket ]

		QStringList lst = s.split(';'); // split by ";"

		if(lst.length() == 2)
		{
			QStringList lstb = lst.at(0).split(',');
			QVector3D vb;
			vb.setX(lstb.at(0).toDouble());
			vb.setY(lstb.at(1).toDouble());
			vb.setZ(lstb.at(2).toDouble());

			QStringList lste = lst.at(1).split(',');
			QVector3D ve;
			ve.setX(lste.at(0).toDouble());
			ve.setY(lste.at(1).toDouble());
			ve.setZ(lste.at(2).toDouble());

			Ray r;
			r.beginPoint = vb;
			r.endPoint = ve;
			r.color = graphColors[raysList.count() % 10];
			raysList.append(r);
			plotter3d->updateGL();

			expressionInput->clear();

			return;
		}

		qDebug() << "addvec prepared string: " << s;

	}

    if(!expressionInput->text().isEmpty())
    {
        Function3d *f = NULL;
        QColor c(graphColors[functionsList.count() % 10]);
        try {
            //plotter2d->addFunction(expressionInput->text(), c);
            f = new Function3d(expressionInput->text());
            f->color = c;
            f->recalculate(plotter3d->getPlotsettings());

            addFunction(f);

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

void GraphWindow3d::functionOnListViewChanged(QStandardItem* item)
{
//    int i = item->index().row();
//    functionsList.at(i)->active = item->checkState() == Qt::Checked;
//    plotter3d->updateGL();
    int i = item->index().row();
    bool state = functionsList.at(i)->active == (item->checkState() == Qt::Checked);
    if(!state)
    {
//        Function3dInfo before(functionsList.at(i));
//        functionsList.at(i)->active = item->checkState() == Qt::Checked;
        QUndoCommand *stateChangedFunction3dCommand = new StateChangedFunction3dCommand(i,
            functionsList, *functionsListView);
        undoStack->push(stateChangedFunction3dCommand);

        //functionsList[i]->recalculate(plotter3d->getPlotsettings());
        plotter3d->updateGL();

        emit windowModified(true);
    }
}

void GraphWindow3d::zoomIn()
{
    //plotter3d->zoomIn();
    QUndoCommand *zoomPlotter3dCommand = new ZoomPlotter3dCommand(*plotter3d, false);
    undoStack->push(zoomPlotter3dCommand);
    emit windowModified(true);
}

void GraphWindow3d::zoomOut()
{
    //plotter3d->zoomOut();
    QUndoCommand *zoomPlotter3dCommand = new ZoomPlotter3dCommand(*plotter3d, true);
    undoStack->push(zoomPlotter3dCommand);
    emit windowModified(true);
}

void GraphWindow3d::scrollLeft()
{
    //plotter3d->scrollLeft();
    QUndoCommand *scrollPlotter3dCommand = new ScrollPlotter3dCommand(*plotter3d, 4);
    undoStack->push(scrollPlotter3dCommand);
    emit windowModified(true);
}

void GraphWindow3d::scrollRight()
{
    //plotter3d->scrollRight();
    QUndoCommand *scrollPlotter3dCommand = new ScrollPlotter3dCommand(*plotter3d, 2);
    undoStack->push(scrollPlotter3dCommand);
    emit windowModified(true);
}

void GraphWindow3d::scrollUp()
{
    //plotter3d->scrollUp();
    QUndoCommand *scrollPlotter3dCommand = new ScrollPlotter3dCommand(*plotter3d, 1);
    undoStack->push(scrollPlotter3dCommand);
    emit windowModified(true);
}

void GraphWindow3d::scrollDown()
{
    //plotter3d->scrollDown();
    QUndoCommand *scrollPlotter3dCommand = new ScrollPlotter3dCommand(*plotter3d, 3);
    undoStack->push(scrollPlotter3dCommand);
    emit windowModified(true);
}

void GraphWindow3d::scrollN()
{
    //plotter3d->scrollN();
    QUndoCommand *scrollPlotter3dCommand = new ScrollPlotter3dCommand(*plotter3d, 5);
    undoStack->push(scrollPlotter3dCommand);
    emit windowModified(true);
}

void GraphWindow3d::scrollS()
{
    //plotter3d->scrollS();
    QUndoCommand *scrollPlotter3dCommand = new ScrollPlotter3dCommand(*plotter3d, 6);
    undoStack->push(scrollPlotter3dCommand);
    emit windowModified(true);
}


void GraphWindow3d::graphEdit()
{
    QModelIndex idx = functionsListView->selectionModel()->currentIndex();
    if(idx.isValid())
    {
        Function3d* f = functionsList[idx.row()];
        Function3dDialog dlg(this);
        dlg.attachFunction(f);

        Function3dInfo before(f);

        if(dlg.exec())
        {
//            functionsListView->updateFunctionAtIndex(idx.row(), f->expression, f->color);

            QUndoCommand *editFunction3dCommand = new EditFunction3dCommand(idx.row(), before,
                functionsList, *functionsListView, *plotter3d);
            undoStack->push(editFunction3dCommand);
            //plotter3d->recalculateAll();
            plotter3d->updateGL();

            emit windowModified(true);
        }
    }
}

void GraphWindow3d::graphDelete()
{
    QModelIndex idx = functionsListView->selectionModel()->currentIndex();
    if(idx.isValid())
    {
//        delete functionsList.at(idx.row());
//        functionsList.remove(idx.row());
//        functionsListView->removeFunction(idx.row());
        QUndoCommand *removeFunction3dCommand = new RemoveFunction3dCommand(
                idx.row(), functionsList, *functionsListView, *plotter3d);
        undoStack->push(removeFunction3dCommand);
        plotter3d->updateGL();

        emit windowModified(true);
    }
}

void GraphWindow3d::graphAdd()
{
    Function3dDialog dlg(this);

    Function3d* f = new Function3d();

    if(!f) {
        qDebug() << "GraphWindow3d::addGraph(): f iz NULL";
        return;
    }

    dlg.attachFunction(f);

    if(dlg.exec()) {
        qDebug() << "GraphWindow3d::addGraph(): dlg.exec";

        f->recalculate(plotter3d->getPlotsettings());
        addFunction(f);
    }
    else
        delete f;
}

void GraphWindow3d::undo()
{
    if(undoStack->canUndo()) {
        undoStack->undo();
        //plotter3d->recalculateAll();
        plotter3d->updateGL();
        emit windowModified(undoStack->index() != lastSaveUndoStackIndex, true);
    }
}

void GraphWindow3d::redo()
{
    if(undoStack->canRedo()) {
        undoStack->redo();
        //plotter3d->recalculateAll();
        plotter3d->updateGL();
        emit windowModified(undoStack->index() != lastSaveUndoStackIndex, true);
    }
}

void GraphWindow3d::startRotation()
{
    plotter3d->startRotation();
}

void GraphWindow3d::stopRotation()
{
    plotter3d->stopRotation();
}

void GraphWindow3d::save(bool saveAs)
{
    qDebug("GraphWindow3d::save()");

    QString filepath;

    if(isUntitled || saveAs)
    {
        filepath = QFileDialog::getSaveFileName(
            this,
            tr("Zapisz wykres funkcji 3d"),
            curFile,
            "*.fv3d"
        );
    }
    else
        filepath = curFile;

    if(filepath.isEmpty()) return;

    FVFormatHandler fh;
    fh.saveGraph3d(filepath, functionsList, plotter3d->getPlotsettings());

    if(isUntitled)
    {
        isUntitled = false;
        curFile = filepath;
        setWindowTitle(QFileInfo(filepath).fileName() + "[*]");
    }

    lastSaveUndoStackIndex = undoStack->index();
    emit windowModified(false);
}

void GraphWindow3d::exportToImage()
{
    QFileInfo fi(curFile);
    savePixmapAsImage(QPixmap::fromImage(plotter3d->grabFrameBuffer(true)), fi.completeBaseName());
}

bool GraphWindow3d::maybeSave()
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

void GraphWindow3d::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void GraphWindow3d::graphSettings()
{
    GraphSettings3dDialog dlg(this);
    PlotSettings3d before = plotter3d->getPlotsettings();
    dlg.setPlotSettings3d(plotter3d->getPlotsettings());

    if(dlg.exec())
    {
        plotter3d->setPlotSettings(dlg.getPlotSettings3d());
        QUndoCommand *plotSettings3dChangedCommand =
            new PlotSettings3dChangedCommand(*plotter3d, before);
        undoStack->push(plotSettings3dChangedCommand);
        emit windowModified(true);
    }
}

void GraphWindow3d::updateActions()
{
    mainWindow->saveAct->setEnabled(isWindowModified());
    mainWindow->saveAsAct->setEnabled(true);

    mainWindow->undoAct->setEnabled(undoStack->canUndo());
    mainWindow->redoAct->setEnabled(undoStack->canRedo());
    mainWindow->graphSettingsAct->setEnabled(true);

    mainWindow->zoomInAct->setEnabled(true);
    mainWindow->zoomOutAct->setEnabled(true);
    mainWindow->zoomInSelectionAct->setEnabled(false);

    mainWindow->moveLeftAct->setEnabled(true);
    mainWindow->moveRightAct->setEnabled(true);
    mainWindow->moveUpAct->setEnabled(true);
    mainWindow->moveDownAct->setEnabled(true);
    mainWindow->moveNAct->setEnabled(true);
    mainWindow->moveSAct->setEnabled(true);
    mainWindow->showGraphListAct->setEnabled(true);
    mainWindow->showGraphListAct->setChecked(functionsListView->isVisible());

    mainWindow->graphAddAct->setEnabled(true);
    mainWindow->graphEditAct->setEnabled(true);
    mainWindow->graphDeleteAct->setEnabled(true);
    mainWindow->derivativeAct->setEnabled(false);
    mainWindow->exportToImageAct->setEnabled(true);
    mainWindow->rotateGraphAct->setEnabled(true);
}
