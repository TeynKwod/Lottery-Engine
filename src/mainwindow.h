#pragma once

#include <QMainWindow>
#include <QTimer>

#include "model.h"
#include "qpushbutton.h"
#include "qtablewidget.h"
#include "enums.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_act_show_lst_triggered();
    void on_act_show_tbl_triggered();
    void on_act_imp_clipboard_triggered();
    void on_act_imp_file_triggered();
    void on_act_exp_clipboard_triggered();
    void on_act_exp_file_triggered();

    void on_rbtn_number_clicked();
    void on_rbtn_id_clicked();
    void on_rbtn_name_clicked();
    void on_rbtn_nick_clicked();
    void on_btn_search_clicked();
    void on_btn_copy_clicked();

    void on_btn_gen_winners_clicked();
    void on_btn_next_active_clicked();
    void on_btn_prev_active_clicked();
    void on_btn_add_part_clicked();
    void on_btn_random_prng_clicked();
    void on_btn_random_rng_clicked();

    void on_btn_read_file_clicked();
    void on_btn_write_file_clicked();
    void on_btn_from_clipboard_clicked();
    void on_btn_to_clipboard_clicked();
    void on_btn_gen_participants_clicked();

    void on_lst_participants_currentRowChanged(int currentRow);
    void on_tbl_participants_itemSelectionChanged();

private:
    void ApplyButtons();
    void ApplyModel();
    void ApplyIterator();
    QString ItemsToString(QList<QTableWidgetItem*>);

    void TextChanger(QPushButton* button, const QString& temp_text, int ms) {
        QString std_text{ button->text() };
        button->setText(temp_text);
        QTimer::singleShot(ms, this, [=]() {
            button->setText(std_text);
        });
    };

    Ui::MainWindow *ui;
    std::unique_ptr<CustomRandom> randomizer_ptr_;
    Model model_;
    SearchType search_type_{ SearchType::NUMBER };
    bool applying_model_{ false };
};
