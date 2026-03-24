#pragma once

#include <chrono>
#include <optional>

#include "customrandom.h"
#include "enums.h"

struct Participant {
    Participant() = default;
    Participant(QString str_part);

    QString GetElement(size_t index) const;

    QString id, name, surname, nick;
    std::chrono::system_clock::time_point time;
};

class Model {
public:
    Model() = default;

    QString ValueNameByIndex(int) const;

    size_t NumberOfParticipants(bool) const;
    Participant GetParticipant(size_t, bool) const;

    void AddParticipant(Participant part) {
        participants_.push_back(part);
    }

    QString AddActiveParticipant();
    QString DeleteActiveParticipant();

    std::unique_ptr<CustomRandom> GetRandomizer() {
        return std::unique_ptr<CustomRandom>(&randomizer_);
    }

    void GenerateTestParticipants(int);
    void ReadFromFile(const QString&);
    std::vector<Participant*> CopyParticipantPtr();
    void GenerateWinners(size_t);
    QString FindParticipants(const QString&, SearchType);

    void SetNextIterator();
    void SetPrevIterator();
    void SetIterator(size_t, bool);
    std::optional<std::vector<Participant>::iterator> GetIterator() {
        return iterator_;
    }
    std::vector<Participant>::iterator GetBeginIterator() {
        return participants_.begin();
    }
    size_t GetCurrentActiveIndex() {
        return current_active_index_;
    }

    QString GetParticipantsString();
    QString GetActiveParticipantsString();

private:
    CustomRandom randomizer_;
    std::optional<std::vector<Participant>::iterator> iterator_;
    std::vector<Participant> participants_;
    std::vector<Participant*> active_part_;
    size_t current_active_index_{ 0 };
};
