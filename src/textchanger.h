#pragma once

#include <QPushButton>
#include <QTimer>
#include <QObject>

template<typename T>
class TextChanger
{
public:
    TextChanger() {
        timer_.setSingleShot(true);
        auto lambda = [this]() {
            OnTimeout();
        };
        QObject::connect(&timer_, &QTimer::timeout, lambda);
    }

    void operator()(T* new_widget, int ms, QString new_text) {
        if (widget_ == nullptr) {
            widget_ = new_widget;
            default_text_ = widget_->text();
            widget_->setText(new_text);
            timer_.setInterval(ms);
            timer_.start();
        }
        else if (widget_ == new_widget) {
            widget_->setText(new_text);
            timer_.setInterval(ms);
        }
        else {
            widget_->setText(default_text_);
            widget_ = new_widget;
            default_text_ = widget_->text();
            widget_->setText(new_text);
            timer_.setInterval(ms);
        }
    }

private:
    void OnTimeout() {
        if (widget_ == nullptr) {
            return;
        }

        widget_->setText(default_text_);
        widget_ = nullptr;
    }

    T* widget_{ nullptr };
    QTimer timer_;
    QString default_text_;
};
