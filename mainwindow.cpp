#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , RootSolver(RootMethods())
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
