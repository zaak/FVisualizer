#include "function3d.h"
#include "plotter3d.h"
#include "qdebug.h"
#include "fvformathandler.h"


class PlotSettings3d;

Function3d::Function3d()
{
    init();
}

Function3d::Function3d(QString expression)
{
    init();
    this->expression = expression;
    evaluator.setExpression(prepareExpression(expression).toStdString());
}

Function3d::Function3d(Function3dInfo& fi)
{
    pointsXCount = fi.pointsXCount;
    pointsYCount = fi.pointsYCount;
    curvatureData = new QVector3D*[pointsXCount];
    for(int i = 0; i < pointsXCount; ++i)
        curvatureData[i] = new QVector3D[pointsYCount];

    drawInInterval = fi.drawInInterval;
    intervalXFrom = fi.intervalXFrom;
    intervalXTo = fi.intervalXTo;
    intervalYFrom = fi.intervalYFrom;
    intervalYTo = fi.intervalYTo;
    color = fi.color;
    active = fi.active;
    drawingMethod = fi.drawingMethod;
    expression = fi.expression;
    evaluator.setExpression(prepareExpression(expression).toStdString());
}

void Function3d::init()
{
    pointsXCount = pointsYCount = 100;
    curvatureData = NULL;
    curvatureData = new QVector3D*[pointsXCount];
    for(int i = 0; i < pointsXCount; ++i)
        curvatureData[i] = new QVector3D[pointsYCount];
    drawInInterval = false;
    intervalXFrom = intervalYFrom = -10.0;
    intervalXTo = intervalYTo = 10.0;
    drawingMethod = GL_QUADS;

    color = QColor(qrand()*16);
}

Function3d::~Function3d()
{
    if(curvatureData != NULL)
    {
        for(int i = 0; i < pointsXCount; ++i)
            delete[] curvatureData[i];

        delete[] curvatureData;
    }

}

void Function3d::recalculate(PlotSettings3d plotSettings3d)
{
    if(!curvatureData) {
        return;
        qDebug() << "Function3d::recalculate: curvatureData is NULL";
    }

//    double graphMinX = -10, graphMinY = -10, graphMinZ = -10;
//    double graphMaxX = 10, graphMaxY = 10, graphMaxZ = 10;
//    double graphSpanX = (graphMaxX - graphMinX);// / pointsXCount;
//    double graphSpanY = (graphMaxY - graphMinY);// / pointsYCount;
//    double graphSpanZ = (graphMaxZ - graphMinZ);

    
    double minX, maxX, minY, maxY, spanX, spanY;

    if(drawInInterval)
    {
        minX = intervalXFrom;
        maxX = intervalXTo;
        minY = intervalYFrom;
        maxY = intervalYTo;
    }
    else {
        minX = plotSettings3d.minX;
        maxX = plotSettings3d.maxX;
        minY = plotSettings3d.minY;
        maxY = plotSettings3d.maxY;
    }

    spanX = fabs(minX - maxX);
    spanY = fabs(minY - maxY);

    double stepX = spanX / pointsXCount;
    double stepY = spanY / pointsYCount;

    //qDebug() << "stepx: " << stepX << ", stepY: " << stepY;

    double dx = minX, dy = minY;

    for(int x = 0; x < pointsXCount; ++x) {

        for(int y = 0; y < pointsYCount; ++y) {
            evaluator.setVariable("x", dx);
            evaluator.setVariable("y", dy);

            curvatureData[x][y] = QVector3D(dx, dy, evaluator.getValue());

            dy += stepY;
        }
        dx += stepX;
        dy = minY;
    }

    // Oblicz normalne
    //QVector3D *v1, *v2, *v3, *v4;

    if(!normals.empty())
        normals.clear();

    if(drawingMethod == GL_TRIANGLES)
        for(int x = 0; x < pointsXCount-1; ++x)
        {
            for(int y = 0; y < pointsYCount-1; ++y)
            {
                  normals.append(QVector3D::normal(curvatureData[x][y], curvatureData[x][y+1], curvatureData[x+1][y+1]));
                  normals.append(QVector3D::normal(curvatureData[x+1][y], curvatureData[x][y], curvatureData[x+1][y+1]));
            }
        }
    else
        for(int x = 0; x < pointsXCount-1; ++x)
        {
            for(int y = 0; y < pointsYCount-1; ++y)
            {
                  normals.append(QVector3D::normal(curvatureData[x][y], curvatureData[x+1][y], curvatureData[x][y+1]));
            }
        }
}

QDomElement Function3d::toXml(QDomDocument &doc)
{
    QDomElement function3dElem = doc.createElement("function3d");

    FVFormatHandler::elementXml(doc, function3dElem, QString("expression"), expression);
    FVFormatHandler::elementXml(doc, function3dElem, QString("color"), color.name());
    FVFormatHandler::elementXml(doc, function3dElem, QString("active"), active ? QString("yes") : QString("no"));
    FVFormatHandler::elementXml(doc, function3dElem, QString("drawInInterval"), drawInInterval ? QString("yes") : QString("no"));

    if(drawInInterval)
    {
        FVFormatHandler::elementXml(doc, function3dElem, QString("intervalXFrom"), QString::number(intervalXFrom));
        FVFormatHandler::elementXml(doc, function3dElem, QString("intervalXTo"), QString::number(intervalXTo));
        FVFormatHandler::elementXml(doc, function3dElem, QString("intervalYFrom"), QString::number(intervalYFrom));
        FVFormatHandler::elementXml(doc, function3dElem, QString("intervalYTo"), QString::number(intervalYTo));
    }

    return function3dElem;
}

Function3dInfo::Function3dInfo()
    : active(true), pointsXCount(100), pointsYCount(100), drawingMethod(GL_QUADS), drawInInterval(false),
      intervalXFrom(-10), intervalXTo(10), intervalYFrom(-10), intervalYTo(10) {};

Function3dInfo::Function3dInfo(Function3d* f)
{
    expression = f->expression;
    color = QColor(f->color);
    active = f->active;
    pointsXCount = f->pointsXCount;
    pointsYCount = f->pointsYCount;
    drawInInterval = f->drawInInterval;
    intervalXFrom = f->intervalXFrom;
    intervalXTo = f->intervalXTo;
    intervalYFrom = f->intervalYFrom;
    intervalYTo = f->intervalYTo;
    drawingMethod = f->drawingMethod;
}

bool Function3dInfo::hasValidInfo()
{
    if(drawInInterval && (intervalXFrom > intervalXTo || intervalYFrom > intervalYTo))
        return false;

    try {
        Function3d testFunction(expression);
        //testFunction.evaluator.setExpression(expression.toStdString());
        testFunction.recalculate(PlotSettings3d());
    }
    catch (ParsingException)
    {
        return false;
    };

    return true;
}
