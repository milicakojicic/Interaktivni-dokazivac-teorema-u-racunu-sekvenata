#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fol.hpp"
#include <QString>
#include <qdebug.h>
#include <sstream>
#include "sequent.h"
#include <QMessageBox>

using namespace std;

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern void yy_switch_to_buffer (YY_BUFFER_STATE new_buffer  );

/* promenljiva u koju se smesta formula nakon parsiranja */
extern Formula parsed_formula;

ostringstream stream;
/*niz u koji se smestaju sekventi koji se dele na dva nakon primene pravila */
QVector<int> roditeljiSaDvaDeteta = {};
/* niz u koji se smestaju sekventi koji nisu jos dokazani */
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
    ui->lineEdit->setFocus();
    setWindowTitle("Interaktivni dokazivač u prirodnoj dedukciji");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_potvrdi_clicked()
{
    /* kada se jednom klikne potvrdi, onemogucujemo da se klikne opet sve dok se ne resetuje aplikacija */
    ui->potvrdi->setDisabled(true);

    /* uzimamo formulu sa ulaza i prarsiramo je */
    QString formula = ui->lineEdit->text();
    formula += ";";
    string novaFormula = formula.toUtf8().constData();
    YY_BUFFER_STATE buffer = yy_scan_string(novaFormula.c_str());

    /* ako je formula neispravna prikazujemo poruku i zatvaramo aplikaciju */
    if(yyparse() == 1){

        qDebug() << "Neispravna formula na ulazu";
        QMessageBox msgBox;
        msgBox.setText("Neispravna formula na ulazu");
        msgBox.setInformativeText("Pokrenite aplikaciju ponovo");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        exit(EXIT_FAILURE);
    }

    /* pravimo levi i desni niz Node-ova i sekvent koji dodajemo u niz sekvenata*/
    QVector<Node> right;
    right.append(Node(parsed_formula));
    QVector<Node> left = {};
    Sequent sequent(left,right,0,0);
    sequents.append(sequent);

    /* inicijalizacija trenutnog sekventa */
    current_sequent = sequent.getId();

    /* dodajemo u ispis formulu */
    ui->textBrowser->append(sequent.print_sequent());

    /* trazimo trenutni sekvent i za njega aktiviramo dugmice */
    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);
    ui->leftPermutation->setDisabled(false);
    ui->rightPermutation->setDisabled(false);
    ui->rightWeak->setDisabled(false);
    ui->leftWeak->setDisabled(false);
    ui->ass->setDisabled(false);
    ui->ponisti->setDisabled(false);
    ui->resetuj->setDisabled(false);

}

void MainWindow::on_andLeft_clicked()
{
    /* uzimamo trenutni sekvent i njegovu levu i desnu stranu */
    Sequent sequent = sequents.at(current_sequent);
    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;
    /* prolazimo kroz cvorove sa leve strane i trazimo onaj kome je tip formule AND */
    for (i=0; i<leftSequent.size(); i++) {

        if (leftSequent[i].getFormula()->getType() == 4) {
            /* uzimamo levi i desni operand i od njih pravimo cvorove */
            Formula leftFormula = ((And*)(leftSequent[i].getFormula().get()))->getOperand1();
            Formula rightFormula = ((And*)(leftSequent[i].getFormula().get()))->getOperand2();

            Node leftNode(leftFormula);
            Node rightNode(rightFormula);

            /* pravimo novi levi sekvent */
            QVector<Node> newLeftSequent = leftSequent;
            newLeftSequent.remove(i);
            newLeftSequent.append(leftNode);
            newLeftSequent.append(rightNode);
            int noviId = sequents.length();
            Sequent newSequent(newLeftSequent, rightSequent, noviId, sequent.getId());

            /* ispis preostalih i novog sekventa */
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
               ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }

            ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");
            ui->textBrowser->append(newSequent.print_sequent());

            sequents.append(newSequent);
            current_sequent = newSequent.getId();

            qDebug() << "Primenjeno pravilo andLeft";
            qDebug() << current_sequent;
        }
    }

    /* resetovanje dozvoljenih dugmica za novo pravilo */
    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);

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

            Node leftNode(leftFormula);
            Node rigthNode(rightFormula);


            QVector<Node> newRightSequent = rightSequent;
            newRightSequent.replace(i, rigthNode);

            QVector<Node> newLeftSequent = leftSequent;
            newLeftSequent.append(leftNode);

            Sequent newSequent(newLeftSequent,newRightSequent, sequents.length(), sequent.getId());
            ui->textBrowser->clear();

            preostali = nadjiPreostale();

            if(!preostali.empty()){
               ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }

            ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");

            ui->textBrowser->append(newSequent.print_sequent());

            sequents.append(newSequent);

            current_sequent = newSequent.getId();
            qDebug() << "Primenjeno pravilo impRight";
            qDebug() << current_sequent;
        }
    }

    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);

}

