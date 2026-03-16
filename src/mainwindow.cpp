#include <QFileDialog>
#include <QClipboard>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QRegularExpression numbers("^\\d+$");
    ui->le_number_winners->setValidator(new QRegularExpressionValidator(numbers, this));

    ui->lst_participants->hide();

    randomizer_ptr_ = model_.GetRandomizer();

    connect(ui->rbtn_active_part, &QAbstractButton::clicked, this, &MainWindow::ApplyModel);
    connect(ui->rbtn_all_part, &QAbstractButton::clicked, this, &MainWindow::ApplyModel);
    connect(ui->le_number_winners, &QLineEdit::textEdited, this, &MainWindow::ApplyButtons);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::ApplyButtons() {
    bool participants_filled = model_.NumberOfParticipants(false) > 0;
    ui->btn_gen_winners->setEnabled(participants_filled && ui->le_number_winners->hasAcceptableInput());
    ui->btn_search->setEnabled(participants_filled);
    ui->btn_to_clipboard->setEnabled(participants_filled);
    ui->btn_write_file->setEnabled(participants_filled);
    ui->act_exp_clipboard->setEnabled(participants_filled);
    ui->act_exp_file->setEnabled(participants_filled);
    ui->btn_add_part->setEnabled(participants_filled && model_.GetIterator().has_value());

    bool active_part_filled = model_.NumberOfParticipants(true) > 0;
    ui->rbtn_active_part->setEnabled(active_part_filled);
    ui->rbtn_active_work->setEnabled(active_part_filled);
    ui->btn_next_active->setEnabled(active_part_filled && !ui->rbtn_active_part->isChecked());
    ui->btn_prev_active->setEnabled(active_part_filled && !ui->rbtn_active_part->isChecked());
}

void MainWindow::ApplyModel() {
    ui->lbl_active_number->setText(QString::number(model_.NumberOfParticipants(true)));

    applying_model_ = true;
    if (ui->act_show_tbl->isChecked()) {
        ui->tbl_participants->clearContents();
        ui->tbl_participants->setRowCount(model_.NumberOfParticipants(ui->rbtn_active_part->isChecked()));

        for (size_t i = 0; i < model_.NumberOfParticipants(ui->rbtn_active_part->isChecked()); ++i) {
            Participant part{model_.GetParticipant(i, ui->rbtn_active_part->isChecked())};
            for (size_t j = 0; j < 5; ++j) {
                ui->tbl_participants->setItem(i, j, new QTableWidgetItem(part.GetElement(j)));
            }
        }
    }
    else {
        ui->lst_participants->clear();

        for (size_t i = 0; i < model_.NumberOfParticipants(ui->rbtn_active_part->isChecked()); ++i) {
            Participant part{model_.GetParticipant(i, ui->rbtn_active_part->isChecked())};
            QString part_str(QString("%1 - ID: %2 Name: %3 Surname: %4 Username: %5 Time: %6").
                arg(QString::number(i + 1),
                part.GetElement(0),
                part.GetElement(1),
                part.GetElement(2),
                part.GetElement(3),
                part.GetElement(4)));
            ui->lst_participants->addItem(part_str);
        }
    }
    applying_model_ = false;

    ApplyButtons();
}

void MainWindow::ApplyIterator() {
    if (!model_.GetIterator().has_value()) {
        return;
    }

    if (ui->act_show_tbl->isChecked()) {
        ui->tbl_participants->selectRow(std::distance(model_.GetBeginIterator(), model_.GetIterator().value()));
    }
    else {
        ui->lst_participants->setCurrentRow(std::distance(model_.GetBeginIterator(), model_.GetIterator().value()));
    }
}

QString MainWindow::ItemsToString(QList<QTableWidgetItem*> items) {
    if (items.size() == 1) {
        return items[0]->text();
    }

    QString result;
    for (const QTableWidgetItem* item : std::as_const(items)) {
        result += model_.ValueNameByIndex(item->column()) + item->text() + " ";
    }
    result.chop(1);
    return result;
}

#define ActionsSlots_start {

void MainWindow::on_act_show_lst_triggered()
{
    ui->act_show_tbl->setChecked(false);
    ui->lst_participants->show();
    ui->tbl_participants->hide();
    ApplyModel();
}


void MainWindow::on_act_show_tbl_triggered()
{
    ui->act_show_lst->setChecked(false);
    ui->lst_participants->hide();
    ui->tbl_participants->show();
    ApplyModel();
}


void MainWindow::on_act_imp_clipboard_triggered()
{
    on_btn_from_clipboard_clicked();
}


void MainWindow::on_act_imp_file_triggered()
{
    on_btn_read_file_clicked();
}


void MainWindow::on_act_exp_clipboard_triggered()
{
    on_btn_to_clipboard_clicked();
}


void MainWindow::on_act_exp_file_triggered()
{
    on_btn_write_file_clicked();
}

#define ActionsSlots_end }

#define SearchSlots_start {

void MainWindow::on_rbtn_number_clicked()
{
    search_type_ = SearchType::NUMBER;
}


void MainWindow::on_rbtn_id_clicked()
{
    search_type_ = SearchType::ID;
}


void MainWindow::on_rbtn_name_clicked()
{
    search_type_ = SearchType::NAME;
}


