#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardItemModel>
#include <QStandardItem>

static QStandardItem* comboItem(QComboBox *combo, int index);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , RootSolver(RootMethods())
    , InterpolSolver(InterpolationMethods())
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////  Menu Buttons  //////////////////////////////////////////////////////

void MainWindow::on_RootPageBtn_clicked()
{
    ui->Pages->setCurrentIndex(0);
    ui->RootPageBtn->setFlat(0);
    ui->IntegerationPageBtn->setFlat(1);
    ui->InterpolationPageBtn->setFlat(1);
    ui->EulerPageBtn->setFlat(1);
    ui->CurveFittingPageBtn->setFlat(1);
}

void MainWindow::on_InterpolationPageBtn_clicked()
{
    ui->Pages->setCurrentIndex(1);
    ui->RootPageBtn->setFlat(1);
    ui->IntegerationPageBtn->setFlat(1);
    ui->InterpolationPageBtn->setFlat(0);
    ui->EulerPageBtn->setFlat(1);
    ui->CurveFittingPageBtn->setFlat(1);
}

void MainWindow::on_IntegerationPageBtn_clicked()
{
    ui->Pages->setCurrentIndex(2);
    ui->RootPageBtn->setFlat(1);
    ui->IntegerationPageBtn->setFlat(0);
    ui->InterpolationPageBtn->setFlat(1);
    ui->EulerPageBtn->setFlat(1);
    ui->CurveFittingPageBtn->setFlat(1);

    for (int i = 1; i <= 3; ++i) {
        if (auto *item = comboItem(ui->IntMethodSelector, i))
            item->setEnabled(false);
    }
}

void MainWindow::on_EulerPageBtn_clicked()
{
    ui->Pages->setCurrentIndex(3);
    ui->RootPageBtn->setFlat(1);
    ui->IntegerationPageBtn->setFlat(1);
    ui->InterpolationPageBtn->setFlat(1);
    ui->EulerPageBtn->setFlat(0);
    ui->CurveFittingPageBtn->setFlat(1);
}

void MainWindow::on_CurveFittingPageBtn_clicked()
{
    ui->Pages->setCurrentIndex(4);
    ui->RootPageBtn->setFlat(1);
    ui->IntegerationPageBtn->setFlat(1);
    ui->InterpolationPageBtn->setFlat(1);
    ui->EulerPageBtn->setFlat(1);
    ui->CurveFittingPageBtn->setFlat(0);
}

///////////////////////////////////////////////////////////////////////////      Root      //////////////////////////////////////////////////////////////

/**
 * @brief Handles the “Solve Root” button click.
 *
 * Reads the user’s function f(x) and tolerance, chooses the selected method
 * (Bisection, Secant, or Newton), computes the root, and populates the UI
 * with the result table and info text.
 *
 * @note Assumes `RootSolver` is a utility with methods:
 *       - make_full_parser(symbol): returns a parser for expressions in x
 *       - findBracket(fx, x, a, b): finds [a,b] where sign(f(a))≠sign(f(b))
 *       - bisectionMethod, secantMethod, newtonMethod → each returns a
 *         `RootReturn` struct containing:
 *           · double Root            – the computed root
 *           · map<char,vector<double>> RootVariables
 *             (keys 'a','b','x' as needed) – iteration history
 *
 * @throws Displays a QMessageBox warning if:
 *         - The equation field is empty
 *         - No method is selected
 *         - The parser can’t understand the equation
 */
