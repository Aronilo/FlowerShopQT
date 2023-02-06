#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "shop_database.h"
#include "deposit_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class TableType {
    Flowers,
    SoldFlowers,
    Workers
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void createMenu();
    void createMenuButtons();

    void createTreeWidget();

    void updateTitle();

    QTreeWidgetItem  *flowers,
                    *sells,
                    *in_storage,
                    *workers;

    ShopDatabase* database;
    TableType tableType;
    int balance;

private slots:
    void addFlowers();
    void sellFlowers();

    void addWorker();
    void fireWorker();

    void changeTable(QTreeWidgetItem*, int);
    void update();

    void deposit();
    void showInfo();
    void paySalary();

    void openDatabase();
    void clearFlowers();
    void clearSells();
    void clearWorkers();
};
#endif // MAINWINDOW_H
