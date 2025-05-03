#include "mainwindow.h"
#include "ui_mainwindow.h"

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

void MainWindow::on_TablePoints_valueChanged(int arg1)
{
    ui->InterpolationTable->setColumnCount(arg1);
}

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

void MainWindow::on_RootSolveButton_clicked()
{
    if (ui->RootEQInput->text().isEmpty()) {
        QMessageBox::warning(this, "Empty Equation", "     Please Enter F(x)!      ");

    } else {
        if (ui->MethodSelector->currentIndex() == 0) {
            QMessageBox::warning(this, "Empty Method", "  Please Choose a Method!  ");

        } else {
            string eq = ui->RootEQInput->text().toStdString();
            int tol_ = ui->RootTol->value();
            symbol x("x");
            parser p = RootSolver.make_full_parser(x);
            ex fx;

            try {
                fx = p(eq);
            } catch (const exception &e) {
                QMessageBox::warning(this, "Unspported", "Wrong or Unspported Equation!");
                return;
            }
            auto bracket = RootSolver.findBracket(fx, x, 0, 100);
            setprecision(tol_);
            RootReturn root;

            int method = ui->MethodSelector->currentIndex();

            if (method == 1) {
                root = RootSolver.bisectionMethod(fx, x, bracket, tol_, 100);
                ui->RootLabel->setText(QString::number(root.Root));

                ui->RootTable->setColumnCount(3);
                QStringList headers;
                headers << "a" << "b" << "c";
                headers << "d";
                ui->RootTable->setHorizontalHeaderLabels(headers);
                ui->RootTable->horizontalHeader()->setStretchLastSection(0);

                // Get vectors from RootReturn
                const std::vector<double> &a_values = root.RootVariables['a'];
                const std::vector<double> &b_values = root.RootVariables['b'];
                const std::vector<double> &x_values = root.RootVariables['x'];

                // Determine row count (use the smallest size to avoid out-of-range)
                int rowCount = std::min({a_values.size(), b_values.size(), x_values.size()});
                ui->RootTable->setRowCount(rowCount);
                std::cout << std::fixed << std::setprecision(17);
                // Fill table
                for (int i = 0; i < rowCount; ++i) {
                    ui->RootTable->setItem(i,
                                           0,
                                           new QTableWidgetItem(
                                               QString::number(a_values[i], 'f', max(5, tol_))));
                    ui->RootTable->setItem(i,
                                           1,
                                           new QTableWidgetItem(
                                               QString::number(b_values[i], 'f', max(5, tol_))));
                    ui->RootTable->setItem(i,
                                           2,
                                           new QTableWidgetItem(
                                               QString::number(x_values[i], 'f', max(5, tol_))));
                }

                //Fill info
                string info;
                info = "Sign changed between:\n";
                info += "a = " + to_string(bracket.first) + " and b = " + to_string(bracket.second)
                        + "\n";
                info += "Method Took " + to_string(x_values.size())
                        + " iterations to find the root.\n";

                ui->RootInfo->setPlainText(QString::fromStdString(info));
            }

            if (method == 2) {
                root = RootSolver.secantMethod(fx, x, bracket, tol_, 100);
                ui->RootLabel->setText(QString::number(root.Root));

                ui->RootTable->setColumnCount(2);
                QStringList headers;
                headers << "x" << "value";
                ui->RootTable->setHorizontalHeaderLabels(headers);
                ui->RootTable->horizontalHeader()->setStretchLastSection(1);

                // Get vectors from RootReturn
                const std::vector<double> &x_values = root.RootVariables['x'];

                // Determine row count (use the smallest size to avoid out-of-range)
                int rowCount = x_values.size();
                ui->RootTable->setRowCount(rowCount);

                // Fill table
                for (int i = 0; i < rowCount; ++i) {
                    ui->RootTable->setItem(i,
                                           0,
                                           new QTableWidgetItem(
                                               QString::fromStdString("x" + to_string(i))));
                    ui->RootTable->setItem(i,
                                           1,
                                           new QTableWidgetItem(
                                               QString::number(x_values[i], 'f', max(5, tol_))));
                }

                //Fill info
                string info;
                info = "Sign changed between:\n";
                info += "a = " + to_string(bracket.first) + " and b = " + to_string(bracket.second)
                        + "\n";
                info += "Method Took " + to_string(x_values.size())
                        + " iterations to find the root.\n";

                ui->RootInfo->setPlainText(QString::fromStdString(info));
            }

            if (method == 3) {
                ex df_expr = diff(fx, x);

                root = RootSolver.newtonMethod(fx, x, bracket, tol_, 100);
                ui->RootLabel->setText(QString::number(root.Root));

                ui->RootTable->setColumnCount(2);
                QStringList headers;
                headers << "x" << "value";
                ui->RootTable->setHorizontalHeaderLabels(headers);
                ui->RootTable->horizontalHeader()->setStretchLastSection(1);

                // Get vectors from RootReturn
                const std::vector<double> &x_values = root.RootVariables['x'];

                // Determine row count (use the smallest size to avoid out-of-range)
                int rowCount = x_values.size();
                ui->RootTable->setRowCount(rowCount);

                // Fill table
                for (int i = 0; i < rowCount; ++i) {
                    ui->RootTable->setItem(i,
                                           0,
                                           new QTableWidgetItem(
                                               QString::fromStdString("x" + to_string(i))));
                    ui->RootTable->setItem(i,
                                           1,
                                           new QTableWidgetItem(
                                               QString::number(x_values[i], 'f', max(5, tol_))));
                }

                //Fill info
                string info;
                info = "Sign changed between:\n";
                info += "a = " + to_string(bracket.first) + " and b = " + to_string(bracket.second)
                        + "\n\n";
                info += "Method Took " + to_string(x_values.size())
                        + " iterations to find the root.\n";
                std::ostringstream oss;
                oss << df_expr;
                info += "\n\n dfx = " + oss.str() + "\n";
                ui->RootInfo->setPlainText(QString::fromStdString(info));
            }
        }
    }
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

