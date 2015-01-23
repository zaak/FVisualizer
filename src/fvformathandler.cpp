#include "fvformathandler.h"
#include <QMessageBox>
#include "qdebug.h"
#include <QFile>
#include "plotter2d.h"
#include "plotter3d.h"

FVFormatHandler::FVFormatHandler()
{
}

void FVFormatHandler::elementXml(QDomDocument& doc, QDomElement& parent, QString attr, QString val)
{
    QDomElement attrElement = doc.createElement(attr);
    parent.appendChild(attrElement);
    QDomText attrText = doc.createTextNode(val);
    attrElement.appendChild(attrText);
}

bool FVFormatHandler::read(QIODevice* device)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!domDocument.setContent(device, true, &errorStr, &errorLine,
                                &errorColumn)) {
        QMessageBox::critical(0, "FVisualizer",
                                 QString("Błąd parsowania pliku (linia %1, kolumna %2: \n%3)")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        return false;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "fvisualizer" || !root.hasAttribute("graphtype") ||
        root.attribute("graphtype") != "2d" && root.attribute("graphtype") != "3d") {
        QMessageBox::critical(0, "Fvisualizer",
                                 "Wskazany plik nie jest plikiem programu FVisualizer.");
        return false;
    }

    return true;
}

QString FVFormatHandler::getGraphType()
{
    return domDocument.documentElement().attribute("graphtype");
}

PlotSettings2d FVFormatHandler::parsePlotsettings2d()
{
    QDomElement plotSettings2d = domDocument.documentElement().firstChildElement("plotsettings2d");

    PlotSettings2d p;

    p.minX = plotSettings2d.firstChildElement("minX").text().trimmed().toDouble();
    p.maxX = plotSettings2d.firstChildElement("maxX").text().trimmed().toDouble();
    p.minY = plotSettings2d.firstChildElement("minY").text().trimmed().toDouble();
    p.maxY = plotSettings2d.firstChildElement("maxY").text().trimmed().toDouble();
    p.showAxes = plotSettings2d.firstChildElement("showAxes").text().trimmed() == "yes";
    if(p.showAxes)
        p.axesColor = QColor(plotSettings2d.firstChildElement("axesColor").text().trimmed());
    p.showLabels = plotSettings2d.firstChildElement("showLabels").text().trimmed() == "yes";
    if(p.showLabels)
        p.labelsColor = QColor(plotSettings2d.firstChildElement("labelsColor").text().trimmed());
    p.showGrid = plotSettings2d.firstChildElement("showGrid").text().trimmed() == "yes";
    if(p.showGrid)
        p.gridColor = QColor(plotSettings2d.firstChildElement("gridColor").text().trimmed());
    p.bgColor = QColor(plotSettings2d.firstChildElement("bgColor").text().trimmed());
    p.showLegend = plotSettings2d.firstChildElement("showLegend").text().trimmed() == "yes";
    p.enableAntialiasing = plotSettings2d.firstChildElement("enableAntialiasing").text().trimmed() == "yes";

    return p;
}

QVector<Function2dInfo> FVFormatHandler::parseFunctions2d()
{
    QDomElement functions = domDocument.documentElement().firstChildElement("functions");
    QDomElement function = functions.firstChildElement("function2d");

    QVector<Function2dInfo> v;

    while(!function.isNull())
    {
        Function2dInfo fi;

        fi.expression = function.firstChildElement("expression").text().trimmed();
        fi.color = QColor(function.firstChildElement("color").text().trimmed());
        fi.active = function.firstChildElement("active").text().trimmed() == "yes";
        fi.drawInInterval = function.firstChildElement("drawInInterval").text().trimmed() == "yes";

        if(fi.drawInInterval)
        {
            fi.intervalFrom = function.firstChildElement("intervalFrom").text().trimmed().toDouble();
            fi.intervalTo = function.firstChildElement("intervalTo").text().trimmed().toDouble();
        }


        v.append(fi);

        function = function.nextSiblingElement("function2d");
    }

    return v;
}

