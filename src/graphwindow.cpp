#include "graphwindow.h"
#include <QDebug>
#include <QStyle>
#include "functionslistview.h"
#include "fvmainwindow.h"

GraphWindow::GraphWindow(QWidget *parentMdi, FVMainWindow *mainWindow)
    : parentMdi(parentMdi), mainWindow(mainWindow), lastSaveUndoStackIndex(0)
{

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setAttribute(Qt::WA_DeleteOnClose);

    Q_ASSERT(parentMdi != NULL);
    qDebug() << "Creating subwindow\nparentMdi geometry: " << parentMdi->geometry();
    qDebug() << this << "[GraphWindow]";

    splitter = new QSplitter(this);

    functionsListView = new FunctionsListView(mainWindow);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    splitter->insertWidget(0, functionsListView);
//    QList<int> sizes;
//    sizes.append(150);
//    sizes.append(600);
//    splitter->setSizes(sizes);

    expressionInput = new QLineEdit(this);
    bgWidget = new QWidget(this);
    bgLayout = new QVBoxLayout(bgWidget);
    bgLayout->setMargin(0);

    expressionWidget = new QWidget(this);
    expressionLayout = new QHBoxLayout(expressionWidget);
    expressionLayout->setMargin(0);
    expressionLabel = new QLabel(this);
    expressionLayout->addWidget(expressionLabel);
    expressionLayout->addWidget(expressionInput);
    expressionWidget->setLayout(expressionLayout);

    bgLayout->insertWidget(1, expressionWidget);
    bgWidget->setLayout(bgLayout);
    splitter->insertWidget(1, bgWidget);


    splitter->setContentsMargins(5, 5, 5, 5);

//    functionsListItemModel = new QStandardItemModel(0, 1, this);
//    functionsListItemModel->setHeaderData(0, Qt::Horizontal, tr("Lista wykresów"));
//    functionsList->setModel(functionsListItemModel);

    setWidget(splitter);

    undoStack = new QUndoStack();

    connect(this, SIGNAL(windowModified(bool)), this, SLOT(setGraphWindowModified(bool)));
}

void GraphWindow::errorMsgBox(QString boxTitle, QString boxMessage) {
    QPalette p;
    p.setColor(expressionInput->backgroundRole(), Qt::red);
    p.setColor(expressionInput->foregroundRole(), Qt::white);
    expressionInput->setPalette(p);

    QMessageBox mb(this);
    mb.setWindowTitle(boxTitle);
    mb.setText(boxMessage);
    mb.setIcon(QMessageBox::Critical);
    mb.exec();

    p.setColor(expressionInput->backgroundRole(), Qt::white);
    p.setColor(expressionInput->foregroundRole(), Qt::black);
    expressionInput->setPalette(p);
}

GraphWindow::~GraphWindow()
{
    qDebug("GraphWindow::~GraphWindow()");
}

QSize GraphWindow::sizeHint() const
{
    return QSize(parentMdi->geometry().width() * 0.9, parentMdi->geometry().height() * 0.9);
}

void GraphWindow::savePixmapAsImage(QPixmap pixmap, QString filename)
{
    qDebug() << "GraphWindow::savePixmapAsImage(QPixmap &pixmap)";

    QString filepath = QFileDialog::getSaveFileName(
            this,
            tr("Zapisz jako obraz"),
            filename,
            tr("PNG (*.png);;JPG (*.jpg);;BMP (*.bmp)")
    );

    qDebug() << filepath;

    if(!filepath.isEmpty())
        pixmap.save(filepath);
}

void GraphWindow::setGraphWindowModified(bool modified, bool urAction)
{
    if(!urAction && lastSaveUndoStackIndex > undoStack->count())
    {
        qDebug() << "[saving usindex] lastSaveUndoStackIndex: " << lastSaveUndoStackIndex << "undoStack->count(): " << undoStack->count();
        lastSaveUndoStackIndex = -1;
    }
    qDebug() << "lastSaveUndoStackIndex: " << lastSaveUndoStackIndex << "undoStack->count(): " << undoStack->count();
    setWindowModified(modified);
    mainWindow->saveAct->setEnabled(modified);
    mainWindow->undoAct->setEnabled(undoStack->canUndo());
    mainWindow->redoAct->setEnabled(undoStack->canRedo());
}

void GraphWindow::showGraphList(bool show)
{
    functionsListView->setVisible(show);
    mainWindow->showGraphListAct->setChecked(show);
}

