#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fol.hpp"
#include <QString>
#include <qdebug.h>
#include <sstream>
#include "sequent.h"

using namespace std;


typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern void yy_switch_to_buffer (YY_BUFFER_STATE new_buffer  );
extern Formula parsed_formula;
ostringstream stream;
QVector<int> roditeljiSaDvaDeteta = {};
QVector<Sequent> preostali = {};


/*niz svih sekvenata*/
QVector<Sequent> sequents= {};

/*trenutni sekvent*/
int current_sequent = 0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_potvrdi_clicked()
{

    ui->potvrdi->setDisabled(true);


    QString formula = ui->lineEdit->text();

    formula += ";";

    string novaFormula = formula.toUtf8().constData();

    YY_BUFFER_STATE buffer = yy_scan_string(novaFormula.c_str());
            if(yyparse() == 1){
        qDebug() << "Neispravna formula na ulazu";
    }

    ostringstream stream;
    parsed_formula->printFormula(stream);

    std::vector<Formula> tmp_vec = std::vector<Formula>();
    parsed_formula->getAtoms(tmp_vec);
    QVector<Formula> assumptions = QVector<Formula>::fromStdVector(tmp_vec);

    QVector<Node> right;
    right.append(Node(parsed_formula, assumptions));
    QVector<Node> left = {};

    Sequent sequent(left,right,0,0,false);

    sequents.append(sequent);

    /*inicijalizacija trenutnog sekventa*/
    current_sequent = sequent.getId();

    ui->textBrowser->append(sequent.print_sequent());
    qDebug() << current_sequent;


}

void MainWindow::on_andLeft_clicked()
{


    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;

    for (i=0; i<leftSequent.size(); i++) {

        if (leftSequent[i].getFormula()->getType() == 4) {
            Formula leftFormula = ((And*)(leftSequent[i].getFormula().get()))->getOperand1();
            Formula rightFormula = ((And*)(leftSequent[i].getFormula().get()))->getOperand2();


            std::vector<Formula> tmp_vec_left = std::vector<Formula>();
            leftFormula->getAtoms(tmp_vec_left);
            QVector<Formula> leftAssumptions = QVector<Formula>::fromStdVector(tmp_vec_left);

            std::vector<Formula> tmp_vec_right = std::vector<Formula>();
            rightFormula->getAtoms(tmp_vec_right);
            QVector<Formula> rightAssumptions = QVector<Formula>::fromStdVector(tmp_vec_right);

            Node leftNode(leftFormula, leftAssumptions);
            Node rightNode(rightFormula, rightAssumptions);

            int noviId = sequent.getId() + 1;

            QVector<Node> newLeftSequent = leftSequent;
            newLeftSequent.remove(i);
            newLeftSequent.append(leftNode);
            newLeftSequent.append(rightNode);

            Sequent newSequent(newLeftSequent, rightSequent, noviId, sequent.getId(), false);
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }
            ui->textBrowser->append(newSequent.print_sequent());

            sequents.append(newSequent);

            current_sequent = newSequent.getId();

            qDebug() << "Primenjeno pravilo andLeft";
            qDebug() << current_sequent;


        }

    }

}

