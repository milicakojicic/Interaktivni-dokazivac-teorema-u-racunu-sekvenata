#include "node.h"

Node::Node(Formula formula, QVector<Formula> assumption)
    :m_formula(formula), m_assumption(assumption)
{
}

Formula Node::getFormula(){
    return m_formula;
}

QVector <Formula> Node::getAssumption() {
    return m_assumption;
}


Node::Node(){

}
