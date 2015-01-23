#include "function.h"

#include <QDebug>

Function::Function()
    : active(true)
{
}


QString Function::prepareExpression(QString s)
{
    s.replace(QRegExp("(\\d+(\\.\\d+)?)([xy])"), "\\1*\\3");
    //removeOutBrackets(s);
    qDebug() << "prepared expression: " << s;

    return s;
}

void Function::setExpression(QString s)
{
    expression = s;
    evaluator.setExpression(prepareExpression(s).toStdString());
}

void Function::removeOutBrackets(QString s)
{
    QString stmp;
    while((s.contains('(') || s.contains(')')) && bracketsOk(stmp = s.mid(1, s.length()-2)))
    {
        qDebug() << stmp;
        s = stmp;
    }
}

bool Function::bracketsOk(QString s)
{
    int inBrackets = 0;

    for(int i = 0; i < s.length(); ++i)
    {
            if(s[i] == '(' && inBrackets >= 0)
                    ++inBrackets;
            else if(s[i] == ')')
                    --inBrackets;
    }

    return inBrackets == 0;
}