void MainWindow::on_impLeft_clicked()
{
    /* uzimamo sekvent i levi i desni niz Node-ova*/
    Sequent sequent = sequents.at(current_sequent);

    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;
    for (i=0; i<leftSequent.size(); i++) {

        /* trazimo sa leve strane implikaciju */
        if (leftSequent[i].getFormula()->getType() == 6) {

            /*uzimamo levu i desnu stranu,pravimo cvorove i novu levu i desnu stranu,
             * a zatim i nove sekvente */
            Formula leftFormula = ((Imp*)(leftSequent[i].getFormula().get()))->getOperand1();
            Formula rightFormula = ((Imp*)(leftSequent[i].getFormula().get()))->getOperand2();

            Node leftNode(leftFormula);
            Node rightNode(rightFormula);

            QVector<Node> newLeftSequent = leftSequent;
            newLeftSequent.remove(i);

            QVector<Node> newRightSequent = rightSequent;
            newRightSequent.push_back(leftNode);

            int noviId = sequents.length();
            Sequent newSequent1(newLeftSequent, newRightSequent, noviId, sequent.getId());
            sequents.append(newSequent1);

            noviId++;
            QVector<Node> newLeftSequent1 = leftSequent;
            newLeftSequent1.remove(i);
            newLeftSequent1.append(rightNode);

            QVector<Node> newRightSequent1 = rightSequent;
            Sequent newSequent2(newLeftSequent1, newRightSequent1, noviId, sequent.getId());
            sequents.append(newSequent2);

            /* ispis preostalih i nova dva sekventa- prvi kao trenutni a drugi u skupu preostalih */
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
                ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
               ui->textBrowser->append(newSequent2.print_sequent());

            }
            else{
                /* ako preostalih nema,ispisujemo samo drugi */
                ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");
                ui->textBrowser->append(newSequent2.print_sequent());
            }

            ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");
            ui->textBrowser->append(newSequent1.print_sequent());

            /* trenutni sekvent je prvi */
            current_sequent = newSequent1.getId();

            qDebug() << "Primenjeno pravilo impLeft";
            qDebug() << current_sequent;

            /* sekvent na kome smo rpimenili pravilo dodajemo u niz roditelja sa dva deteta */
            roditeljiSaDvaDeteta.push_back(sequent.getId());

        }


    }

    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);

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

            Node leftNode(leftFormula);
            Node rightNode(rightFormula);


            QVector<Node> newRightSequent1 = rightSequent;
            newRightSequent1.remove(i);
            newRightSequent1.append(rightNode);


            QVector<Node> newRightSequent2 = rightSequent;
            newRightSequent2.remove(i);
            newRightSequent2.append(leftNode);

            int noviiD = sequents.length();

            Sequent newSequent1(leftSequent,newRightSequent1, noviiD, sequent.getId());
            Sequent newSequent2(leftSequent,newRightSequent2, noviiD + 1, sequent.getId());

            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
               ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
               ui->textBrowser->append(newSequent2.print_sequent());
            }
            else{
                ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");
                ui->textBrowser->append(newSequent2.print_sequent());
            }

            ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");
            ui->textBrowser->append(newSequent1.print_sequent());

            sequents.append(newSequent1);
            sequents.append(newSequent2);

            current_sequent = newSequent1.getId();
            qDebug() << "Primenjeno pravilo andRight";
            qDebug() << current_sequent;

            roditeljiSaDvaDeteta.push_back(sequent.getId());

        }

    }

    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);

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

            Node leftNode(leftFormula);
            Node rightNode(rightFormula);


            QVector<Node> newLeftSequent1 = leftSequent;
            newLeftSequent1.remove(i);
            newLeftSequent1.append(leftNode);


            QVector<Node> newLeftSequent2 = leftSequent;
            newLeftSequent2.remove(i);
            newLeftSequent2.append(rightNode);

            int noviID = sequents.length();

            Sequent newSequent1(newLeftSequent1,rightSequent, noviID, sequent.getId());
            Sequent newSequent2(newLeftSequent2,rightSequent, noviID + 1, sequent.getId());

            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
                ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }

               ui->textBrowser->append(newSequent2.print_sequent());
            }
            else{
                ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");
                ui->textBrowser->append(newSequent2.print_sequent());
            }
            ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");

            ui->textBrowser->append(newSequent1.print_sequent());

            sequents.append(newSequent1);
            sequents.append(newSequent2);

            current_sequent = newSequent1.getId();
            qDebug() << "Primenjeno pravilo orLeft";
            qDebug() << current_sequent;

            roditeljiSaDvaDeteta.push_back(sequent.getId());

        }

    }

    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);

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

            Node leftNode(leftFormula);
            Node rightNode(rightFormula);

            int noviId = sequents.length();

            QVector<Node> newRightSequent = rightSequent;
            newRightSequent.remove(i);
            newRightSequent.append(leftNode);
            newRightSequent.append(rightNode);

            Sequent newSequent(leftSequent, newRightSequent, noviId, sequent.getId());
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
               ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }
            ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");

            ui->textBrowser->append(newSequent.print_sequent());

            sequents.append(newSequent);

            current_sequent = newSequent.getId();

            qDebug() << "OR R" << newSequent.print_sequent();

            int p;
            for(p=0;p<sequents.length();p++){
                qDebug() << sequents[p].print_sequent() << "ID:" <<
                            sequents[p].getId() << "PID:" << sequents[p].getParentId();
            }

            qDebug() << "Primenjeno pravilo orRight";
            qDebug() << current_sequent;

        }

    }

    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);

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

            Node node(formula);

            int noviId = sequents.length();

            QVector<Node> newRightSequent = rightSequent;
            newRightSequent.append(node);

            QVector<Node> newLeftSequent = leftSequent;
            newLeftSequent.remove(i);

            Sequent newSequent(newLeftSequent, newRightSequent, noviId, sequent.getId());
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
                ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }
            ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");

            ui->textBrowser->append(newSequent.print_sequent());

            sequents.append(newSequent);

            current_sequent = newSequent.getId();
            qDebug() << "Primenjeno pravilo notLeft";
            qDebug() << current_sequent;



        }

    }

    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);

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

            Node node(formula);

            int noviId = sequents.length();

            QVector<Node> newRightSequent = rightSequent;
            newRightSequent.remove(i);

            QVector<Node> newLeftSequent = leftSequent;
            newLeftSequent.append(node);

            Sequent newSequent(newLeftSequent, newRightSequent, noviId, sequent.getId());
            ui->textBrowser->clear();
            preostali = nadjiPreostale();

            if(!preostali.empty()){
                ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

               for(int k=0; k < preostali.length(); k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }
            }
            ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");

            ui->textBrowser->append(newSequent.print_sequent());

            sequents.append(newSequent);

            current_sequent = newSequent.getId();
            qDebug() << "Primenjeno pravilo notRight";
            qDebug() << current_sequent;

        }

    }

    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);

}