void MainWindow::on_RootSolveButton_clicked()
{
    // 1. Validate inputs
    const QString eqText = ui->RootEQInput->text();
    if (eqText.isEmpty()) {
        QMessageBox::warning(this, "Empty Equation", "Please enter F(x)!");
        return;
    }
    const int methodIndex = ui->MethodSelector->currentIndex();
    if (methodIndex == 0) {
        QMessageBox::warning(this, "Empty Method", "Please choose a method!");
        return;
    }

    // 2. Parse equation
    const std::string eqString = eqText.toStdString();
    symbol x("x");
    parser p = RootSolver.make_full_parser(x);
    ex fx;
    try {
        fx = p(eqString);
    }
    catch (const std::exception&) {
        QMessageBox::warning(this, "Unsupported", "Wrong or unsupported equation!");
        return;
    }

    // 3. Locate bracket [a,b]
    auto bracket = RootSolver.findBracket(fx, x, 0.0, 100.0);

    // 4. Solve using the chosen method
    const int tol      = ui->RootTol->value();
    RootReturn rootRes;
    switch (methodIndex) {
    case 1:  // Bisection
        rootRes = RootSolver.bisectionMethod(fx, x, bracket, tol, 100);
        break;
    case 2:  // Secant
        rootRes = RootSolver.secantMethod(fx, x, bracket, tol, 100);
        break;
    case 3:  // Newton
        // Precompute derivative (for info display)
        diff(fx, x);
        rootRes = RootSolver.newtonMethod(fx, x, bracket, tol, 100);
        break;
    default:
        return; // should never happen
    }

    // 5. Display root
    ui->RootLabel->setText(QString::number(rootRes.Root));

    // 6. Build iteration table
    // Decide columns based on method
    if (methodIndex == 1) {
        // Bisection: columns a, b, c (midpoint)
        ui->RootTable->setColumnCount(3);
        ui->RootTable->setHorizontalHeaderLabels({"a", "b", "c"});
        const auto &A = rootRes.RootVariables.at('a');
        const auto &B = rootRes.RootVariables.at('b');
        const auto &C = rootRes.RootVariables.at('x');
        const int rows = std::min({A.size(), B.size(), C.size()});
        ui->RootTable->setRowCount(rows);
        for (int i = 0; i < rows; ++i) {
            ui->RootTable->setItem(i, 0, new QTableWidgetItem(
                                             QString::number(A[i], 'f', std::max(5, tol))));
            ui->RootTable->setItem(i, 1, new QTableWidgetItem(
                                             QString::number(B[i], 'f', std::max(5, tol))));
            ui->RootTable->setItem(i, 2, new QTableWidgetItem(
                                             QString::number(C[i], 'f', std::max(5, tol))));
        }
    }
    else {
        // Secant & Newton: columns iteration, x
        ui->RootTable->setColumnCount(2);
        ui->RootTable->setHorizontalHeaderLabels({"Iter", "x"});
        const auto &X = rootRes.RootVariables.at('x');
        const int rows = X.size();
        ui->RootTable->setRowCount(rows);
        for (int i = 0; i < rows; ++i) {
            ui->RootTable->setItem(i, 0, new QTableWidgetItem(
                                             QString::number(i)));
            ui->RootTable->setItem(i, 1, new QTableWidgetItem(
                                             QString::number(X[i], 'f', std::max(5, tol))));
        }
    }
    ui->RootTable->horizontalHeader()->setStretchLastSection(true);

    // 7. Show info summary
    std::ostringstream info;
    info << "Bracket: [" << bracket.first << ", " << bracket.second << "]\n"
         << "Iterations: " << rootRes.RootVariables.at('x').size() << "\n";
    if (methodIndex == 3) {
        // Append derivative expression for Newton
        ex df = diff(fx, x);
        info << "f'(x) = " << df << "\n";
    }
    ui->RootInfo->setPlainText(QString::fromStdString(info.str()));
}

/////////////////////////////////////////////////////////////////////////// Interpolation //////////////////////////////////////////////////////////////

void MainWindow::on_TablePoints_valueChanged(int points)
{
    ui->InterpolationTable->setColumnCount(points);
}

