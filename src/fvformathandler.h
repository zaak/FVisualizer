#ifndef FVFORMATHANDLER_H
#define FVFORMATHANDLER_H

#include <QDomDocument>
#include <QIODevice>
#include "function2d.h"
#include "function3d.h"

class FVFormatHandler
{
public:
    FVFormatHandler();
    static void elementXml(QDomDocument &doc, QDomElement& parent, QString attr, QString val);

    bool read(QIODevice* device);
    QString getGraphType();
    void saveGraph2d(QString& filepath, QVector<Function2d*>& functionsList, PlotSettings2d plotSettings);
    PlotSettings2d parsePlotsettings2d();
    QVector<Function2dInfo> parseFunctions2d();
    void saveGraph3d(QString& filepath, QVector<Function3d*>& functionsList, PlotSettings3d plotSettings);
    PlotSettings3d parsePlotsettings3d();
    QVector<Function3dInfo> parseFunctions3d();

private:
    QDomDocument domDocument;
};

#endif // FVFORMATHANDLER_H
