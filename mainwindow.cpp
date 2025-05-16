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
    , IntegrSolver(IntegrationMethods())
    , EulerSolver(EulerMethods())

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
 *       - bisection, secant, newton → each returns a
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
    RootResult rootRes;
    switch (methodIndex) {
    case 1:  // Bisection
        rootRes = RootSolver.bisection(fx, x, bracket, tol, 100);
        break;
    case 2:  // Secant
        rootRes = RootSolver.secant(fx, x, bracket, tol, 100);
        break;
    case 3:  // Newton
        // Precompute derivative (for info display)
        diff(fx, x);
        rootRes = RootSolver.newton(fx, x, bracket, tol, 100);
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

/**
 * @brief Handles the “Solve Interpolation” button click.
 *
 * Reads the table of data points (xᵢ, yᵢ), the desired interpolation X, and
 * the selected method (lagrange, Newton Forward, Newton Backward). Computes
 * the interpolating polynomial, fills the answer table with intermediate
 * values, and displays P(X) along with method info.
 *
 * @note Assumes `InterpolSolver` provides:
 *   - lagrange(x, y, X, sym)
 *   - newtonForward(x, y, X, sym)
 *   - newtonBackward(x, y, X, sym)
 *   returning an `InterpolationResult` struct containing:
 *     · P.first  – symbolic polynomial expression
 *     · P.second – numeric P(X) value
 *     · L        – vector of (symbolic basis, numeric value) for lagrange
 *     · D        – vector of forward/backward difference tables
 *
 * @throws Displays a QMessageBox warning if:
 *         - No method is selected
 *         - The data table is empty or mismatched
 */
void MainWindow::on_InterpolationSolveButton_clicked()
{
    // 1. Validate method selection
    const int methodIndex = ui->InterpolationMethodSelector->currentIndex();
    if (methodIndex == 0) {
        QMessageBox::warning(this, "Empty Method", "Please choose an interpolation method!");
        return;
    }

    // 2. Read points from the UI table
    QTableWidget *table = ui->InterpolationTable;
    const int cols = table->columnCount();
    std::vector<double> x_vals, y_vals;
    for (int c = 0; c < cols; ++c) {
        bool okx = false, oky = false;
        double xv = table->item(0, c) ? table->item(0, c)->text().toDouble(&okx) : 0.0;
        double yv = table->item(1, c) ? table->item(1, c)->text().toDouble(&oky) : 0.0;
        if (okx && oky) {
            x_vals.push_back(xv);
            y_vals.push_back(yv);
        }
    }
    // 3. Check we got any data
    if (x_vals.empty()) {
        QMessageBox::warning(this, "Empty Data", "Please fill in x and y values!");
        return;
    }

    // 4. Ensure enough points
    const int requiredPoints = ui->TablePoints->value();
    if (static_cast<int>(x_vals.size()) < requiredPoints) {
        QMessageBox::warning(this, "Mismatch", "Not enough (x,y) pairs for the chosen number of points.");
        return;
    }

    // 5. Compute x_min, x_max for info (optional range checks)
    auto [minIt, maxIt] = std::minmax_element(x_vals.begin(), x_vals.end());
    const double x_min = *minIt, x_max = *maxIt;
    const double X = ui->InterpolationX->value();  // the point to evaluate

    // 6. Prepare output table & symbol
    QTableWidget *outTable = ui->InterpolAnsTable;
    symbol sym("x");
    InterpolationResult result;

    // 7. Dispatch to the chosen interpolation method
    switch (methodIndex) {
    case 1:  // Lagrange
        result = InterpolSolver.lagrange(x_vals, y_vals, X, sym);

        // Setup 3 columns: basis name, symbolic expr, numeric value
        outTable->setColumnCount(3);
        outTable->setHorizontalHeaderLabels({"Lᵢ", "Expression", "Value"});
        outTable->setRowCount(result.L.size());
        for (int i = 0; i < static_cast<int>(result.L.size()); ++i) {
            const auto &basis = result.L[i];
            // Lᵢ label
            outTable->setItem(i, 0, new QTableWidgetItem(
                                        QString("L%1").arg(i)));
            // symbolic expression
            std::ostringstream oss;
            oss << basis.first.expand();
            outTable->setItem(i, 1, new QTableWidgetItem(
                                        QString::fromStdString(oss.str())));
            // numeric value at X
            outTable->setItem(i, 2, new QTableWidgetItem(
                                        QString::number(basis.second)));
        }
        break;

    case 2:  // Newton Forward
        result = InterpolSolver.newtonForward(x_vals, y_vals, X, sym);

        // Build difference table header: x, y, Δ¹, Δ², …
        {
            int levels = result.D.size();
            QStringList hdr = {"x", "y"};
            for (int lev = 1; lev < levels; ++lev)
                hdr << QString("Δ%1").arg(lev);
            outTable->setColumnCount(hdr.size());
            outTable->setHorizontalHeaderLabels(hdr);
            outTable->setRowCount(x_vals.size());

            // Pack x, y, then each Δ-level
            std::vector<std::vector<double>> tableData;
            tableData.push_back(x_vals);
            tableData.push_back(y_vals);
            for (auto &Dlev : result.D) tableData.push_back(Dlev);

            // Fill the widget
            for (int col = 0; col < static_cast<int>(tableData.size()); ++col) {
                for (int row = 0; row < static_cast<int>(tableData[col].size()); ++row) {
                    outTable->setItem(row, col, new QTableWidgetItem(
                                                    QString::number(tableData[col][row])));
                }
            }
        }
        break;

    case 3:  // Newton Backward
        result = InterpolSolver.newtonBackward(x_vals, y_vals, X, sym);

        // Similar to forward but align bottom-up
        {
            int levels = result.D.size();
            QStringList hdr = {"x", "y"};
            for (int lev = 1; lev < levels; ++lev)
                hdr << QString("Δ%1").arg(lev);
            outTable->setColumnCount(hdr.size());
            outTable->setHorizontalHeaderLabels(hdr);
            outTable->setRowCount(x_vals.size());

            std::vector<std::vector<double>> tableData;
            tableData.push_back(x_vals);
            tableData.push_back(y_vals);
            for (auto &Dlev : result.D) tableData.push_back(Dlev);

            // For backward, rows shift so last entries align
            int n = x_vals.size();
            for (int col = 0; col < static_cast<int>(tableData.size()); ++col) {
                int len = tableData[col].size();
                for (int i = 0; i < len; ++i) {
                    int row = n - len + i;
                    outTable->setItem(row, col, new QTableWidgetItem(
                                                    QString::number(tableData[col][i])));
                }
            }
        }
        break;

    default:
        // Should never happen
        return;
    }

    // 8. Display the interpolation result and info
    ui->Point->setText(QString::number(result.P.second));

    std::ostringstream info;
    if (methodIndex == 1)       info << "Method: Lagrange\n\n";
    else if (methodIndex == 2)  info << "Method: Newton Forward\n\n";
    else                         info << "Method: Newton Backward\n\n";

    // Hint on best variant if using Newton
    if (methodIndex > 1) {
        double mid = 0.5*(x_min + x_max);
        info << "Best around X=" << X << ": "
             << ((X < mid) ? "Forward" : (X > mid) ? "Backward" : "Either")
             << "\n\n";
    }

    // Show the polynomial itself
    info << "P(x) = " << result.P.first.expand();
    ui->InterpolationInfo->setPlainText(QString::fromStdString(info.str()));
}

///////////////////////////////////////////////////////////////////////////  Integration  //////////////////////////////////////////////////////////////

static QStandardItem* comboItem(QComboBox *combo, int index) {
    if (auto *m = qobject_cast<QStandardItemModel*>(combo->model()))
        return m->item(index);
    return nullptr;
}

void MainWindow::on_IntSolveButton_clicked()
{
    // Handling Embty or Invalid Input
    const QString eqText = ui->IntEQInput->text();
    if (eqText.isEmpty()) {
        QMessageBox::warning(this, "Empty Equation", "Please enter F(x)!");
        return;
    }

    const int methodIndex = ui->IntMethodSelector->currentIndex();
    if (methodIndex == 0) {
        QMessageBox::warning(this, "Empty Method", "Please choose a method!");
        return;
    }

    int a, b, n;
    a = ui->LowerBoundInput->value();
    b = ui->UpperBoundInput->value();
    n = ui->StepsInput->value();

    if (n <= 0){
        QMessageBox::warning(this, "Invalid Input", "Enter a Valid Steps Value!  ");
        return;
    }

    if(a >= b){
        QMessageBox::warning(this, "Invalid Input", "Lower Bound should be less than Upper Bound!  ");
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

    IntegrationResult Result;

    switch (methodIndex) {
    case 1:
        Result = IntegrSolver.trapezoidal(fx, x, a, b, n);
        break;
    case 2:
        Result = IntegrSolver.simpsonOneThird(fx, x, a, b, n);
        break;
    case 3:
        Result = IntegrSolver.simpsonThreeEighth(fx, x, a, b, n);
    default:
        break;
    }

    QTableWidget *table = ui->IntTable;
    table->clearContents();

    // 2 rows: one for x, one for f(x)
    int m = static_cast<int>(Result.X.size());
    table->setRowCount(2);
    table->setColumnCount(m);

    // Label the rows “x” and “f(x)”
    table->setVerticalHeaderLabels({ "x", "f(x)" });

    // (Optionally) label each column by its index or x-value
    QStringList colLabels;
    colLabels.reserve(m);
    for (int j = 0; j < m; ++j) {
        // e.g. label by index:
        colLabels << QString::number(j);
        // or if you prefer to show the x-value as header:
        // colLabels << QString::number(Result.X[j], 'g', 6);
    }
    table->setHorizontalHeaderLabels(colLabels);

    // Fill row 0 with all the x’s, row 1 with all the f(x)’s
    for (int j = 0; j < m; ++j) {
        table->setItem(0, j, new QTableWidgetItem(QString::number(Result.X[j])));
        table->setItem(1, j, new QTableWidgetItem(QString::number(Result.FX[j])));
    }

    // Stretch so it fills the width nicely
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()  ->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->IntLabel->setText(QString::number(Result.I, 'g', 10));

    // 6. Show summary info
    QString info;
    info += "Method: " + ui->MethodSelector->currentText() + "\n";
    info += "Intervals (n): " + QString::number(n) + "\n";
    info += "Step size (h): " + QString::number(Result.h, 'g', 10) + "\n";
    info += "Integral ≈ " + QString::number(Result.I, 'g', 10) + "\n";
    ui->IntInfo->setPlainText(info);
}

void MainWindow::on_StepsInput_valueChanged(int steps)
{
    ui->IntMethodSelector->setCurrentIndex(0);
    if (auto *combo = ui->IntMethodSelector) {
        if (auto *item1 = comboItem(combo, 1)) item1->setEnabled(steps >= 1);
        if (auto *item2 = comboItem(combo, 2)) item2->setEnabled(steps % 2 == 0);
        if (auto *item3 = comboItem(combo, 3)) item3->setEnabled(steps % 3 == 0);
    }
}

///////////////////////////////////////////////////////////////////////////     Euler     ///////////////////////////////////////////////////////////////////

void MainWindow::on_X_eq_option_clicked(bool checked)
{
    ui->X_eq_input->setEnabled(checked);
    ui->X_range_low->setEnabled(!checked);
    ui->X_range_high->setEnabled(!checked);
}

void MainWindow::on_X_range_clicked(bool checked)
{
    ui->X_eq_input->setEnabled(!checked);
    if(ui->EulerMethodSelector->currentIndex() == 2){
        ui->X_range_low->setEnabled(!checked);
        ui->X_range_low->setValue(ui->X0Input->value());
    }
    else
        ui->X_range_low->setEnabled(checked);

    ui->X_range_high->setEnabled(checked);
}

void MainWindow::on_EulerSolveButton_clicked()
{
    // 1. Validate inputs
    const QString eqText = ui->EulerEQInput->text();
    if (eqText.isEmpty()) {
        QMessageBox::warning(this, "Empty Equation", "Please enter F(x)!");
        return;
    }

    const int methodIndex = ui->EulerMethodSelector->currentIndex();
    if (methodIndex == 0) {
        QMessageBox::warning(this, "Empty Method", "Please choose a method!");
        return;
    }

    const double steps = ui->EulerStepsInput->value();
    if (steps <= 0) {
        QMessageBox::warning(this, "Invalid Steps", "Please enter a valid number of steps!");
        return;
    }

    // 2. Parse equation
    const std::string eqString = eqText.toStdString();
    symbol x("x"), y("y");
    parser p;
    p.get_syms()["x"] = x;
    p.get_syms()["y"] = y;
    ex fxy;
    try {
        fxy = p(eqString);
    }
    catch (const std::exception&) {
        QMessageBox::warning(this, "Unsupported", "Wrong or unsupported equation!");
        return;
    }

    // 3. Get initial conditions
    double x0 = ui->X0Input->value();
    double y0 = ui->Y0Input->value();
    double h = ui->EulerStepsInput->value(); // Step size will be calculated later
    EulerResult result;


    // 6. Display results in the UI
    QTableWidget *table = ui->EulerResultsTable;
    table->clearContents();


    if (methodIndex == 1){

        if (ui->X_eq_option->isChecked()) {
            double x_ = ui->X_eq_input->value();
            result = EulerSolver.Euler(fxy, x, y, x0, y0, x_, h);
        } else if (ui->X_range->isChecked()) {
            double xs = ui->X_range_low->value();
            double xe = ui->X_range_high->value();
            result = EulerSolver.Euler(fxy, x, y, x0, y0, {xs, xe}, h);

        }
        table->setColumnCount(3); // Two rows for X and Y
        table->setRowCount(result.X.size());
        table->setHorizontalHeaderLabels({"X", "Y", "h f(x, y)"});

        for (int i = 0; i < result.X.size(); ++i) {
            table->setItem(i, 0, new QTableWidgetItem(QString::number(result.X[i])));
            table->setItem(i, 1, new QTableWidgetItem(QString::number(result.Y[i])));
            if(i < result.X.size() - 1)
            table->setItem(i, 2, new QTableWidgetItem(QString::number(h * result.Fxy[i])));
        }

        // 7. Show summary info
        QString info;
        ui->EulerInfo->selectAll();
        ui->EulerInfo->cut();

        info += "Method: Euler \n";
        info += "Initial Condition: (x0, y0) = (" + QString::number(x0) + ", " + QString::number(y0) + ")\n";
        info += "Step Size (h): " + QString::number(h, 'g', 10) + "\n";
        ui->EulerInfo->setPlainText(info);


    }

    if (methodIndex == 2){

        double x_;
        if (ui->X_eq_option->isChecked()) {
            x_ = ui->X_eq_input->value();
        } else if (ui->X_range->isChecked()) {
            x_ = ui->X_range_high->value();
        }
        result = EulerSolver.ModifiedEuler(fxy, x, y, x0, y0, x_, h);

        table->setColumnCount(7); // Two rows for X and Y
        table->setRowCount(result.X.size());
        table->setHorizontalHeaderLabels({"X", "Yn", "f(x, y)", "Y(n)n+1", "Xn+1", "f(Xn+1, Y(n)n+1)", "Y(n+1)n+1"});

        for (int i = 0; i < result.X.size(); ++i) {
            table->setItem(i, 0, new QTableWidgetItem(QString::number(result.X[i])));
            table->setItem(i, 1, new QTableWidgetItem(QString::number(result.Y[i])));
            if(i < result.X.size() - 1){
                table->setItem(i, 2, new QTableWidgetItem(QString::number(result.Fxy[i])));
                table->setItem(i, 3, new QTableWidgetItem(QString::number(result.Y_P[i])));
                table->setItem(i, 4, new QTableWidgetItem(QString::number(result.X[i+1])));
                table->setItem(i, 5, new QTableWidgetItem(QString::number(result.Fxy_P[i])));
                table->setItem(i, 6, new QTableWidgetItem(QString::number(result.Y[i+1])));
            }
        }

        // 7. Show summary info
        QString info;
        ui->EulerInfo->selectAll();
        ui->EulerInfo->cut();

        info += "Method: Modified Euler \n";
        info += "Initial Condition: (x0, y0) = (" + QString::number(x0) + ", " + QString::number(y0) + ")\n";
        info += "Step Size (h): " + QString::number(h, 'g', 10) + "\n";
        ui->EulerInfo->setPlainText(info);
    }

}

void MainWindow::on_X0Input_valueChanged(double arg1)
{
    if(ui->EulerMethodSelector->currentIndex() == 2){
        if(ui->X_range->isChecked()){
            ui->X_range_low->setValue(arg1);
        }
    }
}

///////////////////////////////////////////////////////////////////////////     Curve     ///////////////////////////////////////////////////////////////////


void MainWindow::on_CurveMethodSelector_currentIndexChanged(int index)
{
    QString example;
    switch (index) {
    case 0:
        example = "";
        break;
    case 1:
        example = "y = ax + b";
        break;
    case 2:
        example = "y = ax^2 + bx + c";
        break;
    case 3:
        example = "y = a e^(bx)";
        break;
    case 4:
        example = "y = a x^b";
        break;
    case 5:
        example = "y = b a^x";
        break;
    default:
        break;
    }

    ui->CurveExampleLabel->setText(example);
}


void MainWindow::on_CurveCustomXCheck_clicked(bool checked)
{
    ui->CurveCustomX->setEnabled(checked);
    if(!checked){
        ui->CurveCustomX->clear();
    }
}


void MainWindow::on_CurveCustomYCheck_clicked(bool checked)
{
    ui->CurveCustomY->setEnabled(checked);
    if(!checked){
        ui->CurveCustomY->clear();
    }

}


void MainWindow::on_CurveTablePoints_valueChanged(int arg1)
{
    ui->CurveInputTable->setColumnCount(arg1);
}


void MainWindow::on_CurveSolveButton_clicked()
{
        const int methodIndex = ui->CurveMethodSelector->currentIndex();
    if (methodIndex == 0) {
        QMessageBox::warning(this, "Empty Method", "Please choose an interpolation method!");
        return;
    }
    qDebug() << "1 con pass\n";

    QTableWidget *table = ui->CurveInputTable;
    const int cols = table->columnCount();
    std::vector<double> x_vals, y_vals;
    for (int c = 0; c < cols; ++c) {
        bool okx = false, oky = false;
        double xv = table->item(0, c) ? table->item(0, c)->text().toDouble(&okx) : 0.0;
        double yv = table->item(1, c) ? table->item(1, c)->text().toDouble(&oky) : 0.0;
        if (okx && oky) {
            x_vals.push_back(xv);
            y_vals.push_back(yv);
        }
    }
    qDebug() << "Table has been read!\n";
    qDebug() << "x vals: "<< x_vals << "\n";
    qDebug() << "y vals: " << y_vals << "\n";

    if (x_vals.empty()) {
        QMessageBox::warning(this, "Empty Data", "Please fill in x and y values!");
        return;
    }

    const int requiredPoints = ui->CurveTablePoints->value();
    if (static_cast<int>(x_vals.size()) < requiredPoints) {
        QMessageBox::warning(this, "Mismatch", "Not enough (x,y) pairs for the chosen number of points.");
        return;
    }
    qDebug() << "Points is valid!\n";

    QString CustomX, CustomY;

    if (ui->CurveCustomXCheck->isChecked()) {
        qDebug() << "Custom X enabled\n";
        QString inputX = ui->CurveCustomX->text().trimmed();
        if (inputX.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Custom X value cannot be empty.");
        } else {
            CustomX = inputX;
            qDebug() << "Custom X found!\n";
        }
    } else {
        qDebug() << "Custom X disabled\n";
        CustomX = "x";
    }
    qDebug() << "Custom X done!\n";

    if (ui->CurveCustomYCheck->isChecked()) {
        QString inputY = ui->CurveCustomY->text().trimmed();
        if (inputY.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Custom Y value cannot be empty.");
        } else {
            CustomY = inputY;
        }
    } else {
        CustomY = "y";
    }
    qDebug() << "Custom Y done!\n";

    ex c_x, c_y;
    symbol x("x"), y("y");
    parser px, py;
    px.get_syms()["x"] = x;
    py.get_syms()["y"] = y;

    try{
        c_x = px(CustomX.toStdString());
    }
    catch (const std::exception& e) {
        QMessageBox::warning(this, "Unsupported", "Wrong or unsupported equation in X   ");
        return;
    }

    try{
        c_y = py(CustomY.toStdString());
    }
    catch (const std::exception& e) {
        QMessageBox::warning(this, "Unsupported", "Wrong or unsupported equation in Y");
        return;

    }
    qDebug() << "exp Done for X, Y!\n";

    ostringstream info;


    // 5. Solve the equation
    CurveResult result;
    if (methodIndex == 1) // y = ax + b
    {
        result = CurveSolver.linear(c_x, c_y, x_vals, y_vals, x, y);
        info << "Model: y = a·x + b\n";
        info << "Normal equations:\n";
        info << "  ∑y = a∑x + n·b\n";
        info << "  ∑x·y = a∑x² + b∑x\n";
    }
    else if (methodIndex == 2) // y = ax^2 + bx + c
    {
        result = CurveSolver.quadric(c_x, c_y, x_vals, y_vals, x, y);
        info << "Model: y = a·x² + b·x + c\n";
        info << "Normal equations:\n";
        info << "  ∑y = a∑x² + b∑x + n·c\n";
        info << "  ∑x·y = a∑x³ + b∑x² + c∑x\n";
        info << "  ∑x²·y = a∑x⁴ + b∑x³ + c∑x²\n";
    }
    else if (methodIndex == 3) // y = a e^(bx)
    {
        result = CurveSolver.exponential(c_x, c_y, x_vals, y_vals, x, y);
        info << "Linearized model: ln(y) = ln(a) + b·x\n";
        info << "Linearized model: Y = A + b·x\n";
        info << "Normal equations:\n";
        info << "  ∑ln(y) = n·ln(a) + b∑x\n";
        info << "  ∑x·ln(y) = ln(a)∑x + b∑x²\n";
    }
    else if (methodIndex == 4) // y = a x^b
    {
        result = CurveSolver.power1(c_x, c_y, x_vals, y_vals, x, y);
        info << "Linearized model: ln(y) = ln(a) + b·ln(x)\n";
        info << "Linearized model: Y = A + b·X\n";
        info << "Normal equations:\n";
        info << "  ∑ln(y) = n·ln(a) + b∑ln(x)\n";
        info << "  ∑ln(x)·ln(y) = ln(a)∑ln(x) + b∑[ln(x)]²\n";
    }
    else if (methodIndex == 5) // y = b a^x
    {
        result = CurveSolver.power2(c_x, c_y, x_vals, y_vals, x, y);
        info << "Linearized model: ln(y) = ln(b) + x·ln(a)\n";
        info << "Linearized model: Y = B + x·A\n";
        info << "Normal equations:\n";
        info << "  ∑ln(y) = n·ln(b) + ln(a)∑x\n";
        info << "  ∑x·ln(y) = ln(b)∑x + ln(a)∑x²\n";
    }


    // 6. Display the result
    auto *restable = ui->CurveResultsTable;
    restable->clear(); // Clears all items and headers [[4]]

    // Configure table based on methodIndex
    const int dataRowCount = result.X.size();
    const int totalRowCount = dataRowCount + 1; // +1 for sum row
    const bool isMethod2 = (methodIndex == 2);

    // Set up columns
    if (isMethod2) {
        restable->setColumnCount(9);
        restable->setHorizontalHeaderLabels({"x", "y", "X", "Y", "XY", "X²", "X²Y", "X³", "X⁴"});
    } else {
        restable->setColumnCount(6);
        restable->setHorizontalHeaderLabels({"x", "y", "X", "Y", "XY", "X²"});
    }

    restable->setRowCount(totalRowCount);

    // Populate data rows
    for (int i = 0; i < dataRowCount; ++i) {
        // Common columns for all methods
        restable->setItem(i, 0, new QTableWidgetItem(QString::number(x_vals[i])));
        restable->setItem(i, 1, new QTableWidgetItem(QString::number(y_vals[i])));
        restable->setItem(i, 2, new QTableWidgetItem(QString::number(result.X[i])));
        restable->setItem(i, 3, new QTableWidgetItem(QString::number(result.Y[i])));
        restable->setItem(i, 4, new QTableWidgetItem(QString::number(result.XY[i])));
        restable->setItem(i, 5, new QTableWidgetItem(QString::number(result.X2[i])));

        // Additional columns for method 2
        if (isMethod2) {
            restable->setItem(i, 6, new QTableWidgetItem(QString::number(result.X2Y[i])));
            restable->setItem(i, 7, new QTableWidgetItem(QString::number(result.X3[i])));
            restable->setItem(i, 8, new QTableWidgetItem(QString::number(result.X4[i])));
        }
    }

    // Set sum row values
    const int sumRow = dataRowCount;
    std::vector<double> sums = {
        result.sum_X, result.sum_Y, result.sum_XY,
        result.sum_X2, result.sum_X2Y, result.sum_X3, result.sum_X4
    };

    for (int col = 2; col < restable->columnCount(); ++col) {
        if (col - 2 < sums.size()) {
            restable->setItem(sumRow, col, new QTableWidgetItem(QString::number(sums[col - 2])));
        }
    }
    info << "---------------------------------------------------\n\n";
    info << "Variables:\n\n";
    info << "∑X = " << sums[0] << endl;
    info << "∑Y = " << sums[1] << endl;
    info << "∑XY = " << sums[2] << endl;
    info << "∑X² = " << sums[3] << endl;
    if(isMethod2){
        info << "∑X²Y = " << sums[4] << endl;
        info << "∑X³ = " << sums[5] << endl;
        info << "∑X⁴ = " << sums[6] << endl;
    }
    info << "\na = " << result.a << endl;
    info << "b = " << result.b << endl;
    if (isMethod2){
        info << "c = " << result.c << endl;
    }
    ex form;
    if (methodIndex == 1){
        form = c_y == result.a *(c_x) + result.b;
    } else if (methodIndex == 2) {
        form = c_y == result.a *pow(c_x, 2) + result.b * (c_x) + result.c;
    } else if (methodIndex == 3){
        form = c_y == result.a * exp(result.b * c_x);
    } else if (methodIndex == 4){
        form = c_y == result.a * pow(c_x, result.b) ;
    }else if (methodIndex == 5){
        form = c_y == result.b * pow(result.a,c_x);
    }
    info << "---------------------------------------------------\n\n";
    info << endl << "Final Formula:\n\n" << form.expand() << endl;

    ui->CurveInfo->setPlainText(QString::fromStdString(info.str()));



}


void MainWindow::on_CurveCustomXCheck_2_clicked(bool checked)
{

}

