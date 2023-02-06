#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tableType(TableType::Flowers)
{
    ui->setupUi(this);

    database = new ShopDatabase("shop.db");
    database->createTables();

    balance = database->getBalance();

    ui->priceSpinBox->setMinimum(1);
    ui->priceSpinBox->setMaximum(100000);

    ui->salarySpinBox->setMinimum(1);
    ui->salarySpinBox->setMaximum(1000000);

    ui->flowerCountSpinBox->setMinimum(1);
    ui->flowerCountSpinBox->setMaximum(100000);

    ui->sellSpinBox->setMinimum(1);
    ui->sellSpinBox->setMaximum(100000);

    ui->balanceLabel->setFont(QFont("Monospace", 11, QFont::Bold));

    ui->tableWidget->setSortingEnabled(true);

    createMenu();
    createMenuButtons();

    createTreeWidget();

    update();

    connect(database, SIGNAL(updated()), this, SLOT(update()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addFlowers() {

    Flower flower(
                0,
                ui->flowerNameLineEdit->text(),
                ui->colorLineEdit->text(),
                ui->sellSpinBox->value(),
                ui->priceSpinBox->value(),
                ui->flowerCountSpinBox->value()
                );

    int cost = ui->priceSpinBox->value() * ui->flowerCountSpinBox->value();
    if (balance < cost) {
        QMessageBox::critical(this, "Неудача", "Недостаточно средств на балансе для закупки!");
        return;
    }

    balance -= cost;
    database->updateBalance(balance);
    database->addFlower(flower);

    ui->flowerNameLineEdit->clear();
    ui->colorLineEdit->clear();

    ui->sellSpinBox->setValue(1);
    ui->priceSpinBox->setValue(1);
    ui->flowerCountSpinBox->setValue(1);
}

void MainWindow::sellFlowers() {

    int value = database->sellFlowers(
                ui->flowerNameLineEdit->text(),
                ui->flowerCountSpinBox->value()
                );

    if (value == -1) {
        QMessageBox::critical(this, "Неудача", "Искомая позиция не найдена!");
        return;
    }

    balance += value;
    database->updateBalance(balance);

    ui->flowerNameLineEdit->clear();
    ui->colorLineEdit->clear();

    ui->sellSpinBox->setValue(1);
    ui->priceSpinBox->setValue(1);
    ui->flowerCountSpinBox->setValue(1);

    update();
}


void MainWindow::addWorker() {

    Worker worker(
                0,
                ui->workerNameLineEdit->text(),
                ui->salarySpinBox->value()
                );

    database->addWorker(worker);

    ui->workerNameLineEdit->clear();
    ui->salarySpinBox->setValue(1);
}

void MainWindow::fireWorker() {

    bool success = database->removeWorker(ui->workerNameLineEdit->text());

    if (!success) {
        QMessageBox::critical(this, "Неудача", "Искомый работник не найден!");
        return;
    }

    ui->workerNameLineEdit->clear();
    ui->salarySpinBox->setValue(1);
}

void MainWindow::createMenu() {

    QAction *clearFlowersAction = new QAction("Цветы");
    QAction *clearSellsAction = new QAction("Продажи");
    QAction *clearWorkersAction = new QAction("Работники");

    connect(clearFlowersAction, SIGNAL(triggered(bool)), this, SLOT(clearFlowers()));
    connect(clearSellsAction, SIGNAL(triggered(bool)), this, SLOT(clearSells()));
    connect(clearWorkersAction, SIGNAL(triggered(bool)), this, SLOT(clearWorkers()));

    QMenu* clearMenu = new QMenu("Очистить", this);

    clearMenu->addAction(clearFlowersAction);
    clearMenu->addAction(clearSellsAction);
    clearMenu->addAction(clearWorkersAction);


    QAction *actionExit = new QAction("Выход");
    QAction* openAction = new QAction("Открыть");

    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openDatabase()));
    connect(actionExit, SIGNAL(triggered(bool)), this, SLOT(close()));


    QMenu* dataBaseMenu = new QMenu("База данных", this);

    dataBaseMenu->addAction(openAction);
    dataBaseMenu->addMenu(clearMenu);
    dataBaseMenu->addAction(actionExit);

    menuBar()->addMenu(dataBaseMenu);


    QAction *depositAction = new QAction("Внести средства на баланс");
    QAction *getBalanceSalaryAction = new QAction("Информация о выплатах");
    QAction *paySalaryAction = new QAction("Выплатить заработную плату");

    connect(depositAction, SIGNAL(triggered(bool)), this, SLOT(deposit()));
    connect(getBalanceSalaryAction, SIGNAL(triggered(bool)), this, SLOT(showInfo()));
    connect(paySalaryAction, SIGNAL(triggered(bool)), this, SLOT(paySalary()));

    QMenu* operationsMenu = new QMenu("Операции", this);

    operationsMenu->addAction(depositAction);
    operationsMenu->addAction(getBalanceSalaryAction);
    operationsMenu->addAction(paySalaryAction);

    menuBar()->addMenu(operationsMenu);
}


void MainWindow::createMenuButtons() {

    QMenu* flowerMenu = new QMenu;

    QAction* addFlowerAction = new QAction("Добавить партию цветов");

    flowerMenu->addAction(addFlowerAction);
    connect(addFlowerAction, SIGNAL(triggered(bool)), this, SLOT(addFlowers()));

    QAction *sellFlowerAction = new QAction("Продать партию цветов");
    flowerMenu->addAction(sellFlowerAction);
    connect(sellFlowerAction, SIGNAL(triggered(bool)), this, SLOT(sellFlowers()));

    ui->flowerActionPushButton->setMenu(flowerMenu);

    QMenu* workerMenu = new QMenu;

    QAction* addWorkerAction = new QAction("Нанять работника");

    workerMenu->addAction(addWorkerAction);
    connect(addWorkerAction, SIGNAL(triggered(bool)), this, SLOT(addWorker()));

    QAction *fireWorkerAction = new QAction("Уволить работника");
    workerMenu->addAction(fireWorkerAction);
    connect(fireWorkerAction, SIGNAL(triggered(bool)), this, SLOT(fireWorker()));

    ui->workerActionPushButton->setMenu(workerMenu);
}

void MainWindow::createTreeWidget() {

    flowers = new QTreeWidgetItem(ui->treeWidget);
    flowers->setText(0, "Цветы");

    sells = new QTreeWidgetItem(flowers);
    sells->setText(0, "Продажи");

    in_storage = new QTreeWidgetItem(flowers);
    in_storage->setText(0, "На складе");

    workers = new QTreeWidgetItem(ui->treeWidget);
    workers->setText(0, "Работники");

    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(changeTable(QTreeWidgetItem*, int)));
}