void MainWindow::on_impRight_clicked()
{

    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;

    for (i=0; i<rightSequent.size(); i++) {

        if (rightSequent[i].getFormula()->getType() == 6) {
            Formula leftFormula = ((Imp*)(rightSequent[i].getFormula().get()))->getOperand1();
            Formula rightFormula = ((Imp*)(rightSequent[i].getFormula().get()))->getOperand2();

            std::vector<Formula> tmp_vec_left = std::vector<Formula>();
            leftFormula->getAtoms(tmp_vec_left);
            QVector<Formula> leftAssumptions = QVector<Formula>::fromStdVector(tmp_vec_left);

            std::vector<Formula> tmp_vec_right = std::vector<Formula>();
            rightFormula->getAtoms(tmp_vec_right);
            QVector<Formula> rightAssumptions = QVector<Formula>::fromStdVector(tmp_vec_right);


            Node leftNode(leftFormula, leftAssumptions);
            Node rigthNode(rightFormula, rightAssumptions);


            QVector<Node> newRightSequent = rightSequent;
            newRightSequent.replace(i, rigthNode);

            QVector<Node> newLeftSequent = leftSequent;
            newLeftSequent.append(leftNode);

            Sequent newSequent(newLeftSequent,newRightSequent, sequent.getId() + 1, sequent.getId(), false);
            ui->textBrowser->clear();

            preostali = nadjiPreostale();

            if(!preostali.empty()){
               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }

            ui->textBrowser->append(newSequent.print_sequent());

            sequents.append(newSequent);

            current_sequent = newSequent.getId();
            qDebug() << "Primenjeno pravilo impRight";
            qDebug() << current_sequent;


        }

    }
}

void MainWindow::on_impLeft_clicked()
{
    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;

    for (i=0; i<leftSequent.size(); i++) {

        if (leftSequent[i].getFormula()->getType() == 6) {
            Formula leftFormula = ((Imp*)(leftSequent[i].getFormula().get()))->getOperand1();
            Formula rightFormula = ((Imp*)(leftSequent[i].getFormula().get()))->getOperand2();

            std::vector<Formula> tmp_vec_left = std::vector<Formula>();
            leftFormula->getAtoms(tmp_vec_left);
            QVector<Formula> leftAssumptions = QVector<Formula>::fromStdVector(tmp_vec_left);

            std::vector<Formula> tmp_vec_right = std::vector<Formula>();
            rightFormula->getAtoms(tmp_vec_right);
            QVector<Formula> rightAssumptions = QVector<Formula>::fromStdVector(tmp_vec_right);

            Node leftNode(leftFormula, leftAssumptions);
            Node rightNode(rightFormula, rightAssumptions);

            int noviId = sequent.getId() + 1;

            QVector<Node> newLeftSequent = leftSequent;
            newLeftSequent.remove(i);

            QVector<Node> newRightSequent = rightSequent;
            newRightSequent.push_back(leftNode);

            Sequent newSequent1(newLeftSequent, newRightSequent, noviId, sequent.getId(), false);
            ui->textBrowser->append(newSequent1.print_sequent());

            sequents.append(newSequent1);

            noviId++;
            QVector<Node> newLeftSequent1 = leftSequent;
            newLeftSequent1.remove(i);
            newLeftSequent1.append(rightNode);

            QVector<Node> newRightSequent1 = rightSequent;

            Sequent newSequent2(newLeftSequent1, newRightSequent1, noviId, sequent.getId(), false);
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }
            ui->textBrowser->append(newSequent2.print_sequent());

            sequents.append(newSequent2);


            current_sequent = newSequent1.getId();
            qDebug() << "Primenjeno pravilo impLeft";
            qDebug() << current_sequent;

            roditeljiSaDvaDeteta.push_back(sequent.getId());

        }


    }



}

