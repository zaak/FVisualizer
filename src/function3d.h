#ifndef FUNCTION3D_H
#define FUNCTION3D_H

#include <QVector3D>
#include "function.h"
#include <QVector>
#include <QDebug>

class PlotSettings3d;
class Function3dInfo;

// Klasa funkcji 3d - funkcji dwóch zmiennych
class Function3d : public Function
{
public:
    Function3d();
    Function3d(QString expression);
    Function3d(Function3dInfo& fi);
    ~Function3d();

    QVector3D** curvatureData;
    QVector<QVector3D> normals;
    int pointsXCount;
    int pointsYCount;

    void recalculate(PlotSettings3d plotSettings);
    QDomElement toXml(QDomDocument& doc);

    bool drawInInterval;
    double intervalXFrom;
    double intervalXTo;
    double intervalYFrom;
    double intervalYTo;

    int drawingMethod;

private:
    void init();
};

class Function3dInfo
{
public:
    Function3dInfo();
    Function3dInfo(Function3d* f);
    bool hasValidInfo();

    QString expression;
    QColor color;
    bool active;
    int pointsXCount;
    int pointsYCount;
    bool drawInInterval;
    double intervalXFrom;
    double intervalXTo;
    double intervalYFrom;
    double intervalYTo;
    int drawingMethod;
};

class Ray
{
public:
	QVector3D beginPoint;
	QVector3D endPoint;
	QColor color;
};

#endif // FUNCTION3D_H
