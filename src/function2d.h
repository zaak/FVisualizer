#ifndef FUNCTION2D_H
#define FUNCTION2D_H

#include <QPointF>
#include "function.h"

class PlotSettings2d;
class Function2dInfo;

// Klasa funkcji 2d
class Function2d : public Function
{
public:
    Function2d(QString expresion);
    Function2d(Function2dInfo& fi);
    Function2d();
    ~Function2d();

    QPointF* curveData;
    int pointsCount;

    void recalculate(const PlotSettings2d& settings);
    QString getDerivative();
    QString optimizeExpression(QString s);
    Function2dInfo getFunctionInfo();
    QDomElement toXml(QDomDocument& doc);

    bool drawInInterval;
    double intervalFrom;
    double intervalTo;

private:
    string getNodeDerivative(Node* n);
    void init();
};

class Function2dInfo
{
public:
    Function2dInfo();
    Function2dInfo(Function2d* f);
    bool hasValidInfo();

    QString expression;
    QColor color;
    bool active;
    int pointsCount;
    bool drawInInterval;
    double intervalFrom;
    double intervalTo;
};

#endif // FUNCTION2D_H