void MainWindow::on_rightAnd_clicked()
{
    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;

    for (i=0; i<rightSequent.size(); i++) {

        if (rightSequent[i].getFormula()->getType() == 4) {
            Formula leftFormula = ((And*)(rightSequent[i].getFormula().get()))->getOperand1();
            Formula rightFormula = ((And*)(rightSequent[i].getFormula().get()))->getOperand2();

            std::vector<Formula> tmp_vec_left = std::vector<Formula>();
            leftFormula->getAtoms(tmp_vec_left);
            QVector<Formula> leftAssumptions = QVector<Formula>::fromStdVector(tmp_vec_left);

            std::vector<Formula> tmp_vec_right = std::vector<Formula>();
            rightFormula->getAtoms(tmp_vec_right);
            QVector<Formula> rightAssumptions = QVector<Formula>::fromStdVector(tmp_vec_right);


            Node leftNode(leftFormula, leftAssumptions);
            Node rightNode(rightFormula, rightAssumptions);


            QVector<Node> newRightSequent1 = rightSequent;
            newRightSequent1.remove(i);
            newRightSequent1.append(rightNode);


            QVector<Node> newRightSequent2 = rightSequent;
            newRightSequent2.remove(i);
            newRightSequent2.append(leftNode);

            Sequent newSequent1(leftSequent,newRightSequent1, sequent.getId() + 1, sequent.getId(), false);
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }

            ui->textBrowser->append(newSequent1.print_sequent());

            Sequent newSequent2(leftSequent,newRightSequent2, sequent.getId() + 2, sequent.getId(), false);
            ui->textBrowser->append(newSequent2.print_sequent());


            sequents.append(newSequent1);
            sequents.append(newSequent2);


            current_sequent = newSequent1.getId();
            qDebug() << "Primenjeno pravilo andRight";
            qDebug() << current_sequent;

            roditeljiSaDvaDeteta.push_back(sequent.getId());

        }

    }
}

void MainWindow::on_orLeft_clicked()
{

    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;

    for (i=0; i<leftSequent.size(); i++) {

        if (leftSequent[i].getFormula()->getType() == 5) {
            Formula leftFormula = ((Or*)(leftSequent[i].getFormula().get()))->getOperand1();
            Formula rightFormula = ((Or*)(leftSequent[i].getFormula().get()))->getOperand2();

            std::vector<Formula> tmp_vec_left = std::vector<Formula>();
            leftFormula->getAtoms(tmp_vec_left);
            QVector<Formula> leftAssumptions = QVector<Formula>::fromStdVector(tmp_vec_left);

            std::vector<Formula> tmp_vec_right = std::vector<Formula>();
            rightFormula->getAtoms(tmp_vec_right);
            QVector<Formula> rightAssumptions = QVector<Formula>::fromStdVector(tmp_vec_right);


            Node leftNode(leftFormula, leftAssumptions);
            Node rightNode(rightFormula, rightAssumptions);


            QVector<Node> newLeftSequent1 = leftSequent;
            newLeftSequent1.remove(i);
            newLeftSequent1.append(leftNode);


            QVector<Node> newLeftSequent2 = leftSequent;
            newLeftSequent2.remove(i);
            newLeftSequent2.append(rightNode);

            Sequent newSequent1(newLeftSequent1,rightSequent, sequent.getId() + 1, sequent.getId(), false);
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }
            ui->textBrowser->append(newSequent1.print_sequent());

            Sequent newSequent2(newLeftSequent2,rightSequent, sequent.getId() + 2, sequent.getId(), false);
            ui->textBrowser->append(newSequent2.print_sequent());

            sequents.append(newSequent1);
            sequents.append(newSequent2);


            current_sequent = newSequent1.getId();
            qDebug() << "Primenjeno pravilo orLeft";
            qDebug() << current_sequent;

            roditeljiSaDvaDeteta.push_back(sequent.getId());

        }

    }


}

void MainWindow::on_orRight_clicked()
{

    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;

    for (i=0; i<rightSequent.size(); i++) {

        if (rightSequent[i].getFormula()->getType() == 5) {
            Formula leftFormula = ((Or*)(rightSequent[i].getFormula().get()))->getOperand1();
            Formula rightFormula = ((Or*)(rightSequent[i].getFormula().get()))->getOperand2();


            std::vector<Formula> tmp_vec_left = std::vector<Formula>();
            leftFormula->getAtoms(tmp_vec_left);
            QVector<Formula> leftAssumptions = QVector<Formula>::fromStdVector(tmp_vec_left);

            std::vector<Formula> tmp_vec_right = std::vector<Formula>();
            rightFormula->getAtoms(tmp_vec_right);
            QVector<Formula> rightAssumptions = QVector<Formula>::fromStdVector(tmp_vec_right);

            Node leftNode(leftFormula, leftAssumptions);
            Node rightNode(rightFormula, rightAssumptions);

            int noviId = sequent.getId() + 1;

            QVector<Node> newRightSequent = rightSequent;
            newRightSequent.remove(i);
            newRightSequent.append(leftNode);
            newRightSequent.append(rightNode);

            Sequent newSequent(leftSequent, newRightSequent, noviId, sequent.getId(), false);
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }
            ui->textBrowser->append(newSequent.print_sequent());

            sequents.append(newSequent);

            current_sequent = newSequent.getId();
            qDebug() << "Primenjeno pravilo orRight";
            qDebug() << current_sequent;

        }

    }

}