void MainWindow::on_falseLeft_clicked()
{
    Sequent sequent = sequents.at(current_sequent);


    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    int i;

    for (i=0; i<leftSequent.size(); i++) {

        if (leftSequent[i].getFormula()->getType() == 1) {
            preostali = nadjiPreostale();
            ui->textBrowser->clear();

            if(!preostali.empty()){
                if(preostali.length() > 1){
                    ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");
                }

                for(int k=0; k < preostali.length()-1; k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }

                ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");
                ui->textBrowser->append("<b style='color:blue'>Završeno</b>");
                ui->textBrowser->append(preostali[preostali.length()-1].print_sequent());

            }
            else{
                ui->textBrowser->clear();
                ui->textBrowser->append("<b style='color:red'>Teorema</b>");
            }

            qDebug() << "Primenjeno pravilo falseLeft";
            qDebug() << current_sequent;

            traziNovoPravilo(sequent);

        }

    }

    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);

}

void MainWindow::on_trueRight_clicked()
{
    Sequent sequent = sequents.at(current_sequent);
    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();
    int i;

    /* prodjemo kroz niz cvorova sa desne strane i ako nadjemo neki sa tipom formule TRUE
      ispisujemo preostale i za taj sekvent zavrseno */
    for (i=0; i<rightSequent.size(); i++) {
        if (rightSequent[i].getFormula()->getType() == 0) {

            preostali = nadjiPreostale();
            ui->textBrowser->clear();

            if(!preostali.empty()){
                if(preostali.length() > 1){
                    ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");
                }
                for(int k=0; k < preostali.length()-1; k++){
                  ui->textBrowser->append(preostali[k].print_sequent());
               }

                ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");
                ui->textBrowser->append("<b style='color:blue'>Završeno</b>");
                /* u delu za trenutno dokazivanje ispisujemo poslednje pravilo iz preostalih,
                    tj. sledece koje treba da se dokaze*/
                ui->textBrowser->append(preostali[preostali.length()-1].print_sequent());

                traziNovoPravilo(sequent);

            }
            else{
                ui->textBrowser->clear();
                ui->textBrowser->append("<b style='color:red'>Teorema</b>");
            }

            qDebug() << "Primenjeno pravilo trueRight";
            qDebug() << current_sequent;

        }

    }

    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);

}

