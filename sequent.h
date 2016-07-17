#ifndef SEQUENT_H
#define SEQUENT_H

#include <QVector>
#include "node.h"

class Sequent
{
private:
    QVector<Node> m_left;
    QVector<Node> m_right;

public:
    Sequent(QVector<Node> left,QVector<Node> right);
    void print_sequent();
    QVector<Node> getLeft();
    QVector<Node> getRight();
};

#endif // SEQUENT_H