PlotSettings3d FVFormatHandler::parsePlotsettings3d()
{
    QDomElement plotSettings3d = domDocument.documentElement().firstChildElement("plotsettings3d");

    PlotSettings3d p;

    p.minX = plotSettings3d.firstChildElement("minX").text().trimmed().toDouble();
    p.maxX = plotSettings3d.firstChildElement("maxX").text().trimmed().toDouble();
    p.minY = plotSettings3d.firstChildElement("minY").text().trimmed().toDouble();
    p.maxY = plotSettings3d.firstChildElement("maxY").text().trimmed().toDouble();
    p.minZ = plotSettings3d.firstChildElement("minZ").text().trimmed().toDouble();
    p.maxZ = plotSettings3d.firstChildElement("maxZ").text().trimmed().toDouble();

    p.showEdges = plotSettings3d.firstChildElement("showEdges").text().trimmed() == "yes";
    if(p.showEdges)
        p.edgesColor = QColor(plotSettings3d.firstChildElement("edgesColor").text().trimmed());
    p.showLabels = plotSettings3d.firstChildElement("showLabels").text().trimmed() == "yes";
    if(p.showLabels)
        p.labelsColor = QColor(plotSettings3d.firstChildElement("labelsColor").text().trimmed());
    p.bgColor =  QColor(plotSettings3d.firstChildElement("bgColor").text().trimmed());
    p.acwRotation = plotSettings3d.firstChildElement("acwRotation").text().trimmed() == "yes";
    p.showCSMiniature = plotSettings3d.firstChildElement("showCSMiniature").text().trimmed() == "yes";
    p.showDirectionsMarkers = plotSettings3d.firstChildElement("showDirectionsMarkers").text().trimmed() == "yes";
    p.showLegend = plotSettings3d.firstChildElement("showLegend").text().trimmed() == "yes";

    return p;
}

QVector<Function3dInfo> FVFormatHandler::parseFunctions3d()
{
    QDomElement functions = domDocument.documentElement().firstChildElement("functions");
    QDomElement function = functions.firstChildElement("function3d");

    QVector<Function3dInfo> v;

    while(!function.isNull())
    {
        Function3dInfo fi;

        fi.expression = function.firstChildElement("expression").text().trimmed();
        fi.color = QColor(function.firstChildElement("color").text().trimmed());
        fi.active = function.firstChildElement("active").text().trimmed() == "yes";
        fi.drawInInterval = function.firstChildElement("drawInInterval").text().trimmed() == "yes";

        if(fi.drawInInterval)
        {
            fi.intervalXFrom = function.firstChildElement("intervalXFrom").text().trimmed().toDouble();
            fi.intervalXTo = function.firstChildElement("intervalXTo").text().trimmed().toDouble();
            fi.intervalYFrom = function.firstChildElement("intervalYFrom").text().trimmed().toDouble();
            fi.intervalYTo = function.firstChildElement("intervalYTo").text().trimmed().toDouble();
        }

        v.append(fi);

        function = function.nextSiblingElement("function3d");
    }

    return v;
}

void FVFormatHandler::saveGraph2d(QString& filepath, QVector<Function2d*>& functionsList,
                                  PlotSettings2d plotSettings)
{
    domDocument = QDomDocument("fvisualizer");
    QDomNode xmlNode = domDocument.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    domDocument.insertBefore(xmlNode, domDocument.firstChild());

    QDomElement fvisualizerElem = domDocument.createElement("fvisualizer");
    fvisualizerElem.setAttribute("version", "1.0");
    fvisualizerElem.setAttribute("graphtype", "2d");
    domDocument.appendChild(fvisualizerElem);

    fvisualizerElem.appendChild(plotSettings.toXml(domDocument));

    QDomElement functionsElem = domDocument.createElement("functions");
    fvisualizerElem.appendChild(functionsElem);

    for(int i = 0; i < functionsList.size(); ++i)
    {
        functionsElem.appendChild(functionsList[i]->toXml(domDocument));
    }

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(0, "FVisualzer", "Błąd zapisu pliku.");
        return;
    }

    QTextStream out(&file);

    const int indentSize = 4;

    domDocument.save(out, indentSize);
}

void FVFormatHandler::saveGraph3d(QString& filepath, QVector<Function3d *>& functionsList,
                                  PlotSettings3d plotSettings)
{
    domDocument = QDomDocument ("fvisualizer");
    QDomNode xmlNode = domDocument.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    domDocument.insertBefore(xmlNode, domDocument.firstChild());

    QDomElement fvisualizerElem = domDocument.createElement("fvisualizer");
    fvisualizerElem.setAttribute("version", "1.0");
    fvisualizerElem.setAttribute("graphtype", "3d");
    domDocument.appendChild(fvisualizerElem);

    fvisualizerElem.appendChild(plotSettings.toXml(domDocument));

    QDomElement functionsElem = domDocument.createElement("functions");
    fvisualizerElem.appendChild(functionsElem);

    for(int i = 0; i < functionsList.size(); ++i)
    {
        functionsElem.appendChild(functionsList[i]->toXml(domDocument));
    }

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(0, "FVisualzer", "Błąd zapisu pliku.");
        return;
    }

    QTextStream out(&file);

    const int indentSize = 4;

    domDocument.save(out, indentSize);
}
