#ifndef SEQUENT_H
#define SEQUENT_H

#include <QVector>
#include "node.h"

class Sequent
{
private:
    QVector<Node> m_left;
    QVector<Node> m_right;
    int m_id;
    int m_parent_id;

public:
    Sequent(QVector<Node> left,QVector<Node> right, int id, int parent_id);
    Sequent();
    QString print_sequent();
    QVector<Node> getLeft();
    QVector<Node> getRight();
    int getId();
    int getParentId();
};

#endif // SEQUENT_H