/* pravilo assumption */
void MainWindow::on_ass_clicked()
{
    Sequent sequent = sequents.at(current_sequent);
    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();
    int i,j;

    /* prolazimo kroz niz cvorova sa leve i desne strane u sekventu */
    for (i=0; i<leftSequent.size(); i++) {
        for (j=0; j<rightSequent.size(); j++) {

            /* ako su dva cvora jednaka znaci da je pravilo moguce primeniti*/
            if (leftSequent[i].getFormula()->equalTo(rightSequent[j].getFormula())) {
                /* ispisujemo preostale */
                preostali = nadjiPreostale();
                ui->textBrowser->clear();

                if(!preostali.empty()){
                    if(preostali.length()>1){
                        ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");
                    }

                    for(int k=0; k < preostali.length()-1; k++){
                      ui->textBrowser->append(preostali[k].print_sequent());
                   }

                    /* dodaje se plavo ZAVRSENO za trenutnu formulu */
                    ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");
                    ui->textBrowser->append("<b style='color:blue'>Završeno</b>");
                    ui->textBrowser->append(preostali[preostali.length()-1].print_sequent());

                    /* trazimo sledece pravilo koje ce se dokazivati*/
                    traziNovoPravilo(sequent);
                }
                else{
                    /* ako nema nista vise da se dokazuje ispisuje se crvemo TEOREMA*/
                    ui->textBrowser->clear();
                    ui->textBrowser->append("<b style='color:red'>Teorema</b>");
                }

                qDebug() << "Primenjeno pravilo ass";
                qDebug() << current_sequent;


                //


            }
        }
    }

    Sequent trenutni = trenutniSekvent(current_sequent);
    resetuj_dugmice(trenutni);

}

/* pravilo levo slabljenje*/
void MainWindow::on_leftWeak_clicked()
{
    Sequent sequent = sequents.at(current_sequent);
    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();
    int n = leftSequent.size()-1;

    QVector<Node> newLeftSequent = leftSequent;
    newLeftSequent.remove(n);

    Sequent newSequent(newLeftSequent,rightSequent, sequents.length(), sequent.getId());

    ui->textBrowser->clear();
    preostali = nadjiPreostale();

    if(!preostali.empty()){
       ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

       for(int k=0; k < preostali.length(); k++){
          ui->textBrowser->append(preostali[k].print_sequent());
       }
    }
    ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");
    ui->textBrowser->append(newSequent.print_sequent());


    sequents.append(newSequent);
    current_sequent = newSequent.getId();

    qDebug() << "Primenjeno pravilo leftWeak";
    qDebug() << current_sequent;

    resetuj_dugmice(newSequent);

}

