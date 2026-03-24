#include "creatorwindow.h"
#include "ui_creatorwindow.h"

CreatorWindow::CreatorWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreatorWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Контакты");
}

CreatorWindow::~CreatorWindow()
{
    delete ui;
}