void MainWindow::on_notLeft_clicked()
{
    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;

    for (i=0; i<leftSequent.size(); i++) {

        if (leftSequent[i].getFormula()->getType() == 3) {
            Formula formula = ((Not*)(leftSequent[i].getFormula().get()))->getOperand();


            std::vector<Formula> tmp_vec_right = std::vector<Formula>();
            formula->getAtoms(tmp_vec_right);
            QVector<Formula> assumptions = QVector<Formula>::fromStdVector(tmp_vec_right);

            Node node(formula, assumptions);

            int noviId = sequent.getId() + 1;

            QVector<Node> newRightSequent = rightSequent;
            newRightSequent.append(node);

            QVector<Node> newLeftSequent = leftSequent;
            newLeftSequent.remove(i);

            Sequent newSequent(newLeftSequent, newRightSequent, noviId, sequent.getId(), false);
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }
            ui->textBrowser->append(newSequent.print_sequent());

            sequents.append(newSequent);

            current_sequent = newSequent.getId();
            qDebug() << "Primenjeno pravilo notLeft";
            qDebug() << current_sequent;



        }

    }
}

void MainWindow::on_notRight_clicked()
{
    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;

    for (i=0; i<rightSequent.size(); i++) {

        if (rightSequent[i].getFormula()->getType() == 3) {
            Formula formula = ((Not*)(rightSequent[i].getFormula().get()))->getOperand();


            std::vector<Formula> tmp_vec_right = std::vector<Formula>();
            formula->getAtoms(tmp_vec_right);
            QVector<Formula> assumptions = QVector<Formula>::fromStdVector(tmp_vec_right);

            Node node(formula, assumptions);

            int noviId = sequent.getId() + 1;

            QVector<Node> newRightSequent = rightSequent;
            newRightSequent.remove(i);

            QVector<Node> newLeftSequent = leftSequent;
            newLeftSequent.append(node);

            Sequent newSequent(newLeftSequent, newRightSequent, noviId, sequent.getId(), false);
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }
            ui->textBrowser->append(newSequent.print_sequent());

            sequents.append(newSequent);

            current_sequent = newSequent.getId();
            qDebug() << "Primenjeno pravilo notRight";
            qDebug() << current_sequent;

        }

    }
}

void MainWindow::on_falseLeft_clicked()
{
    Sequent sequent = sequents.at(current_sequent);


    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;

    for (i=0; i<leftSequent.size(); i++) {

        if (leftSequent[i].getFormula()->getType() == 1) {
            ui->textBrowser->clear();
            ui->textBrowser->append("<b style='color:blue'>Done</b>");

            preostali = nadjiPreostale();

            if(!preostali.empty()){
               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }
            else{
                ui->textBrowser->clear();
                ui->textBrowser->append("<b style='color:red'>Done</b>");
            }



            qDebug() << "Primenjeno pravilo falseLeft";
            qDebug() << current_sequent;

            traziNovoPravilo(sequent);

        }

    }
}

