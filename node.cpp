#include "node.h"

Node::Node(Formula formula)
    :m_formula(formula)
{}

Formula Node::getFormula(){
    return m_formula;
}

Node::Node(){

}
