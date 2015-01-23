#ifndef FUNCTIONSLISTVIEW_H
#define FUNCTIONSLISTVIEW_H

#include <QListView>
#include <QStandardItemModel>
class FVMainWindow;

class FunctionsListView : public QListView
{
    Q_OBJECT

public:
    QStandardItemModel *functionsListItemModel;

public:
    FunctionsListView(FVMainWindow *mw);
    void updateFunctionAtIndex(int idx, QString expression, QColor color, bool checked = true);
    void contextMenuEvent(QContextMenuEvent *event);
    void addFunction(QString expression, QColor color, bool active = true);
    void addFunction(int index, QString expression, QColor color, bool active = true);
    void removeFunction(int index);

    QSize sizeHint() const;

    bool w3d;

private:
    FVMainWindow *mw;
};


#endif // FUNCTIONSLISTVIEW_H
