#include "helpwindow.h"
#include "ui_helpwindow.h"

HelpWindow::HelpWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HelpWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Инструкция");
}

HelpWindow::~HelpWindow()
{
    delete ui;
}
