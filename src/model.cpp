#include <vector>
#include <fstream>

#include "model.h"

Participant::Participant(QString str_part) {
    QStringList data = str_part.split(",");
    if (data.length() != 5) {
        qWarning() << "Error in Participant(QString): invalid string";
        return;
    }
    id = data[0];
    name = data[1];
    surname = data[2];
    nick = data[3];

    std::tm tm{};
    std::istringstream stream(data[4].toStdString());
    stream >> std::get_time(&tm, "%d.%m.%Y %H:%M");
    time = std::chrono::system_clock::from_time_t(mktime(&tm));
}

QString Participant::GetElement(size_t index) const {
    switch (index) {
    case 0:
        return id;
        break;
    case 1:
        return name;
        break;
    case 2:
        return surname;
        break;
    case 3:
        return nick;
        break;
    case 4:
    {
        const std::chrono::time_zone* local_zone = std::chrono::locate_zone("Europe/Moscow");
        std::chrono::zoned_time show_time(local_zone, time);
        return QString::fromStdString(std::format("{:%d.%m.%Y %H:%M}", show_time));
        break;
    }
    default:
        qWarning() << "Error in GetElement: unknown index";
        return "Unknown index";
        break;
    }
}

QString Model::ValueNameByIndex(int index) const {
    switch (index) {
    case 0:
        return "ID: ";
    case 1:
        return "Имя: ";
    case 2:
        return "Фамилия: ";
    case 3:
        return "Username: ";
    case 4:
        return "Время: ";
    default:
        qWarning() << "Error in ValueNameByIndex: invalid column";
        return "";
        break;
    }
}

// True for active participants, false for all
size_t Model::NumberOfParticipants(bool need_active_part) const {
    if (need_active_part) {
        return active_part_.size();
    }
    return participants_.size();
}

// True for active participants, false for all
Participant Model::GetParticipant(size_t index, bool need_active_part) const {
    if (need_active_part) {
        return *active_part_[index];
    }
    return participants_[index];
}

QString Model::AddActiveParticipant() {
    if (!iterator_.has_value()) {
        return "Нет выделенного участника";
    }

    for (Participant* part : active_part_) {
        if (part == iterator_.value().base()) {
            return "Участник уже в списке";
        }
    }

    active_part_.push_back(iterator_.value().base());
    return "Участник добавлен";
}

QString Model::DeleteActiveParticipant() {
    if (!iterator_.has_value()) {
        return "Нет выделенного участника";
    }

    for (size_t i = 0; i < active_part_.size(); ++i) {
        if (active_part_[i] == iterator_.value().base()) {
            active_part_.erase(active_part_.begin() + i);
            if (current_active_index_ == active_part_.size()) {
                --current_active_index_;
            }
            return "Участник удалён";
        }
    }

    return "Участник не выбран";
}

void Model::GenerateTestParticipants(int number_of_participants) {
    static const QString name_charset = "abcdefghijklmnopqrstuvwxyz", nick_charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

    iterator_ = std::nullopt;
    participants_.clear();
    active_part_.clear();
    participants_.reserve(number_of_participants);

    for (int i = 0; i < number_of_participants; ++i) {
        Participant part;

        part.id = QString::number(i);
        part.time = std::chrono::system_clock::now();

        randomizer_.ChangeLimits(4, 10);
        int name_long = randomizer_.GetRandomNumber();
        int surname_long = randomizer_.GetRandomNumber();
        int nick_long = randomizer_.GetRandomNumber();
        part.name.reserve(name_long);
        part.surname.reserve(surname_long);
        part.nick.reserve(nick_long);

        randomizer_.ChangeLimits(0, name_charset.length() - 1);
        for (int j = 0; j < name_long; ++j) {

            part.name.push_back(name_charset[randomizer_.GetRandomNumber()]);
        }

        for (int j = 0; j < surname_long; ++j) {
            part.surname.push_back(name_charset[randomizer_.GetRandomNumber()]);
        }

        randomizer_.ChangeLimits(0, nick_charset.length() - 1);
        for (int j = 0; j < nick_long; ++j) {
            part.nick.push_back(nick_charset[randomizer_.GetRandomNumber()]);
        }

        AddParticipant(part);
    }
}

void Model::ReadFromFile(const QString& file_name){
    iterator_ = std::nullopt;
    participants_.clear();
    active_part_.clear();
    std::ifstream input{ file_name.toStdString() };
    std::string readed_line;

    if (!input.is_open()) {
        qWarning() << "Error in ReadFromFile: can't open file";
        return;
    }

    std::getline(input, readed_line);
    while (std::getline(input, readed_line)) {
        AddParticipant(Participant(QString::fromStdString(readed_line)));
    }

    input.close();
}

