#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "rootmethods.h"
#include "interpolationmethods.h"


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

private:
    Ui::MainWindow *ui;
    RootMethods RootSolver;
    InterpolationMethods InterpolSolver;
};
#endif // MAINWINDOW_H
