#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uimanager.h"
#include "questionwidget.h"
#include "skillpanel.h"
#include "statusbar.h"
#include "GameData.h"
#include <QDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(960, 720);
    if (centralWidget()) {
        centralWidget()->setContentsMargins(0, 0, 0, 0);
        centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);
    }
    ui->container->setContentsMargins(0, 0, 0, 0);
    UIManager::instance()->setContainer(ui->container);
}

MainWindow::~MainWindow()
{
    delete ui;
}