std::vector<Participant*> Model::CopyParticipantPtr() {
    std::vector<Participant*> result;
    result.reserve(participants_.size());

    for (Participant& part : participants_) {
        result.push_back(&part);
    }

    return result;
}

void Model::GenerateWinners(size_t number_of_winners) {
    iterator_ = std::nullopt;
    active_part_.clear();
    active_part_.reserve(number_of_winners);

    std::vector<Participant*> temp_parts = CopyParticipantPtr();
    for (size_t i = 0; i < number_of_winners; ++i) {
        randomizer_.ChangeLimits(0, temp_parts.size() - 1);
        size_t index = randomizer_.GetRandomNumber();
        active_part_.push_back(temp_parts[index]);
        auto it = temp_parts.begin() + index;
        temp_parts.erase(it);
    }
}

QString Model::FindParticipants(const QString& key, SearchType type) {
    switch (type) {
    case SearchType::NUMBER:
        if (size_t index = key.toULongLong(); index < 1 || index > participants_.size()) {
            return "Неверный индекс";
        }
        else {
            iterator_ = participants_.begin() + (index - 1);
            return "Участник найден";
        }
        break;
    case SearchType::ID:
    {
        auto comparator = [key](const Participant& part) -> bool {
            return part.id == key;
        };
        if (auto iter = std::find_if(participants_.begin(), participants_.end(), comparator); iter == participants_.end()) {
            return "Участник не найден";
        }
        else {
            iterator_ = iter;
            return "Участник найден";
        }
        break;
    }
    case SearchType::NAME:
        iterator_ = std::nullopt;
        active_part_.clear();

        for (Participant& elem : participants_) {
            if (elem.name.contains(key) || elem.surname.contains(key)) {
                active_part_.push_back(&elem);
            }
        }

        if (active_part_.empty()) {
            return "Участники не найдены";
        }
        else {
            iterator_ = participants_.begin() + std::distance(participants_.data(), active_part_[0]);
            current_active_index_ = 0;
            return "Участники найдены";
        }
        break;
    case SearchType::USERNAME:
        iterator_ = std::nullopt;
        active_part_.clear();

        for (Participant& elem : participants_) {
            if (elem.nick == key) {
                active_part_.push_back(&elem);
            }
        }

        if (active_part_.empty()) {
            return "Участники не найдены";
        }
        else {
            iterator_ = participants_.begin() + std::distance(participants_.data(), active_part_[0]);
            current_active_index_ = 0;
            return "Участники найдены";
        }
        break;
    default:
        qWarning() << "Error in FindParticipants: unknown SearchType";
        return "Ошибка";
        break;
    }
}

void Model::SetNextIterator() {
    if (!iterator_.has_value() || current_active_index_ >= active_part_.size() - 1) {
        current_active_index_ = -1;
    }

    iterator_ = participants_.begin() + std::distance(participants_.data(), active_part_[++current_active_index_]);
}

void Model::SetPrevIterator() {
    if (!iterator_.has_value() || current_active_index_ == 0) {
        current_active_index_ = active_part_.size();
    }

    iterator_ = participants_.begin() + std::distance(participants_.data(), active_part_[--current_active_index_]);
}

void Model::SetIterator(size_t index, bool need_active_part) {
    if (need_active_part) {
        current_active_index_ = index;

        auto comparator = [&](const Participant& part) -> bool {
            return &part == active_part_[current_active_index_];
        };

        iterator_ = std::find_if(participants_.begin(), participants_.end(), comparator);
        return;
    }

    iterator_ = participants_.begin() + index;
    for (size_t i = 0; i < active_part_.size(); ++i) {
        if (active_part_[i] == iterator_.value().base()) {
            current_active_index_ = i;
        }
    }
}

QString Model::GetParticipantsString() {
    QString result;

    for (size_t j = 0; j < participants_.size(); ++j) {
        result += QString::number(j + 1) + " - ";

        for (int i = 0; i < 5; ++i) {
            result += ValueNameByIndex(i) + participants_[j].GetElement(i) + " ";
        }

        result.chop(1);
        result += "\n";
    }

    return result;
}

QString Model::GetActiveParticipantsString() {
    QString result;

    for (size_t j = 0; j < active_part_.size(); ++j) {
        result += QString::number(j + 1) + " - ";

        for (int i = 0; i < 5; ++i) {
            result += ValueNameByIndex(i) + active_part_[j]->GetElement(i) + " ";
        }

        result.chop(1);
        result += "\n";
    }

    return result;
}
