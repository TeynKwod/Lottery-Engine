#pragma once

#include <random>
#include <chrono>
#include <QDebug>

class CustomRandom
{
public:
    CustomRandom() {
        generator_ = std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());
    }

    int GetRandomNumber () {
        if (!limits_set_) {
            qWarning() << "Error in GetRandomNumber: limit isn't set";
            return 0;
        }

        return distribution_(generator_);
    }

    void ChangeLimits(int left_limit, int right_limit) {
        if (!limits_set_) {
            limits_set_ = true;
        }

        distribution_.param(std::uniform_int_distribution<int>::param_type (left_limit, right_limit));
    }

    void SetTimeBasedSeed() {
        generator_ = std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());
    }

    void SetHardwareBasedSeed() {
        generator_ = std::mt19937(std::random_device()());
    }

private:
    bool limits_set_{ false };
    std::mt19937 generator_;
    std::uniform_int_distribution<int> distribution_;
};
