#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <QMdiSubWindow>
#include <QtWidgets>
#include "ExpressionEvaluator.h"
#include <QDomDocument>

class FVMainWindow;
class QMdiSubWindow;
class FunctionsListView;

class GraphWindow : public QMdiSubWindow
{
    Q_OBJECT

public:
    GraphWindow(QWidget *parentMdi, FVMainWindow *mainWindow);
    ~GraphWindow();
    QSize sizeHint() const;

private:
    QWidget *parentMdi;

protected:
    FVMainWindow *mainWindow;
    QUndoStack* undoStack;
    void errorMsgBox(QString boxTitle, QString boxMessage);

    bool isUntitled;
    QString curFile;

    int lastSaveUndoStackIndex;

protected:
    // Wspólne elementy występujące w potomnych klasach okien wykresów 2d i 3d
    QSplitter *splitter;
    //QListView *functionsList; QStandardItemModel *functionsListItemModel;
    FunctionsListView *functionsListView;
    QWidget *expressionWidget;
    QHBoxLayout *expressionLayout;
    QLabel *expressionLabel;
    QLineEdit *expressionInput;
    QWidget *bgWidget;
    QVBoxLayout *bgLayout;

public:
    void savePixmapAsImage(QPixmap pixmap, QString filename);

    // Funkcja (slot) dodająca wykres funkcji do danego okna
protected slots:
    virtual void addLineEditFunction() = 0;
    void setGraphWindowModified(bool modified, bool urAction = false);

public slots:
    // Przybliż / oddal
    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;
    virtual void scrollLeft() = 0;
    virtual void scrollRight() = 0;
    virtual void scrollUp() = 0;
    virtual void scrollDown() = 0;

    virtual void graphAdd() = 0;
    virtual void graphEdit() = 0;
    virtual void graphDelete() = 0;

    virtual void undo() = 0;
    virtual void redo() = 0;

    virtual void save(bool saveAs = false) = 0;

    virtual void exportToImage() = 0;

    void showGraphList(bool show);

    virtual void graphSettings() = 0;

    virtual void updateActions() = 0;

signals:
    void windowModified(bool modified, bool urAction = false);

};

#endif // GRAPHWINDOW_H