/* pravilo desno slabljenje */
void MainWindow::on_rightWeak_clicked()
{
    /* uzimamo sekvent za trenutni ID,nizove cvorova sa leve i desne strane*/
    Sequent sequent = sequents.at(current_sequent);
    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();
    int n = rightSequent.size()-1;

    /*pravimo novi desni sekvent tako sto sklanjamo poslednji Node iz njega*/
    QVector<Node> newRightSequent = rightSequent;
    newRightSequent.remove(n);

    /* pravimo novi sekvent sa istom levom stranom, izmenjenom desnom stranom, gde je novi ID
       maksimalan id u nizu sekvenata + 1 tj. duzina niza sekvenata, a parentID je ID trenutnog sekventa
       od kog smo dobili novi sekvent */
    Sequent newSequent(leftSequent,newRightSequent, sequents.length(), sequent.getId());

    /* ispisujemo preostale nedokazane sekvente i zatim novi sekvent kao trenutni sekvent koji se dokazuje */
    ui->textBrowser->clear();
    preostali = nadjiPreostale();

    if(!preostali.empty()){
        ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

       for(int k=0; k < preostali.length(); k++){
          ui->textBrowser->append(preostali[k].print_sequent());
       }
    }
    ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");
    ui->textBrowser->append(newSequent.print_sequent());


    sequents.append(newSequent);
    current_sequent = newSequent.getId();

    qDebug() << "Primenjeno pravilo rightWeak";
    qDebug() << current_sequent;

    resetuj_dugmice(newSequent);

}

/* pravilo leve kontrakcije */
void MainWindow::on_leftContraction_clicked()
{
    Sequent sequent = sequents.at(current_sequent);
    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();
    int n = leftSequent.size()-1;

    QVector<Node> newLeftSequent = leftSequent;
    newLeftSequent.push_back(leftSequent[n]);

    Sequent newSequent(newLeftSequent,rightSequent, sequents.length(), sequent.getId());
    ui->textBrowser->clear();
    preostali = nadjiPreostale();

    if(!preostali.empty()){
        ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

       for(int k=0; k < preostali.length(); k++){
          ui->textBrowser->append(preostali[k].print_sequent());
       }
    }
    ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");

    ui->textBrowser->append(newSequent.print_sequent());

    sequents.append(newSequent);
    current_sequent = newSequent.getId();

    qDebug() << "Primenjeno pravilo leftContraction";
    qDebug() << current_sequent;

    resetuj_dugmice(newSequent);

}

/* pravilo desne kontrakcije */
void MainWindow::on_rightContraction_clicked()
{
    /* uzimamo sekvent gde je ID trenutni i niz levih i desnih Node-ova */
    Sequent sequent = sequents.at(current_sequent);
    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();
    int n = rightSequent.size()-1;

    /* pravimo novi desni sekvent tako sto dodajemo jos jednom poslednji Node sa desne strane */
    QVector<Node> newRightSequent = rightSequent;
    newRightSequent.push_back(rightSequent[n]);

    /* pravimo novi sekvent sa istom levom stranom, izmenjenom desnom stranom, gde je novi ID
       maksimalan id u nizu sekvenata + 1 tj. duzina niza sekvenata, a parentID je ID trenutnog sekventa
       od kog smo dobili novi sekvent */
    Sequent newSequent(leftSequent,newRightSequent, sequents.length(), sequent.getId());

    /* ispisujemo preostale nedokazane sekvente i zatim novi sekvent kao trenutni sekvent koji se dokazuje*/
    ui->textBrowser->clear();
    preostali = nadjiPreostale();

    if(!preostali.empty()){
        ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

       for(int k=0; k < preostali.length(); k++){
          ui->textBrowser->append(preostali[k].print_sequent());
       }
    }
    ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");

    ui->textBrowser->append(newSequent.print_sequent());

    sequents.append(newSequent);
    current_sequent = newSequent.getId();

    qDebug() << "Primenjeno pravilo rightContraction";
    qDebug() << current_sequent;

    /* za novi sekvent resetujemo dugmice */
    resetuj_dugmice(newSequent);

}

