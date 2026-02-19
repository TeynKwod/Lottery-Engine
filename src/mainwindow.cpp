#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QRegularExpression numbers("^\\d+$");
    ui->le_number_winners->setValidator(new QRegularExpressionValidator(numbers, this));

    ui->lst_participants->hide();

    randomizer_ptr_ = model_.GetRandomizer();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::ApplyModel() {
    bool participants_filled = model_.NumberOfParticipants(false) > 0;
    ui->btn_gen_winners->setEnabled(participants_filled);
    ui->btn_search->setEnabled(participants_filled);
    ui->btn_to_clipboard->setEnabled(participants_filled);
    ui->btn_write_file->setEnabled(participants_filled);

    bool active_part_filled = model_.NumberOfParticipants(true) > 0;
    ui->rbtn_active_part->setEnabled(active_part_filled);
    ui->rbtn_active_work->setEnabled(active_part_filled);
    ui->btn_next_active->setEnabled(active_part_filled & !show_active_part_);
    ui->btn_prev_active->setEnabled(active_part_filled & !show_active_part_);

    if (table_view_) {
        ui->tbl_participants->clearContents();
        ui->tbl_participants->setRowCount(model_.NumberOfParticipants(show_active_part_));

        for (size_t i = 0; i < model_.NumberOfParticipants(show_active_part_); ++i) {
            Participant part{model_.GetParticipant(i, show_active_part_)};
            for (size_t j = 0; j < 5; ++j) {
                ui->tbl_participants->setItem(i, j, new QTableWidgetItem(part.GetElement(j)));
            }
        }

        if (model_.GetIterator() != std::nullopt) {
            ui->tbl_participants->selectRow(std::distance(model_.GetBeginIterator(), model_.GetIterator().value()));
        }
    }
    else {
        ui->lst_participants->clear();

        for (size_t i = 0; i < model_.NumberOfParticipants(show_active_part_); ++i) {
            Participant part{model_.GetParticipant(i, show_active_part_)};
            QString part_str(QString("%1 - ID: %2 Name: %3 Surname: %4 Username: %5 Time: %6").
                arg(QString::number(i + 1),
                part.GetElement(0),
                part.GetElement(1),
                part.GetElement(2),
                part.GetElement(3),
                part.GetElement(4)));
            ui->lst_participants->addItem(part_str);
        }

        if (model_.GetIterator() != std::nullopt) {
            ui->lst_participants->setCurrentRow(std::distance(model_.GetBeginIterator(), model_.GetIterator().value()));
        }
    }
}

QString MainWindow::ItemsToString(QList<QTableWidgetItem*> items) {
    QString result;
    for (QTableWidgetItem* item : items) {
        result += model_.ValueNameByIndex(item->column()) + item->text() + " ";
    }
    return result;
}

#define ActionsSlots_start {

void MainWindow::on_act_show_lst_triggered()
{
    table_view_ = false;
    ui->lst_participants->show();
    ui->tbl_participants->hide();
    ApplyModel();
}


void MainWindow::on_act_show_tbl_triggered()
{
    table_view_ = true;
    ui->lst_participants->hide();
    ui->tbl_participants->show();
    ApplyModel();
}

#define ActionsSlots_end }

#define ParticipantShowSlots_start {

void MainWindow::on_rbtn_active_part_clicked()
{
    show_active_part_ = true;
    ApplyModel();
}


void MainWindow::on_rbtn_all_part_clicked()
{
    show_active_part_ = false;
    ApplyModel();
}

#define ParticipantShowSlots_end }

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
    model_.FindParticipants(ui->le_search->text(), search_type_);
    ApplyModel();
}


void MainWindow::on_btn_copy_clicked()
{
    QClipboard* clip = QApplication::clipboard();
    if (!clip) {
        qWarning() << "Error in on_btn_copy_clicked: can't access to clipboard";
        return;
    }

    if (table_view_) {
        clip->setText(ItemsToString(ui->tbl_participants->selectedItems()));
    }
    else if (ui->lst_participants->currentRow() >= 0) {
        clip->setText(ui->lst_participants->currentItem()->text());
    }
}

#define SearchSlots_end }

#define WinnersSlots_start {

void MainWindow::on_btn_gen_winners_clicked()
{
    size_t number_of_winners = ui->le_number_winners->text().toULongLong();
    if (number_of_winners == 0 || number_of_winners > model_.NumberOfParticipants(false)) {
        qWarning() << "Error in on_btn_gen_winners_clicked: invalid number of winners";
        return;
    }
    model_.GenerateWinners(number_of_winners);
    ApplyModel();
}


void MainWindow::on_btn_next_active_clicked()
{
    model_.SetNextIterator();
    ApplyModel();
}


void MainWindow::on_btn_prev_active_clicked()
{
    model_.SetPrevIterator();
    ApplyModel();
}


void MainWindow::on_btn_random_prng_clicked()
{
    randomizer_ptr_->SetTimeBasedSeed();
}


void MainWindow::on_btn_random_rng_clicked()
{
    randomizer_ptr_->SetHardwareBasedSeed();
}

#define WinnersSlots_end }

#define ParticipantControlSlots_start {

void MainWindow::on_btn_read_file_clicked()
{
    QString file_name = QFileDialog::getOpenFileName();
    if (file_name.isEmpty()) {
        qInfo() << "on_btn_read_file_clicked: no file name";
        return;
    }

    model_.ReadFromFile(file_name);
    ApplyModel();
}


void MainWindow::on_btn_write_file_clicked()
{
    QString directory = QFileDialog::getExistingDirectory();
    if (directory.isEmpty()) {
        qInfo() << "on_btn_write_file_clicked: no directory";
        return;
    }

    directory += "/Participants.txt";
    QFile file(directory);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning() << "Error in on_btn_write_file_clicked: can't open file";
    }

    QTextStream output(&file);
    if (ui->rbtn_all_work->isChecked()) {
        output << model_.GetParticipantsString();
    }
    else {
        output << model_.GetActiveParticipantsString();
    }

    file.close();
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
        qWarning() << "Error in on_btn_to_clipboard_clicked: can't access to clipboard";
        return;
    }

    if (ui->rbtn_all_work->isChecked()) {
        clip->setText(model_.GetParticipantsString());
    }
    else {
        clip->setText(model_.GetActiveParticipantsString());
    }
}


void MainWindow::on_btn_gen_participants_clicked()
{
    model_.GenerateTestParticipants(100000);
    ApplyModel();
}

#define ParticipantControlSlots_end }