void MainWindow::on_trueRight_clicked()
{
    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;

    for (i=0; i<rightSequent.size(); i++) {

        if (rightSequent[i].getFormula()->getType() == 0) {
            ui->textBrowser->clear();

            ui->textBrowser->append("<b style='color:blue'>Done</b>");

            preostali = nadjiPreostale();

            if(!preostali.empty()){
               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }
            else{
                ui->textBrowser->clear();
                ui->textBrowser->append("<b style='color:red'>Done</b>");
            }

            qDebug() << "Primenjeno pravilo trueRight";
            qDebug() << current_sequent;

             traziNovoPravilo(sequent);
        }

    }
}

void MainWindow::on_ass_clicked()
{
    Sequent sequent = sequents.at(current_sequent);

    qDebug() << sequent.print_sequent();

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i,j;


    for (i=0; i<leftSequent.size(); i++) {
        for (j=0; j<rightSequent.size(); j++) {

            if (leftSequent[i].getFormula()->equalTo(rightSequent[j].getFormula())) {
                ui->textBrowser->clear();

                ui->textBrowser->append("<b style='color:blue'>Done</b>");

                preostali = nadjiPreostale();

                if(!preostali.empty()){
                   for(int k=0; k < preostali.length(); k++){
                      ui->textBrowser->append(preostali[k].print_sequent());
                   }
                }
                else{
                    ui->textBrowser->clear();
                    ui->textBrowser->append("<b style='color:red'>Done</b>");
                }



                qDebug() << "Primenjeno pravilo ass";
                qDebug() << current_sequent;

                traziNovoPravilo(sequent);

            }
        }
    }
}

void MainWindow::on_leftWeak_clicked()
{
    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();


    int n;

    if (leftSequent.size() > 0) {
        n = leftSequent.size()-1;
    }


    QVector<Node> newLeftSequent = leftSequent;
    newLeftSequent.remove(n);

    Sequent newSequent(newLeftSequent,rightSequent, sequent.getId() + 1, sequent.getId(), false);
    ui->textBrowser->clear();
    preostali = nadjiPreostale();

    if(!preostali.empty()){
       for(int k=0; k < preostali.length(); k++){
          ui->textBrowser->append(preostali[k].print_sequent());
       }
    }
    ui->textBrowser->append(newSequent.print_sequent());


    sequents.append(newSequent);
    current_sequent = newSequent.getId();

    qDebug() << "Primenjeno pravilo leftWeak";
    qDebug() << current_sequent;



}

void MainWindow::on_rightWeak_clicked()
{

    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();


    int n;

    if (rightSequent.size() > 0) {
        n = rightSequent.size()-1;
    }


    QVector<Node> newRightSequent = rightSequent;
    newRightSequent.remove(n);

    Sequent newSequent(leftSequent,newRightSequent, sequent.getId() + 1, sequent.getId(), false);
    ui->textBrowser->clear();
    preostali = nadjiPreostale();

    if(!preostali.empty()){
       for(int k=0; k < preostali.length(); k++){
          ui->textBrowser->append(preostali[k].print_sequent());
       }
    }
    ui->textBrowser->append(newSequent.print_sequent());


    sequents.append(newSequent);
    current_sequent = newSequent.getId();

    qDebug() << "Primenjeno pravilo rightWeak";
    qDebug() << current_sequent;


}

void MainWindow::on_leftContraction_clicked()
{
    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();


    int n;

    if (leftSequent.size() > 0) {
        n = leftSequent.size()-1;
    }


    QVector<Node> newLeftSequent = leftSequent;
    newLeftSequent.push_back(leftSequent[n]);

    Sequent newSequent(newLeftSequent,rightSequent, sequent.getId() + 1, sequent.getId(), false);
    ui->textBrowser->clear();
    preostali = nadjiPreostale();

    if(!preostali.empty()){
       for(int k=0; k < preostali.length(); k++){
          ui->textBrowser->append(preostali[k].print_sequent());
       }
    }
    ui->textBrowser->append(newSequent.print_sequent());


    sequents.append(newSequent);
    current_sequent = newSequent.getId();

    qDebug() << "Primenjeno pravilo leftContraction";
    qDebug() << current_sequent;


}

