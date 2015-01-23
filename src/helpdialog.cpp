#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowMinMaxButtonsHint),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);

    helpIcon = QIcon(":/images/help.png");
    questionIcon = QIcon(":/images/question.png");

    setWindowIcon(helpIcon);
    setWindowTitle("Pomoc programu FVisualizer");

    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClicked(QTreeWidgetItem*,int)));

    setupHelpContent();
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void HelpDialog::setupHelpContent()
{
    helpContents.append(HelpContent(QString("1. Tworzenie wykresów funkcji"), QString(":/help/tworzenie.html"), -1)); // 0
    helpContents.append(HelpContent(QString("1.1. Dodawanie wykresu"), QString("dodawanie"), 0));
    helpContents.append(HelpContent(QString("1.2. Operatory"), QString("operatory"), 0));
    helpContents.append(HelpContent(QString("1.3. Funkcje"), QString("funkcje"), 0));
    helpContents.append(HelpContent(QString("1.4. Stałe"), QString("stale"), 0));

    helpContents.append(HelpContent(QString("2. Edycja i usuwanie wykresu"), QString(":/help/edycja.html"), -1));
    helpContents.append(HelpContent(QString("2.1 Edycja wykresu"), QString("edycja"), 5));
    helpContents.append(HelpContent(QString("2.2 Usuwanie wykresu"), QString("usuwanie"), 5));

    helpContents.append(HelpContent(QString("3. Ustawienia okna wykresu"), QString(":/help/ustawienia.html"), -1));
    helpContents.append(HelpContent(QString("3.1. Ustawienia wykresu 2d"), QString("ustawienia2d"), 8));
    helpContents.append(HelpContent(QString("3.2. Ustawienia wykresu 3d"), QString("ustawienia3d"), 8));

    // --------
    QTreeWidgetItem* w;
    for(int i = 0; i < helpContents.size(); ++i)
    {
        if(helpContents.at(i).parentIndex != -1)
        {
            w = new QTreeWidgetItem(helpContents.at(helpContents.at(i).parentIndex).item);
            w->setIcon(0, questionIcon);
        }
        else
        {
            w = new QTreeWidgetItem(ui->treeWidget);
            w->setExpanded(true);
            w->setIcon(0, helpIcon);
        }
        w->setText(0, helpContents.at(i).title);
        helpContents[i].item = w;
        mapItemIndex[w] = i;
    }

    if(!helpContents.empty())
    {
        ui->helpBrowser->setContents(helpContents.first().location);
        ui->treeWidget->setItemSelected(helpContents.first().item, true);
    }
}

void HelpDialog::itemClicked(QTreeWidgetItem* item,int)
{


    QMap<QTreeWidgetItem*,int>::const_iterator i = mapItemIndex.find(item);
    if(i != mapItemIndex.end())
    {
        HelpContent c = helpContents.at(i.value());
        if(c.parentIndex == -1)
        {
            ui->helpBrowser->setContents(c.location);
        }
        else
        {
            ui->helpBrowser->setContents(helpContents.at(c.parentIndex).location);
            ui->helpBrowser->scrollToAnchor(c.location);
        }
    }
}
