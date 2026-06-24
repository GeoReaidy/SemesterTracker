#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->loginPasswordLineEdit->setEchoMode(
        QLineEdit::Password
    );

    ui->registerPasswordLineEdit->setEchoMode(
        QLineEdit::Password
    );

    ui->registerConfirmPasswordLineEdit->setEchoMode(
        QLineEdit::Password
    );

    ui->registerMaxCreditsSpinBox->setRange(
        1,
        1000
    );

    ui->registerMaxCreditsSpinBox->setValue(
        120
    );

    ui->authStackedWidget->setCurrentWidget(
        ui->welcomePage
    );

    ui->authMessageLabel->clear();
    ui->authMessageLabel->hide();

    ui->getStartedButton->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}