void MainWindow::updateTitle() {

    QString title;

    switch (tableType) {
    case TableType::Flowers:
        title = "На складе";
        break;
    case TableType::SoldFlowers:
        title = "Продажи";
        break;
    case TableType::Workers:
        title = "Персонал";
        break;
    }

    setWindowTitle(title);
}

void MainWindow::changeTable(QTreeWidgetItem* item, int column) {

    if (column != 0)
        return;

    if (item == in_storage) {
        tableType = TableType::Flowers;
    } else if (item == sells) {
        tableType = TableType::SoldFlowers;
    } else if (item == workers) {
        tableType = TableType::Workers;
    }

    emit update();
}

void MainWindow::update() {

    updateTitle();

    ui->balanceLabel->setText(tr("Баланс магазина: ") + QString::number(balance));

    ui->tableWidget->setRowCount(0);
    QList<Flower> flowers;
    QList<Worker> workers;

    switch (tableType) {
    case TableType::Flowers:

        flowers = database->flowers();

        ui->tableWidget->setColumnCount(5);
        ui->tableWidget->setHorizontalHeaderLabels(QStringList() <<
                                                   "Название" << "Цвет" <<
                                                   "Цена продажи" << "Цена закупки" <<
                                                   "Количество");

        for (auto flower : flowers) {
            int index = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(index);

            ui->tableWidget->setItem(index, 0, new QTableWidgetItem(flower.name));
            ui->tableWidget->setItem(index, 1, new QTableWidgetItem(flower.color));
            ui->tableWidget->setItem(index, 2, new QTableWidgetItem(QString::number(flower.price)));
            ui->tableWidget->setItem(index, 3, new QTableWidgetItem(QString::number(flower.primeCost)));
            ui->tableWidget->setItem(index, 4, new QTableWidgetItem(QString::number(flower.count)));
        }

        break;
    case TableType::SoldFlowers:

        flowers = database->soldFlowers();

        ui->tableWidget->setColumnCount(6);
        ui->tableWidget->setHorizontalHeaderLabels(QStringList() <<
                                                   "Название" << "Цвет" <<
                                                   "Цена продажи" << "Цена закупки" <<
                                                   "Количество" << "Дата продажи");

        for (auto flower : flowers) {
            int index = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(index);

            ui->tableWidget->setItem(index, 0, new QTableWidgetItem(flower.name));
            ui->tableWidget->setItem(index, 1, new QTableWidgetItem(flower.color));
            ui->tableWidget->setItem(index, 2, new QTableWidgetItem(QString::number(flower.price)));
            ui->tableWidget->setItem(index, 3, new QTableWidgetItem(QString::number(flower.primeCost)));
            ui->tableWidget->setItem(index, 4, new QTableWidgetItem(QString::number(flower.count)));
            ui->tableWidget->setItem(index, 5, new QTableWidgetItem(flower.date));
        }

        break;
    case TableType::Workers:

        workers = database->workers();

        ui->tableWidget->setColumnCount(2);
        ui->tableWidget->setHorizontalHeaderLabels(QStringList() <<
                                                   "Имя" << "Размер заработной платы");

        for (auto& worker : workers) {
            int index = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(index);

            ui->tableWidget->setItem(index, 0, new QTableWidgetItem(worker.name));
            ui->tableWidget->setItem(index, 1, new QTableWidgetItem(QString::number(worker.salary)));
        }

        break;
    }

    ui->tableWidget->resizeColumnsToContents();
}

