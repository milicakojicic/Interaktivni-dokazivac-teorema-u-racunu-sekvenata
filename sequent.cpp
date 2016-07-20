#include "sequent.h"
#include <sstream>
#include <qdebug.h>
#include <QString>


Sequent::Sequent(QVector<Node> left, QVector<Node> right, int id, int parent_id)
{
    this->m_left = left;
    this->m_right = right;
    this->m_id = id;
    this->m_parent_id = parent_id;
}

Sequent::Sequent () {

}

int Sequent::getId() {
    return m_id;
}

int Sequent::getParentId() {
    return m_parent_id;
}

QVector<Node> Sequent::getLeft(){
    return this->m_left;
}

QVector<Node> Sequent::getRight(){
    return this->m_right;
}

QString Sequent::print_sequent(){
    int i;
    int n = getLeft().length();
    int n1 = getRight().length();
    std::ostringstream stream;

    if (n == 0) {
        stream <<" ";
    }

    if(n == 1){
        for (i = 0; i < n; i++){
            getLeft()[i].getFormula()->printFormula(stream);
        }
    }
    else if(n == 2){
            getLeft()[0].getFormula()->printFormula(stream);
            stream << ',';
            getLeft()[1].getFormula()->printFormula(stream);
    }

    else if (n > 2){
        for (i = 0; i <= n-2; i++){
            getLeft()[i].getFormula()->printFormula(stream);
            stream << ", ";
        }
        getLeft()[n-1].getFormula()->printFormula(stream);
    }

    stream << "|- ";

    if (n1 == 0) {
        stream<< " ";
    }

    if(n1 == 1){
        for (i = 0; i < n1; i++){
            getRight()[i].getFormula()->printFormula(stream);
        }
    }
    else if(n1 == 2){
            getRight()[0].getFormula()->printFormula(stream);
            stream << ',';
            getRight()[1].getFormula()->printFormula(stream);
    }

    else if (n1 > 2){
        for (i = 0; i <= n1-2; i++){
            getRight()[i].getFormula()->printFormula(stream);
            stream << ", ";
        }
        getRight()[n1-1].getFormula()->printFormula(stream);
    }


    string formulaStream = stream.str();
    return QString::fromStdString(formulaStream);

}


