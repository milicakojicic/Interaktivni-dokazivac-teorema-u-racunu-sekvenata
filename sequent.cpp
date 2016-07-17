#include "sequent.h"
#include <sstream>
#include <qdebug.h>

Sequent::Sequent(QVector<Node> left, QVector<Node> right)
{
    this->m_left = left;
    this->m_right = right;
}

QVector<Node> Sequent::getLeft(){
    return this->m_left;
}

QVector<Node> Sequent::getRight(){
    return this->m_right;
}

void Sequent::print_sequent(){
    int i;
    int n = getLeft().length();
    int n1 = getRight().length();
    std::ostringstream stream;

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

    if(n1 == 1){
        for (i = 0; i < n1; i++){
            getRight()[i].getFormula()->printFormula(stream);
        }
    }
    else if(n1 == 2){
            getLeft()[0].getFormula()->printFormula(stream);
            stream << ',';
            getLeft()[1].getFormula()->printFormula(stream);
    }
    else if (n1 > 2){
        for (i = 0; i <= n1-2; i++){
            getLeft()[i].getFormula()->printFormula(stream);
            stream << ", ";
        }
        getLeft()[n1-1].getFormula()->printFormula(stream);
    }


    stream <<  getRight()[n1-1].getFormula()->getType();

    string formulaStream = stream.str();
    formulaStream += " ;";
    qDebug() << QString::fromStdString(formulaStream);

}
