#pragma once
#include "AuthFlow.h"

class MojangRefresh : public AuthFlow
{
    Q_OBJECT
public:
    explicit MojangRefresh(
        AccountData *data,
        QObject *parent = nullptr
    );
};

class MojangLogin : public AuthFlow
{
    Q_OBJECT
public:
    explicit MojangLogin(
        AccountData *data,
        const QString &password,
        QObject *parent = nullptr
    );

private:
    QString m_password;
};
