#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sequent.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_potvrdi_clicked();

    void on_andLeft_clicked();

    void on_impRight_clicked();

    void on_impLeft_clicked();

    void on_rightAnd_clicked();

    void on_orLeft_clicked();

    void on_orRight_clicked();

    void on_notLeft_clicked();

    void on_notRight_clicked();

    void on_falseLeft_clicked();

    void on_trueRight_clicked();

    void on_ass_clicked();

    void on_leftWeak_clicked();

    void on_rightWeak_clicked();

    void on_leftContraction_clicked();

    void on_rightContraction_clicked();

    void traziNovoPravilo(Sequent s);

    Sequent vratiRoditelja(Sequent s);

    void on_ponisti_clicked();

    QVector<Sequent> nadjiPreostale();

    void on_resetuj_clicked();

    void resetuj_dugmice(Sequent s);

    Sequent trenutniSekvent(int currentSequent);

    void on_leftPermutation_clicked();

    void on_rightPermutation_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