/* nakon sto se jedan sekvent dokaze, trazimo sledeci koji treba da se dokaze */
void MainWindow::traziNovoPravilo(Sequent s){

    /*trazimo roditelja sekventa*/
    Sequent parent = vratiRoditelja(s);


    /* Dok se ID roditelja ne nadje u nizu roditelja sa dvoje dece(sto znaci da je tu doslo do razdvajanja)
     *  ili dok ne dodjemo do prvog sekventa(znaci da smo dokazali) trazimo njegovog roditelja
      tj idemo dalje po stablu */
    while(roditeljiSaDvaDeteta.indexOf(parent.getId()) == -1 && parent.getId() != 0){
        parent = vratiRoditelja(parent);
    }

    /* Ako jeste roditelj sa dva deteta,uzimamo njegovo drugo dete i postavljamo ga za
        trenutno pravilo,a roditelja brisemo iz liste roditelja sa dva deteta */
    if(roditeljiSaDvaDeteta.indexOf(parent.getId()) != -1){
        current_sequent = parent.getId() + 2;
        roditeljiSaDvaDeteta.removeLast();

    }

}

/* funkcija koja za prosledjeni sekvent vraca njgovog roditelja */
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

/* funkcija koja nalazi sve sekvente koji nisu jos dokazani */
QVector<Sequent> MainWindow::nadjiPreostale(){

    QVector<Sequent> preostali;
    int i,deteID,roditeljID;

    /* prolazi se kroz niz roditelja koji imaju dva deteta i za svaki se uzima
      njegovo drugo dete (prvo se dokazuje ili je vec dokazano) i smesta se u niz koji vracamo */
    for(i=0; i < roditeljiSaDvaDeteta.length(); i++){
        deteID = roditeljiSaDvaDeteta[i] + 2;

        int j;
        for(j=0; j < sequents.length(); j++) {
            if(sequents[j].getId() == deteID) {
                preostali.push_back(sequents[j]);
            }
        }

    }

    return preostali;
}

/* funkcija koja vraca dokaz jedan korak u nazad */
void MainWindow::on_ponisti_clicked()
{
    /* uzima se trenutni sekvent */
    Sequent sequent_current = trenutniSekvent(current_sequent);
    /* uzima se roditelj trenutnog sekventa */
    Sequent roditelj = vratiRoditelja(sequent_current);

    /* Ako se roditelj formula deli na dva dela,brisemo iz skupa svih sequenta */
    if(roditeljiSaDvaDeteta.indexOf(roditelj.getId()) == -1){
        sequents.removeLast();
    }
    /* Ako se roditelj formula deli na dva dela,brisemo oba deteta iz skupa svih formula i
    brisemo roditelja iz niza formula sa dva deteta */
    else{
        roditeljiSaDvaDeteta.removeLast();
        sequents.removeLast();
        sequents.removeLast();
    }

    /* Postavljamo roditelja za trenutnu formulu */
    current_sequent = roditelj.getId();

    /* Cistimo prikaz */
    ui->textBrowser->clear();

    /* Nalazimo sta je ostalo da se dokaze i to ispisujemo */
    preostali = nadjiPreostale();

    if(!preostali.empty()){
       ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

       for(int k=0; k < preostali.length(); k++){
          ui->textBrowser->append(preostali[k].print_sequent());
       }
    }

    /* Ispisujemo formulu na koju smo se vratili i koja se dokazuje */

    ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");
    ui->textBrowser->append(roditelj.print_sequent());

    /* resetujemo dugmice za novu formulu */
    resetuj_dugmice(roditelj);

}

