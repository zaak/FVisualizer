#ifndef FUNCTION_H
#define FUNCTION_H

#include "ExpressionEvaluator.h"
#include <QString>
#include <QColor>
#include <QDomElement>

class Function
{
public:
    Function();

    QString expression;
    QColor color;
    ExpressionEvaluator evaluator;
    bool active;

    void setExpression(QString s);

protected:
    QString prepareExpression(QString s);
    void removeOutBrackets(QString s);
    bool bracketsOk(QString s);
};

#endif // FUNCTION_H
