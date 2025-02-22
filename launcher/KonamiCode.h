#pragma once

#include <QKeyEvent>

class KonamiCode : public QObject
{
    Q_OBJECT
public:
    explicit KonamiCode(QObject *parent = nullptr);
    void input(QEvent *event);

signals:
    void triggered();

private:
    int m_progress = 0;
};
