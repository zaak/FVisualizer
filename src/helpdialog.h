#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QVector>
#include <QMap>
#include <QIcon>

class QModelIndex;
class QTreeWidgetItem;
class HelpContent;

namespace Ui {
    class HelpDialog;
}

class HelpDialog : public QDialog {
    Q_OBJECT
public:
    HelpDialog(QWidget *parent = 0);
    ~HelpDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::HelpDialog *ui;
    void setContents(const QString &fileName);
    void setupHelpContent();
    QVector<HelpContent> helpContents;
    QMap<QTreeWidgetItem*,int> mapItemIndex;
    QIcon helpIcon;
    QIcon questionIcon;

private slots:
    void itemClicked(QTreeWidgetItem*,int);
};

class HelpContent {
public:
    HelpContent() {};
    HelpContent(QString title, QString location, int parentIndex)
        : title(title), location(location), parentIndex(parentIndex) {};
    QString title;
    QString location;
    int parentIndex;
    QTreeWidgetItem* item;
};

#endif // HELPDIALOG_H
