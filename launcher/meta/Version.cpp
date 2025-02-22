/* Copyright 2015-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Version.h"

#include <QDateTime>

#include "JsonFormat.h"
#include "minecraft/PackProfile.h"

Meta::Version::Version(const QString &uid, const QString &version)
    : BaseVersion(), m_uid(uid), m_version(version)
{
}

Meta::Version::~Version()
{
}

QString Meta::Version::descriptor()
{
    return m_version;
}
QString Meta::Version::name()
{
    if(m_data)
        return m_data->name;
    return m_uid;
}
QString Meta::Version::typeString() const
{
    return m_type;
}

QDateTime Meta::Version::time() const
{
    return QDateTime::fromMSecsSinceEpoch(m_time * 1000, Qt::UTC);
}

void Meta::Version::parse(const QJsonObject& obj)
{
    parseVersion(obj, this);
}

void Meta::Version::mergeFromList(const Meta::VersionPtr& other)
{
    if (other->m_providesRecommendations)
        setRecommended(other->m_recommended);
    if (m_type != other->m_type)
        setType(other->m_type);
    if (m_time != other->m_time)
        setTime(other->m_time);

    m_requires = other->m_requires;
    m_conflicts = other->m_conflicts;
    setVolatile(other->m_volatile);
}

void Meta::Version::merge(const VersionPtr &other)
{
    mergeFromList(other);
    if(other->m_data)
    {
        setData(other->m_data);
    }
}

QString Meta::Version::localFilename() const
{
    return m_uid + '/' + m_version + ".json";
}

void Meta::Version::setType(const QString &type)
{
    m_type = type;
    emit typeChanged();
}

void Meta::Version::setTime(const qint64 time)
{
    m_time = time;
    emit timeChanged();
}

void Meta::Version::setRequires(const Meta::RequireSet &requires, const Meta::RequireSet &conflicts)
{
    m_requires = requires;
    m_conflicts = conflicts;
    emit requiresChanged();
}

void Meta::Version::setVolatile(bool volatile_)
{
    m_volatile = volatile_;
}


void Meta::Version::setData(const VersionFilePtr &data)
{
    m_data = data;
}

void Meta::Version::setProvidesRecommendations()
{
    m_providesRecommendations = true;
}

void Meta::Version::setRecommended(bool recommended)
{
    m_recommended = recommended;
}
