#include "mainwindow.h"
#include <QApplication>
#include "fol.hpp"

extern int yyparse();

/* Ovaj pokazivac ce nakon parsiranja dobiti vrednost
   adrese parsirane formule. */
extern Formula parsed_formula;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
