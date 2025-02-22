// SPDX-License-Identifier: GPL-3.0-only
/*
 *  PolyMC - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "SkinUpload.h"

#include <QNetworkRequest>
#include <QHttpMultiPart>

#include "Application.h"

QByteArray getVariant(SkinUpload::Model model) {
    switch (model) {
        default:
            qDebug() << "Unknown skin type!";
        case SkinUpload::STEVE:
            return "CLASSIC";
        case SkinUpload::ALEX:
            return "SLIM";
    }
}

void SkinUpload::executeTask()
{
    QNetworkRequest request(QUrl("https://api.minecraftservices.com/minecraft/profile/skins"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_token).toLocal8Bit());
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart skin;
    skin.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    skin.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"skin.png\""));
    skin.setBody(m_skin);

    QHttpPart model;
    model.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"variant\""));
    model.setBody(getVariant(m_model));

    multiPart->append(skin);
    multiPart->append(model);

    QNetworkReply *rep = APPLICATION->network()->post(request, multiPart);
    m_reply = shared_qobject_ptr<QNetworkReply>(rep);

    setStatus(tr("Uploading skin"));
    connect(rep, &QNetworkReply::uploadProgress, this, &Task::setProgress);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(rep, SIGNAL(errorOccurred(QNetworkReply::NetworkError)), this, SLOT(downloadError(QNetworkReply::NetworkError)));
#else
    connect(rep, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadError(QNetworkReply::NetworkError)));
#endif
    connect(rep, SIGNAL(finished()), this, SLOT(downloadFinished()));
}

void SkinUpload::downloadError(QNetworkReply::NetworkError error)
{
    // error happened during download.
    qCritical() << "Network error: " << error;
    emitFailed(m_reply->errorString());
}

void SkinUpload::downloadFinished()
{
    // if the download failed
    if (m_reply->error() != QNetworkReply::NetworkError::NoError)
    {
        emitFailed(QString("Network error: %1").arg(m_reply->errorString()));
        m_reply.reset();
        return;
    }
    emitSucceeded();
}