void MainWindow::on_rbtn_nick_clicked()
{
    search_type_ = SearchType::USERNAME;
}


void MainWindow::on_btn_search_clicked()
{
    TextChanger(ui->btn_search, 1000, model_.FindParticipants(ui->le_search->text(), search_type_), "Поиск");
    ApplyModel();
    ApplyIterator();
}


void MainWindow::on_btn_copy_clicked()
{
    QClipboard* clip = QApplication::clipboard();
    if (!clip) {
        qWarning() << "Error in on_btn_copy_clicked: can't access to clipboard";
        TextChanger(ui->btn_copy, 1000, "Ошибка", "Скопировать");
        return;
    }

    if (ui->act_show_tbl->isChecked()) {
        clip->setText(ItemsToString(ui->tbl_participants->selectedItems()));
    }
    else if (ui->lst_participants->currentRow() >= 0) {
        clip->setText(ui->lst_participants->currentItem()->text());
    }

    TextChanger(ui->btn_copy, 1000, "Скопированно", "Скопировать");
}

#define SearchSlots_end }

#define WinnersSlots_start {

void MainWindow::on_btn_gen_winners_clicked()
{
    size_t number_of_winners = ui->le_number_winners->text().toULongLong();
    if (number_of_winners == 0 || number_of_winners > model_.NumberOfParticipants(false)) {
        TextChanger(ui->btn_gen_winners, 1000, "Неверное кол-во участников", "Выбрать победителей");
        return;
    }
    model_.GenerateWinners(number_of_winners);
    TextChanger(ui->lbl_info, 5000, "Выбрано " + QString::number(number_of_winners) + " новых участников");
    ApplyModel();
}


void MainWindow::on_btn_next_active_clicked()
{
    model_.SetNextIterator();
    ApplyIterator();
}


void MainWindow::on_btn_prev_active_clicked()
{
    model_.SetPrevIterator();
    ApplyIterator();
}


void MainWindow::on_btn_add_part_clicked()
{
    TextChanger(ui->lbl_info, 5000, model_.AddActiveParticipant());
    ApplyModel();
}


void MainWindow::on_btn_random_prng_clicked()
{
    randomizer_ptr_->SetTimeBasedSeed();
    TextChanger(ui->lbl_info, 5000, "Генератор пересобран на основе текущего времени");
}


void MainWindow::on_btn_random_rng_clicked()
{
    randomizer_ptr_->SetHardwareBasedSeed();
    TextChanger(ui->lbl_info, 5000, "Генератор пересобран на основе аппаратного генератора");
}

#define WinnersSlots_end }

#define ParticipantControlSlots_start {

void MainWindow::on_btn_read_file_clicked()
{
    QString file_name = QFileDialog::getOpenFileName();
    if (file_name.isEmpty()) {
        TextChanger(ui->lbl_info, 5000, "Файл не выбран"); // Error info
        return;
    }

    model_.ReadFromFile(file_name);
    ApplyModel();
}


void MainWindow::on_btn_write_file_clicked()
{
    QString directory = QFileDialog::getExistingDirectory();
    if (directory.isEmpty()) {
        TextChanger(ui->lbl_info, 5000, "Директория не выбрана"); // Error info
        return;
    }

    directory += "/Participants.txt";
    QFile file(directory);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        TextChanger(ui->lbl_info, 5000, "Не получилось создать и открыть файл для записи"); // Error info
        return;
    }

    QTextStream output(&file);
    if (ui->rbtn_all_work->isChecked()) {
        output << model_.GetParticipantsString();
    }
    else {
        output << model_.GetActiveParticipantsString();
    }

    file.close();

    TextChanger(ui->lbl_info, 5000, "Создан файл Participants.txt в выбранной директории");
}


void MainWindow::on_btn_from_clipboard_clicked()
{
    QClipboard* clip = QApplication::clipboard();
    if (!clip) {
        qWarning() << "Error in on_btn_from_clipboard_clicked: can't access to clipboard";
        return;
    }
    Participant part{clip->text()};
    model_.AddParticipant(part);
    ApplyModel();
}


void MainWindow::on_btn_to_clipboard_clicked()
{
    QClipboard* clip = QApplication::clipboard();
    if (!clip) {
        TextChanger(ui->btn_to_clipboard, 1000, "Нет доступа к буферу обмена", "Экспорт в буфер обмена"); // Error info
        return;
    }

    if (ui->rbtn_all_work->isChecked()) {
        clip->setText(model_.GetParticipantsString());
    }
    else {
        clip->setText(model_.GetActiveParticipantsString());
    }

    TextChanger(ui->lbl_info, 5000, "Участники скопированны в буфер обмена");
}


void MainWindow::on_btn_gen_participants_clicked()
{
    model_.GenerateTestParticipants(1000);
    ApplyModel();
}

#define ParticipantControlSlots_end }

void MainWindow::on_lst_participants_currentRowChanged(int currentRow)
{
    if (applying_model_ || ui->rbtn_active_part->isChecked()) {
        return;
    }

    model_.SetIterator(currentRow);
    ApplyButtons();
}


void MainWindow::on_tbl_participants_itemSelectionChanged()
{
    if (applying_model_ || ui->rbtn_active_part->isChecked()) {
        return;
    }

    model_.SetIterator(ui->tbl_participants->currentRow());
    ApplyButtons();
}
