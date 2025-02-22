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

#include "NetJob.h"

auto NetJob::addNetAction(NetAction::Ptr action) -> bool
{
    action->m_index_within_job = m_queue.size();
    m_queue.append(action);

    action->setNetwork(m_network);

    return true;
}

void NetJob::startNext()
{
    if (m_queue.isEmpty() && m_doing.isEmpty()) {
        // We're finished, check for failures and retry if we can (up to 3 times)
        if (!m_failed.isEmpty() && m_try < 3) {
            m_try += 1;
            while (!m_failed.isEmpty())
                m_queue.enqueue(m_failed.take(*m_failed.keyBegin()));
        }
    }

    ConcurrentTask::startNext();
}

auto NetJob::size() const -> int
{
    return m_queue.size() + m_doing.size() + m_done.size();
}

auto NetJob::canAbort() const -> bool
{
    bool canFullyAbort = true;

    // can abort the downloads on the queue?
    for (auto part : m_queue)
        canFullyAbort &= part->canAbort();

    // can abort the active downloads?
    for (auto part : m_doing)
        canFullyAbort &= part->canAbort();

    return canFullyAbort;
}

auto NetJob::abort() -> bool
{
    bool fullyAborted = true;

    // fail all downloads on the queue
    for (auto task : m_queue)
        m_failed.insert(task.get(), task);
    m_queue.clear();

    // abort active downloads
    auto toKill = m_doing.values();
    for (auto part : toKill) {
        fullyAborted &= part->abort();
    }

    if (fullyAborted)
        emitAborted();
    else
        emitFailed(tr("Failed to abort all tasks in the NetJob!"));

    return fullyAborted;
}

auto NetJob::getFailedActions() -> QList<NetAction*>
{
    QList<NetAction*> failed;
    failed.reserve(m_failed.size());
    std::transform(m_failed.cbegin(), m_failed.cend(), std::back_inserter(failed), [](const auto& index) { return dynamic_cast<NetAction*>(index.get()); });
    return failed;
}

auto NetJob::getFailedFiles() -> QList<QString>
{
    QList<QString> failed;
    failed.reserve(m_failed.size());
    std::transform(m_failed.cbegin(), m_failed.cend(), std::back_inserter(failed), [](const auto& index) { return static_cast<NetAction*>(index.get())->url().toString(); });
    return failed;
}

void NetJob::updateState()
{
    emit progress(m_done.count(), m_total_size);
    setStatus(tr("Executing %1 task(s) (%2 out of %3 are done)")
                  .arg(QString::number(m_doing.count()), QString::number(m_done.count()), QString::number(m_total_size)));
}
