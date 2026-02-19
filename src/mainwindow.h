#pragma once

#include <QMainWindow>

#include "model.h"
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

    void on_rbtn_active_part_clicked();
    void on_rbtn_all_part_clicked();

    void on_rbtn_number_clicked();
    void on_rbtn_id_clicked();
    void on_rbtn_name_clicked();
    void on_rbtn_nick_clicked();
    void on_btn_search_clicked();
    void on_btn_copy_clicked();

    void on_btn_gen_winners_clicked();
    void on_btn_next_active_clicked();
    void on_btn_prev_active_clicked();
    void on_btn_random_prng_clicked();
    void on_btn_random_rng_clicked();

    void on_btn_read_file_clicked();
    void on_btn_write_file_clicked();
    void on_btn_from_clipboard_clicked();
    void on_btn_to_clipboard_clicked();
    void on_btn_gen_participants_clicked();

private:
    void ApplyModel();
    QString ItemsToString(QList<QTableWidgetItem*>);

    Ui::MainWindow *ui;
    std::unique_ptr<CustomRandom> randomizer_ptr_;
    Model model_;
    SearchType search_type_{ SearchType::NUMBER };
    bool table_view_{ true };
    bool show_active_part_{ false };
};
