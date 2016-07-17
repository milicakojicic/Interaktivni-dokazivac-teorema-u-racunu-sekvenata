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
    QString formula = ui->lineEdit->text();

    formula += ";";

    string novaFormula = formula.toUtf8().constData();

    YY_BUFFER_STATE buffer = yy_scan_string(novaFormula.c_str());
            if(yyparse() == 1){
        qDebug() << "Neispravna formula na ulazu";
    }

    ostringstream stream;
    parsed_formula->printFormula(stream);

    QVector<Node> right;
    right.append(Node(parsed_formula));
    QVector<Node> left = {};

    Sequent sequent(left,right);
    sequent.print_sequent();

    string formulaStream = stream.str();
    formulaStream += " ;";
    qDebug() << QString::fromStdString(formulaStream);

    //ui->textBrowser->append();

}
