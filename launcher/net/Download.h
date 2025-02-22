// SPDX-License-Identifier: GPL-3.0-only
/*
 *  PolyMC - Minecraft Launcher
 *  Copyright (c) 2022 flowln <flowlnlnln@gmail.com>
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

#pragma once

#include "HttpMetaCache.h"
#include "NetAction.h"
#include "Sink.h"
#include "Validator.h"

#include "QObjectPtr.h"

namespace Net {
class Download : public NetAction {
    Q_OBJECT

   public:
    using Ptr = shared_qobject_ptr<class Download>;
    enum class Option { NoOptions = 0, AcceptLocalFiles = 1, MakeEternal = 2 };
    Q_DECLARE_FLAGS(Options, Option)

   protected:
    explicit Download();

   public:
    ~Download() override = default;

    static auto makeCached(QUrl url, MetaEntryPtr entry, Options options = Option::NoOptions) -> Download::Ptr;
    static auto makeByteArray(QUrl url, QByteArray* output, Options options = Option::NoOptions) -> Download::Ptr;
    static auto makeFile(QUrl url, const QString& path, Options options = Option::NoOptions) -> Download::Ptr;

   public:
    void addValidator(Validator* v);
    auto abort() -> bool override;
    auto canAbort() const -> bool override { return true; };

   private:
    auto handleRedirect() -> bool;

   protected slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal) override;
    void downloadError(QNetworkReply::NetworkError error) override;
    void sslErrors(const QList<QSslError>& errors);
    void downloadFinished() override;
    void downloadReadyRead() override;

   public slots:
    void executeTask() override;

   private:
    std::unique_ptr<Sink> m_sink;
    Options m_options;
};
}  // namespace Net

Q_DECLARE_OPERATORS_FOR_FLAGS(Net::Download::Options)