void MainWindow::on_rightContraction_clicked()
{
    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int n;

    if (rightSequent.size() > 0) {
        n = rightSequent.size()-1;
    }

    QVector<Node> newRightSequent = rightSequent;
    newRightSequent.push_back(rightSequent[n]);

    Sequent newSequent(leftSequent,newRightSequent, sequent.getId() + 1, sequent.getId(), false);
    ui->textBrowser->clear();
    preostali = nadjiPreostale();

    if(!preostali.empty()){
       for(int k=0; k < preostali.length(); k++){
          ui->textBrowser->append(preostali[k].print_sequent());
       }
    }
    ui->textBrowser->append(newSequent.print_sequent());

    sequents.append(newSequent);
    current_sequent = newSequent.getId();

    qDebug() << "Primenjeno pravilo rightContraction";
    qDebug() << current_sequent;

}

//funckcija koja trazi sledecu granu koja se dokazuje
void MainWindow::traziNovoPravilo(Sequent s){

    //s je sequent koji je assumption
    s.setIndikator(true);
    Sequent parent = vratiRoditelja(s);


    /* Dok se ID roditelja ne nadje u nizu roditelja sa dvoje dece ili dok ne dodjemo
    do prvog sekventa,povecavamo indikator i trazimo njegovog roditelja. */
    while(roditeljiSaDvaDeteta.indexOf(parent.getId()) == -1 && parent.getId() != 0){

        parent.setIndikator(true);
        parent = vratiRoditelja(parent);

    }

    /*Kada dodjemo do prvog sekventa,znaci da nema vise nista da se dokazuje i da je teorema
    uspesno dokazana. Inace se prebacujemo na drugo dete cvora sa dva deteta*/

    if(roditeljiSaDvaDeteta.indexOf(parent.getId()) != -1){
        current_sequent = parent.getId() + 2;
        roditeljiSaDvaDeteta.removeLast();

        if(current_sequent == 0){
            qDebug() << "USPESNO DOKAZANA TEOREMA";
            /*BOKIRAJ PROGRAM*/
        }

    }

    if(parent.getId() == 0){
        qDebug() << "USPESNO DOKAZANA TEOREMA";

        /*BOKIRAJ PROGRAM*/
    }


}

Sequent MainWindow::vratiRoditelja(Sequent s){
    int roditeljId = s.getParentId();
    Sequent roditelj;


    int i=0;
    for(;i < sequents.length(); i++) {
        if(sequents[i].getId() == roditeljId) {
            roditelj = sequents[i];
        }
    }

    return roditelj;
}

QVector<Sequent> MainWindow::nadjiPreostale(){

    QVector<Sequent> preostali;
    int i,deteID,roditeljID;

    for(i=0; i < roditeljiSaDvaDeteta.length(); i++){
        deteID = roditeljiSaDvaDeteta[i] + 2;

        int j=0;
        for(;j < sequents.length(); j++) {
            if(sequents[j].getId() == deteID) {
                preostali.push_back(sequents[j]);
            }
        }

    }

    return preostali;
}

void MainWindow::on_ponisti_clicked()
{
    Sequent sequent_current;

    int i=0;
    for(;i < sequents.length(); i++) {
        if(sequents[i].getId() == current_sequent) {
            sequent_current = sequents[i];
        }
    }

    Sequent roditelj = vratiRoditelja(sequent_current);

    qDebug() << roditelj.print_sequent();

    /*Ako se formula ne racva,brisemo iz skupa svih sequenta*/
    if(roditeljiSaDvaDeteta.indexOf(roditelj.getId()) == -1){
        sequents.removeLast();
    }
    else{
        roditeljiSaDvaDeteta.removeLast();
        sequents.removeLast();
        sequents.removeLast();
    }

    current_sequent = roditelj.getId();

    ui->textBrowser->clear();
    ui->textBrowser->append(roditelj.print_sequent());

}
