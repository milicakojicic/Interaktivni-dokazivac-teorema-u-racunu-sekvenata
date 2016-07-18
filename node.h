#ifndef NODE_H
#define NODE_H
#include "fol.hpp"
#include <QVector>

class Node
{
private:
    Formula m_formula;
    QVector <Formula> m_assumption;
public:
    Node(Formula formula, QVector <Formula> assumption);
    Node();
    Formula getFormula();
    QVector <Formula> getAssumption();

};

#endif // NODE_H