void MainWindow::on_InterpolationSolveButton_clicked()
{
    int MethodIndex = ui->InterpolationMethodSelector->currentIndex();
    int Points = ui->TablePoints->value();
    if(MethodIndex == 0) {
        QMessageBox::warning(this, "Empty Method", "Please Choose a Method first!");
        return;
    }

    QTableWidget *table = ui->InterpolationTable;
    int colCount = table->columnCount();

    std::vector<double> x, y;

    for (int col = 0; col < colCount; ++col) {
        bool ok1, ok2;
        double a = table->item(0, col) ? table->item(0, col)->text().toDouble(&ok1) : 0;
        double b = table->item(1, col) ? table->item(1, col)->text().toDouble(&ok2) : 0;

        if (ok1 && ok2) {
            x.push_back(a);
            y.push_back(b);
        }
    }

    // 1. Empty Check
    if (x.empty() || y.empty()) {
        QMessageBox::warning(this, "Empty Data", "Please fill x and y values in the table.");
        return;
    }

    // 2. Mismatch Check
    if (x.size() < Points) {
        QMessageBox::warning(this, "Mismatch", "x and y must have the same number of values.");
        return;
    }

    // 3. Get min & max of x
    auto [min_it, max_it] = std::minmax_element(x.begin(), x.end());
    double x_min = *min_it;
    double x_max = *max_it;

    qDebug() << "x min:" << x_min << ", x max:" << x_max;

    double solve_x = ui->InterpolationX->value();

    if(solve_x > x_max || solve_x < x_min){
        QString Warning = QString::fromStdString("enter a valid number between " + to_string(x_min) + " and " + to_string(x_max) + " .");
        QMessageBox::warning(this, "Out of range", Warning);
        return;
    }

    auto *ansTable = ui->InterpolAnsTable;
    symbol sym("x");

    if(MethodIndex == 1){ // Lagranch
        InterpolationReturn Sol = InterpolSolver.lagrangeInterpolation(x,y,solve_x,sym);


        ansTable->setColumnCount(3);
        ansTable->setHorizontalHeaderLabels({"L", "L Expresion", "L value"});
        ansTable->setRowCount(Sol.L.size());

        for (int n = 0; n < Sol.L.size(); ++n) {
            // simple preview L0 | L0_ex | L0_val
            QString lName = QString("L%1").arg(n);
            std::ostringstream oss;
            oss << Sol.L[n].first.expand();
            QString lExpr = QString::fromStdString(oss.str());
            QString lVal = QString::number(Sol.L[n].second);

            ansTable->setItem(n, 0, new QTableWidgetItem(lName));
            ansTable->setItem(n, 1, new QTableWidgetItem(lExpr));
            ansTable->setItem(n, 2, new QTableWidgetItem(lVal));
        }

        ui->Point->setText(QString::number(Sol.P.second));

        string info;
        info += "Method: Lagranch.\n\n";

        std::ostringstream oss;
        oss << Sol.P.first.expand();
        info += "P(x) = " + oss.str();

        ui->InterpolationInfo->insertPlainText(QString::fromStdString(info));
    }

    if (MethodIndex == 2){
        InterpolationReturn Sol = InterpolSolver.newtonForwardInterpolation(x,y,solve_x,sym);

        ansTable->setColumnCount(Sol.D.size()+1);
        QStringList header = {"x", "y"};
        for (int i = 1; i <= Sol.D.size()-1; ++i) {
            header.push_back(QString::fromStdString("Δ" + to_string(i)));
        }
        ansTable->setHorizontalHeaderLabels(header);
        ansTable->setRowCount(x.size());
        vector<vector<double>> T;
        T.push_back(x);

        for (auto D : Sol.D) {
            T.push_back(D);
        }

            for (int i = 0; i < T.size(); ++i) {
                for (int n = 0; n < T[i].size(); ++n) {
                    ansTable->setItem(n, i, new QTableWidgetItem(QString::number(T[i][n])));
                }
            }

        // Print D matrix (Newton data)
        string info = "Method: Newton Forward.\n\n";
        double x_mid = (x_max + x_min)/2;

        info += "Best Method for this point is ";
        if (solve_x < x_mid) {
            info += "Forward";
        } else if (solve_x > x_mid) {
            info += "Backward";
        } else {
            info += "Either Work";
        }
        info += "\n\n";
        ostringstream oss;
        oss << "P(x) = " << Sol.P.first.expand();
        info += oss.str() + "\n\n";




        // Show in UI
        ui->InterpolationInfo->insertPlainText(QString::fromStdString(info));

        ui->Point->setText(QString::number(Sol.P.second));

    }

    if(MethodIndex == 3){
        InterpolationReturn Sol = InterpolSolver.newtonBackwardInterpolation(x,y,solve_x,sym);

        ansTable->setColumnCount(Sol.D.size()+1);
        QStringList header = {"x", "y"};
        for (int i = 1; i <= Sol.D.size()-1; ++i) {
            header.push_back(QString::fromStdString("Δ" + to_string(i)));
        }
        ansTable->setHorizontalHeaderLabels(header);
        ansTable->setRowCount(x.size());
        vector<vector<double>> T;
        T.push_back(x);

        for (auto D : Sol.D) {
            T.push_back(D);
        }

        for (int i = 0; i < T.size(); ++i) {
            for (int n = T[0].size() - T[i].size(); n < T[0].size(); ++n) {
                ansTable->setItem(n, i, new QTableWidgetItem(QString::number(T[i][n])));
            }
        }

        // Print D matrix (Newton data)
        string info = "Method: Newton Backward.\n\n";
        double x_mid = (x_max + x_min)/2;

        info += "Best Method for this point is ";
        if (solve_x < x_mid) {
            info += "Forward";
        } else if (solve_x > x_mid) {
            info += "Backward";
        } else {
            info += "Either Work";
        }
        info += "\n\n";
        ostringstream oss;
        oss << "P(x) = " << Sol.P.first.expand();
        info += oss.str() + "\n\n";




        // Show in UI
        ui->InterpolationInfo->insertPlainText(QString::fromStdString(info));

        ui->Point->setText(QString::number(Sol.P.second));
    }


}

///////////////////////////////////////////////////////////////////////////  Integration  //////////////////////////////////////////////////////////////

void MainWindow::on_IntSolveButton_clicked()
{
    // Handling Embty or Invalid Input

}

static QStandardItem* comboItem(QComboBox *combo, int index) {
    if (auto *m = qobject_cast<QStandardItemModel*>(combo->model()))
        return m->item(index);
    return nullptr;
}

void MainWindow::on_StepsInput_valueChanged(int steps)
{
    if (auto *combo = ui->IntMethodSelector) {
        if (auto *item1 = comboItem(combo, 1)) item1->setEnabled(steps >= 1);
        if (auto *item2 = comboItem(combo, 2)) item2->setEnabled(steps % 2 == 1);
        if (auto *item3 = comboItem(combo, 3)) item3->setEnabled(steps % 3 == 0);
    }
}

