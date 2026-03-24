#pragma once

#include <QDialog>

namespace Ui {
class CreatorWindow;
}

class CreatorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CreatorWindow(QWidget *parent = nullptr);
    ~CreatorWindow();

private slots:
    void on_btn_close_clicked() {
        this->close();
    }

private:
    Ui::CreatorWindow *ui;
};
