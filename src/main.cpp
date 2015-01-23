#include <QApplication>
#include <QtCore>
#include "fvmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QTranslator qt;
    qDebug() << "Translator load: " << qt.load(":/translations/qt_pl.qm");
    app.installTranslator(&qt);


    FVMainWindow mainWin;
    mainWin.show();
    return app.exec();
}

