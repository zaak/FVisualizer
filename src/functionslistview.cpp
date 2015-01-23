#include "functionslistview.h"
#include <QDebug>
#include <QMouseEvent>
#include <QStyle>
#include <QMenu>
#include "fvmainwindow.h"
#include "function2d.h"

FunctionsListView::FunctionsListView(FVMainWindow *mw)
    : mw(mw), w3d(false)
{
    setSpacing(1); // Odstępy między funkcjami na liście
    functionsListItemModel = new QStandardItemModel(0, 1, this);
    functionsListItemModel->setHeaderData(0, Qt::Horizontal, tr("Lista wykresów"));
    setModel(functionsListItemModel);
}

QSize FunctionsListView::sizeHint() const
{
    return QSize(150, 0);
}

void FunctionsListView::addFunction(QString expression, QColor color, bool checked)
{
    QStandardItem *item = new QStandardItem(expression);
    item->setData(QVariant(Qt::Checked), Qt::CheckStateRole);
    item->setData(color, Qt::DecorationRole);
    item->setCheckable(true);
    item->setEditable(false);
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

    functionsListItemModel->appendRow(item);
}

void FunctionsListView::addFunction(int index, QString expression, QColor color, bool checked)
{
    QStandardItem *item = new QStandardItem(expression);
    item->setData(QVariant(Qt::Checked), Qt::CheckStateRole);
    item->setData(color, Qt::DecorationRole);
    item->setCheckable(true);
    item->setEditable(false);
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

    functionsListItemModel->insertRow(index, item);
}

void FunctionsListView::updateFunctionAtIndex(int idx, QString expression, QColor color, bool checked)
{
    QStandardItem *item = functionsListItemModel->item(idx, 0);
    item->setText(expression);
    item->setData(color, Qt::DecorationRole);
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
}

void FunctionsListView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;

    QModelIndex idx = indexAt(event->pos());
    if(idx.isValid()) // Menu kontekstowe wykresu (kliknięcie na wzorze funkcji - elemencie listy)
    {
        menu.addAction(mw->graphAddAct);
        menu.addAction(mw->graphEditAct);
        menu.addAction(mw->graphDeleteAct);

        if(!w3d)
        {
            menu.addSeparator();
            menu.addAction(mw->derivativeAct);
        }

    }
    else              // Menu kontekstowe listy funkcji
    {
        menu.addAction(mw->graphAddAct);
    }

    menu.exec(mapToGlobal(event->pos()));
}

void FunctionsListView::removeFunction(int index)
{
    functionsListItemModel->removeRow(index);
}
