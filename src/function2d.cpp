#include "function2d.h"
#include <cmath>
#include "plotter2d.h"
#include "qdebug.h"
#include "fvformathandler.h"

Function2d::Function2d()
{
    init();
}

Function2d::Function2d(QString expression)
{
    init();
    this->expression = expression;
    evaluator.setExpression(prepareExpression(expression).toStdString());
}


Function2d::Function2d(Function2dInfo& fi)
{
    pointsCount = fi.pointsCount;
    drawInInterval = fi.drawInInterval;
    curveData = NULL;
    curveData = new QPointF[pointsCount];
    intervalFrom = fi.intervalFrom;
    intervalTo = fi.intervalTo;
    color = fi.color;
    active = fi.active;
    expression = fi.expression;
    evaluator.setExpression(prepareExpression(expression).toStdString());
}

void Function2d::init()
{
    pointsCount = 1000;
    curveData = NULL;
    drawInInterval = false;
    curveData = new QPointF[pointsCount];
    intervalFrom = -10.0;
    intervalTo = 10.0;
    color = QColor(qrand());
}

Function2d::~Function2d()
{
    if(curveData != NULL)
        delete[] curveData;
}

void Function2d::recalculate(const PlotSettings2d& settings)
{
    if(!curveData) {
        return;
        qDebug() << "Function2d::recalculate: curveData is NULL";
    }

    double min, max;

    if(drawInInterval) {
        min = intervalFrom;
        max = intervalTo;
    } else {
        min = settings.minX;
        max = settings.maxX;
    }

    double step = fabs(min - max) / pointsCount;

    double d = min;
    for(int i = 0; i < pointsCount; ++i)
    {
        d += step;
        evaluator.setVariable("x", d);
        curveData[i] = QPointF(d, evaluator.getValue());
    }
}

QString Function2d::getDerivative()
{
    QString s(getNodeDerivative(evaluator.getRootNode()).c_str());

    qDebug() << "s: " << s;

    QString st;

    while ((st = optimizeExpression(s)) != s)
       s = st;

    return s;
}

QString Function2d::optimizeExpression(QString s)
{
    s.replace(QRegExp("([(+-*/^])\\((\\d+(\\.\\d+)?)\\)"), "\\1\\2"); // niepotrzebne nawiasy dookoła liczb
    s.replace(QRegExp("\\((\\d+(\\.\\d+)?)\\([)+-*/^]))"), "\\1\\2");
    s.replace(QRegExp("^\\((\\d+(\\.\\d+)?)\\)"), "\\1");
    s.replace(QRegExp("\\((\\d+(\\.\\d+)?)\\$)"), "\\1");
    s.replace(QRegExp("([(+-*/^])\\((x)\\)"), "\\1\\2"); // j.w. dla "x"
    s.replace(QRegExp("\\(\\(([^()]+)\\)\\)"), "(\\1)"); // ((...)) na (...)
    s.replace(QRegExp("([(+-*/])(1\\*)"), "\\1"); // 1*
    s.remove(QRegExp("^1\\*"));
    s.replace(QRegExp("(\\*1)([)+-*/])"), "\\2"); // *1
    s.remove(QRegExp("\\*1$"));
    s.replace(QRegExp("(\\^1)([)+-*/])"), "\\2"); // ^1

    removeOutBrackets(s);

    qDebug() << "> opt: " << s;

    return s;
}


// dla skrócenia kodu
#define d(n) getNodeDerivative(n)

