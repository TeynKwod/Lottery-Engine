#pragma once

#include <QDialog>

namespace Ui {
class HelpWindow;
}

class HelpWindow : public QDialog
{
    Q_OBJECT

public:
    explicit HelpWindow(QWidget *parent = nullptr);
    ~HelpWindow();

private slots:
    void on_btn_close_clicked() {
        this->close();
    }

private:
    Ui::HelpWindow *ui;
};
