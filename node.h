#ifndef NODE_H
#define NODE_H
#include "fol.hpp"
#include <QVector>

class Node
{
private:
    Formula m_formula;
public:
    Node(Formula formula);
    Node();
    Formula getFormula();

};

#endif // NODE_H