/* funkcija koja brise dosadasnji rad i vraca program na pocetak; */
void MainWindow::on_resetuj_clicked()
{

    ui->potvrdi->setDisabled(false);
    sequents.clear();
    current_sequent = 0;
    ui->textBrowser->clear();
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();

    ui->andLeft->setDisabled(true);
    ui->falseLeft->setDisabled(true);
    ui->impLeft->setDisabled(true);
    ui->impRight->setDisabled(true);
    ui->notLeft->setDisabled(true);
    ui->notRight->setDisabled(true);
    ui->orLeft->setDisabled(true);
    ui->orRight->setDisabled(true);
    ui->rightAnd->setDisabled(true);
    ui->trueRight->setDisabled(true);
    ui->rightContraction->setDisabled(true);
    ui->leftContraction->setDisabled(true);
    ui->leftPermutation->setDisabled(true);
    ui->rightPermutation->setDisabled(true);
    ui->rightWeak->setDisabled(true);
    ui->leftWeak->setDisabled(true);
    ui->ass->setDisabled(true);
    ui->ponisti->setDisabled(true);
}

/* funkcija koja se poziva nakon svakog menjanja formulei i koja aktivira dugmice za pravila
 * koja se mogu primeniti nad tom formulom */
void MainWindow::resetuj_dugmice(Sequent s){
   ui->andLeft->setDisabled(true);
   ui->falseLeft->setDisabled(true);
   ui->impLeft->setDisabled(true);
   ui->impRight->setDisabled(true);
   ui->notLeft->setDisabled(true);
   ui->notRight->setDisabled(true);
   ui->orLeft->setDisabled(true);
   ui->orRight->setDisabled(true);
   ui->rightAnd->setDisabled(true);
   ui->trueRight->setDisabled(true);
   ui->rightContraction->setDisabled(true);
   ui->leftContraction->setDisabled(true);
   ui->rightPermutation->setDisabled(true);
   ui->leftPermutation->setDisabled(true);
   ui->rightWeak->setDisabled(true);
   ui->leftWeak->setDisabled(true);

   QVector<Node> levo = s.getLeft();
   QVector<Node> desno = s.getRight();
   QVector<int> tipovilevo = {};
   QVector<int> tipovidesno = {};

   /* skupljaju se u nizu tipovi svi tipovi cvorova sa leve i desne strane sekventa */
   int i,j;
   for(i=0; i < levo.length(); i++ ){
       int tip = levo[i].getFormula()->getType();
       if(tipovilevo.indexOf(tip) == -1){
          tipovilevo.push_back(tip);
       }
   }

   for(j=0; j < desno.length(); j++){
       int tip = desno[j].getFormula()->getType();
       if(tipovidesno.indexOf(tip) == -1){
          tipovidesno.push_back(tip);
       }
   }

   /* u zavisnosti od tipova cvorova,aktiviramo dugmice */
   if(tipovidesno.indexOf(0) != -1){
       ui->trueRight->setDisabled(false);
   }
   if(tipovilevo.indexOf(1) != -1){
       ui->falseLeft->setDisabled(false);
   }
   if(tipovilevo.indexOf(3) != -1){
       ui->notLeft->setDisabled(false);
   }
   if(tipovidesno.indexOf(3) != -1){
       ui->notRight->setDisabled(false);
   }
   if(tipovilevo.indexOf(4) != -1){
       ui->andLeft->setDisabled(false);
   }
   if(tipovidesno.indexOf(4) != -1){
       ui->rightAnd->setDisabled(false);
   }
   if(tipovilevo.indexOf(5) != -1){
       ui->orLeft->setDisabled(false);
   }
   if(tipovidesno.indexOf(5) != -1){
       ui->orRight->setDisabled(false);
   }
   if(tipovilevo.indexOf(5) != -1){
       ui->orLeft->setDisabled(false);
   }
   if(tipovidesno.indexOf(5) != -1){
       ui->orRight->setDisabled(false);
   }
   if(tipovilevo.indexOf(6) != -1){
       ui->impLeft->setDisabled(false);
   }
   if(tipovidesno.indexOf(6) != -1){
       ui->impRight->setDisabled(false);
   }
   if(tipovilevo.indexOf(7) != -1){
       ui->orLeft->setDisabled(false);
   }
   if(tipovidesno.indexOf(7) != -1){
       ui->orRight->setDisabled(false);
   }
   if(desno.length() > 0){
       ui->rightContraction->setDisabled(false);
       ui->rightPermutation->setDisabled(false);
       ui->rightWeak->setDisabled(false);
   }
   if(levo.length() > 0){
       ui->leftContraction->setDisabled(false);
       ui->leftPermutation->setDisabled(false);
       ui->leftWeak->setDisabled(false);
   }


}

