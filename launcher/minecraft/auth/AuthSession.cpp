#include "AuthSession.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStringList>

QString AuthSession::serializeUserProperties()
{
    QJsonObject userAttrs;
    /*
    for (auto key : u.properties.keys())
    {
        auto array = QJsonArray::fromStringList(u.properties.values(key));
        userAttrs.insert(key, array);
    }
    */
    QJsonDocument value(userAttrs);
    return value.toJson(QJsonDocument::Compact);

}

bool AuthSession::makeOffline(const QString& offline_playername)
{
    if (status != PlayableOffline && status != PlayableOnline)
    {
        return false;
    }
    session = "-";
    player_name = offline_playername;
    status = PlayableOffline;
    return true;
}

void AuthSession::makeDemo() {
    player_name = "Player";
    demo = true;
}
