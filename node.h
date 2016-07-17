#ifndef NODE_H
#define NODE_H
#include "fol.hpp"

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