void MainWindow::deposit() {
    DepositDialog* dlg = new DepositDialog(this);

    switch( dlg->exec() ) {
    case QDialog::Accepted:
        balance += dlg->deposit();
        database->updateBalance(balance);
        update();
        break;
    case QDialog::Rejected:
        break;
    }
}

void MainWindow::showInfo() {

    QMessageBox messageBox(this);
    messageBox.resize(800, 400);

    int totalSalary = database->totalSalary();
    int count = balance / totalSalary;
    QString ending = "";

    if (count % 10 == 1 && count / 10 % 10 != 1)
        ending = "у";
    else if (2 <= count % 10 && count % 10 <= 4 && count / 10 % 10 != 1)
        ending = "ы";

    QString text = "Общий размер заработной платы работников:\n" +
            QString::number(database->totalSalary()) + " P\n" +
            "Текущих средств хватит на " + QString::number(count) + " выплат" + ending + ".";

    messageBox.setText(text);
    messageBox.setWindowTitle("Информация о выплатах");
    messageBox.exec();
}

void MainWindow::paySalary() {
    int totalSalary = database->totalSalary();

    if (balance < totalSalary) {
        QMessageBox::critical(this, "Неудача", "Недостаточно средств на балансе для выплаты!");
    } else {
        QMessageBox::about(this, "Успех", "Операция прошла успешно!");
        balance -= totalSalary;
        database->updateBalance(balance);
        update();
    }
}

void MainWindow::openDatabase() {

    QDir pathDirProject(QCoreApplication::applicationDirPath());

    pathDirProject.cdUp();
    pathDirProject.cdUp();

    QString filename = QFileDialog::getOpenFileName(
                this,
                "Выберите файл с базой данных",
                pathDirProject.path(),
                tr("Database Files (*.db *.sql)")
                );

    if (filename.isEmpty())
        return;

    delete database;
    database = new ShopDatabase(filename);
    database->createTables();

    update();
}


void MainWindow::clearFlowers() {
    database->clearTable("flowers");
}

void MainWindow::clearSells() {
    database->clearTable("sold_flowers");
}

void MainWindow::clearWorkers() {
    database->clearTable("workers");
}