/* za prosledjen id sekventa se vraca sam sekvent tako sto se pretrazuje po nizu svih sekvenata u dokazu */
Sequent MainWindow::trenutniSekvent(int currentSequent){
    Sequent sequent_current;

    int i=0;
    for(;i < sequents.length(); i++) {
        if(sequents[i].getId() == currentSequent) {
            sequent_current = sequents[i];
        }
    }

    return sequent_current;
}

void MainWindow::on_leftPermutation_clicked()
{
    /* uzimamo sekvent za trenutni ID,nizove cvorova sa leve i desne strane*/
    Sequent sequent = sequents.at(current_sequent);
    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    /*pravimo novi desni sekvent tako sto sklanjamo poslednji Node iz njega*/
    QVector<Node> newLeftSequent = leftSequent;
    newLeftSequent.remove(0);
    newLeftSequent.push_back(leftSequent[0]);

    /* pravimo novi sekvent sa izmenjenom levom stranom, istom desnom stranom, gde je novi ID
       maksimalan id u nizu sekvenata + 1 tj. duzina niza sekvenata, a parentID je ID trenutnog sekventa
       od kog smo dobili novi sekvent */
    Sequent newSequent(newLeftSequent,rightSequent, sequents.length(), sequent.getId());

    /* ispisujemo preostale nedokazane sekvente i zatim novi sekvent kao trenutni sekvent koji se dokazuje */
    ui->textBrowser->clear();
    preostali = nadjiPreostale();

    if(!preostali.empty()){
        ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

       for(int k=0; k < preostali.length(); k++){
          ui->textBrowser->append(preostali[k].print_sequent());
       }
    }
    ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");
    ui->textBrowser->append(newSequent.print_sequent());


    sequents.append(newSequent);
    current_sequent = newSequent.getId();

    qDebug() << "Primenjeno pravilo leftPermutation";
    qDebug() << current_sequent;

    resetuj_dugmice(newSequent);
}

void MainWindow::on_rightPermutation_clicked()
{
    /* uzimamo sekvent za trenutni ID,nizove cvorova sa leve i desne strane*/
    Sequent sequent = sequents.at(current_sequent);
    QVector<Node> leftSequent = sequent.getLeft();
    QVector<Node> rightSequent = sequent.getRight();

    /*pravimo novi desni sekvent tako sto sklanjamo poslednji Node iz njega*/
    QVector<Node> newRightSequent = rightSequent;
    newRightSequent.remove(0);
    newRightSequent.push_back(rightSequent[0]);

    /* pravimo novi sekvent sa istom levom stranom, izmenjenom desnom stranom, gde je novi ID
       maksimalan id u nizu sekvenata + 1 tj. duzina niza sekvenata, a parentID je ID trenutnog sekventa
       od kog smo dobili novi sekvent */
    Sequent newSequent(leftSequent,newRightSequent, sequents.length(), sequent.getId());

    /* ispisujemo preostale nedokazane sekvente i zatim novi sekvent kao trenutni sekvent koji se dokazuje */
    ui->textBrowser->clear();
    preostali = nadjiPreostale();

    if(!preostali.empty()){
        ui->textBrowser->append("<b style='color:green'>Preostalo da se dokaze:</b>");

       for(int k=0; k < preostali.length(); k++){
          ui->textBrowser->append(preostali[k].print_sequent());
       }
    }
    ui->textBrowser->append("<b style='color:green'>Trenutno se dokazuje:</b>");
    ui->textBrowser->append(newSequent.print_sequent());


    sequents.append(newSequent);
    current_sequent = newSequent.getId();

    qDebug() << "Primenjeno pravilo leftPermutation";
    qDebug() << current_sequent;

    resetuj_dugmice(newSequent);
}