string Function2d::getNodeDerivative(Node *n)
{
    if(n->getOperator() != NULL)
    {
        string op = n->getOperator()->getSymbol();

        if(n->getOperator()->getType() == Operator::BINARY)
        {
            Node* R = n->getRight();
            Node* L = n->getLeft();
            string& Rs = R->getNString();
            string& Ls = L->getNString();

            if(L->hasConstant()) { // stała
                    if(op == "+") return "(" + d(R) + ")";
                    if(op == "-") return "(-(" + d(R) + "))";
                    if(op == "*") return "(" + Ls + ")*(" + d(R) + ")";
                    if(op == "/") return "((-" + Ls + ")*(" + d(R) + "))/(" + Rs + ")^2";
                    if(op == "^") return "(" + d(R) + ")*(" + Ls + ")^(" + Rs + ")*ln(" + Ls + ")";
            }
            else if(R->hasConstant())
            {
                    if(op == "+") return "(" + d(L) + ")";
                    if(op == "-") return "(" + d(L) + ")";
                    if(op == "*") return "(" + d(L) + ")*(" + Rs + ")";
                    if(op == "/") return "(" + d(L) + ")/(" + Rs + ")";
                    if(op == "^")
                    {
                        QString s = QString::number(R->getNodeValue()-1);
                        return Rs + "*(" + Ls + ")^(" + s.toStdString() + ")*(" + d(L) + ")";
                    }
            }
            else
            {
                    if(op == "+")    return "(" + d(L) + ")+(" + d(R) + ")";
                    if(op == "-")    return "(" + d(L) + ")-(" + d(R) + ")";
                    if(op == "*")    return "(" + Ls + ")*(" + d(R) + ")+(" + Rs + ")*(" + d(L) + ")";
                    if(op == "/")    return "((" + Rs + ")*(" + d(L) + ")-(" + Ls + ")*(" + d(R) + "))/(" + Rs + ")^2";
                    if(op == "^")    return "((" + Rs + ")*(" + Ls + ")^(" + Rs + "-1)*(" + d(L) + ")+(" + Ls + ")^(" + Rs + ")*ln(" + Ls + ") * (" + d(R) + "))";
            }

        } else { //Operator::UNARY
            Node* L = n->getLeft();
            string& Ls = L->getNString();

            if(op == "sin")     return "cos(" + Ls + ")*(" + d(L) + ")";
            if(op == "cos")     return "(-sin(" + Ls + "))*(" + d(L) + ")";
            if(op == "tan")     return "(1/cos(" + Ls + ")^2)*(" + d(L) + ")";
            if(op == "ctan")    return "(-1/sin(" + Ls + ")^2)*(" + d(L) + ")";
            if(op == "hsin")    return "hcos(" + Ls + ")*(" + d(L) + ")";
            if(op == "hcos")    return "hsin(" + Ls + ")*(" + d(L) + ")";
            if(op == "htan")    return "(1/hcos(" + Ls + ")^2)*(" + d(L) + ")";
            if(op == "hctan")   return "(-1/hsin(" + Ls + ")^2)*(" + d(L) + ")";
            if(op == "asin")    return "(1/sqrt(1-(" + Ls + ")^2))*(" + d(L) + ")";
            if(op == "acos")    return "(-1/sqrt(1-(" + Ls + ")^2))*(" + d(L) + ")";
            if(op == "atan")    return "(1/(1+(" + Ls + ")^2))*(" + d(L) + ")";
            if(op == "actan")   return "(-1/(1+(" + Ls + ")^2))*(" + d(L) + ")";
            if(op == "sqrt")    return "(1/(2*sqrt(" + Ls + ")))*(" + d(L) + ")";
            if(op == "ln")      return "(1/(" + Ls + "))*(" + d(L) + ")";
            if(op == "log")     return "(1/(" + Ls + "*ln(10)))*(" + d(L) + ")";
            if(op == "exp")     return "exp(" + Ls + ")*(" + d(L) +")";
////            if(op == "floor")   return floor(v1);
////            if(op == "ceil")    return ceil(v1);
//            if(op == "abs")     return abs(v1);
        }
    }

    string& Ns = n->getNString();

    if(Ns == "x" || Ns == "0+x") return "1";
    if(Ns == "0-x") return "(-1)";
    if(n->hasConstant()) return "0";

    return "err";
}

QDomElement Function2d::toXml(QDomDocument &doc)
{
    QDomElement function2dElem = doc.createElement("function2d");

    FVFormatHandler::elementXml(doc, function2dElem, QString("expression"), expression);
    FVFormatHandler::elementXml(doc, function2dElem, QString("color"), color.name());
    FVFormatHandler::elementXml(doc, function2dElem, QString("active"), active ? QString("yes") : QString("no"));
    FVFormatHandler::elementXml(doc, function2dElem, QString("drawInInterval"), drawInInterval ? QString("yes") : QString("no"));

    if(drawInInterval)
    {
        FVFormatHandler::elementXml(doc, function2dElem, QString("intervalFrom"), QString::number(intervalFrom));
        FVFormatHandler::elementXml(doc, function2dElem, QString("intervalTo"), QString::number(intervalTo));
    }

    return function2dElem;
}

Function2dInfo::Function2dInfo()
    : active(true), pointsCount(1000), drawInInterval(false),
      intervalFrom(-10), intervalTo(10) {};

Function2dInfo::Function2dInfo(Function2d* f)
{
    expression = f->expression;
    color = QColor(f->color);
    active = f->active;
    pointsCount = f->pointsCount;
    drawInInterval = f->drawInInterval;
    intervalFrom = f->intervalFrom;
    intervalTo = f->intervalTo;
}

bool Function2dInfo::hasValidInfo()
{
    if(drawInInterval && intervalFrom > intervalTo) return false;

    try {
        Function2d testFunction(expression);
        //testFunction.evaluator.setExpression(expression.toStdString());
        testFunction.recalculate(PlotSettings2d());
    }
    catch (ParsingException)
    {
        return false;
    };

    return true;
}
