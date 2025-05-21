#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "rootmethods.h"
#include "interpolationmethods.h"
#include "integrationmethods.h"
#include "eulermethods.h"
#include "curvefitting.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_TablePoints_valueChanged(int points);

    void on_RootPageBtn_clicked();

    void on_InterpolationPageBtn_clicked();

    void on_IntegerationPageBtn_clicked();

    void on_EulerPageBtn_clicked();

    void on_CurveFittingPageBtn_clicked();

    void on_RootSolveButton_clicked();

    void on_InterpolationSolveButton_clicked();

    void on_IntSolveButton_clicked();

    void on_StepsInput_valueChanged(int steps);

    void on_X_eq_option_clicked(bool checked);

    void on_X_range_clicked(bool checked);

    void on_EulerSolveButton_clicked();

    void on_X0Input_valueChanged(double arg1);

    void on_CurveMethodSelector_currentIndexChanged(int index);

    void on_CurveCustomXCheck_clicked(bool checked);

    void on_CurveCustomYCheck_clicked(bool checked);

    void on_CurveTablePoints_valueChanged(int arg1);

    void on_CurveSolveButton_clicked();

private:
    Ui::MainWindow *ui;

    RootMethods RootSolver;
    InterpolationMethods InterpolSolver;
    IntegrationMethods IntegrSolver;
    EulerMethods EulerSolver;
    CurveFitting CurveSolver;
};
#endif // MAINWINDOW_H
