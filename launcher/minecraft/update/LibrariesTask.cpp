#include "LibrariesTask.h"

#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"

#include "Application.h"

void LibrariesTask::executeTask()
{
    setStatus(tr("Getting the library files from Mojang..."));
    qDebug() << m_inst->name() << ": downloading libraries";

    // Build a list of URLs that will need to be downloaded.
    auto components = m_inst->getPackProfile();
    auto profile = components->getProfile();

    auto job = new NetJob(tr("Libraries for instance %1").arg(m_inst->name()), APPLICATION->network());
    downloadJob.reset(job);

    auto metacache = APPLICATION->metacache();

    auto processArtifactPool = [&](const QList<LibraryPtr> & pool, QStringList & errors, const QString & localPath)
    {
        for (auto lib : pool)
        {
            if(!lib)
            {
                emitFailed(tr("Null jar is specified in the metadata, aborting."));
                return false;
            }
            auto dls = lib->getDownloads(m_inst->runtimeContext(), metacache.get(), errors, localPath);
            for(auto dl : dls)
            {
                downloadJob->addNetAction(dl);
            }
        }
        return true;
    };

    QStringList failedLocalLibraries;
    QList<LibraryPtr> libArtifactPool;
    libArtifactPool.append(profile->getLibraries());
    libArtifactPool.append(profile->getNativeLibraries());
    libArtifactPool.append(profile->getMavenFiles());
    for (auto agent : profile->getAgents())
    {
        libArtifactPool.append(agent->library());
    }
    libArtifactPool.append(profile->getMainJar());
    processArtifactPool(libArtifactPool, failedLocalLibraries, m_inst->getLocalLibraryPath());

    QStringList failedLocalJarMods;
    processArtifactPool(profile->getJarMods(), failedLocalJarMods, m_inst->jarModsDir());

    if (!failedLocalJarMods.empty() || !failedLocalLibraries.empty())
    {
        downloadJob.reset();
        QString failed_all = (failedLocalLibraries + failedLocalJarMods).join("\n");
        emitFailed(tr("Some artifacts marked as 'local' are missing their files:\n%1\n\nYou need to either add the files, or removed the packages that require them.\nYou'll have to correct this problem manually.").arg(failed_all));
        return;
    }

    connect(downloadJob.get(), &NetJob::succeeded, this, &LibrariesTask::emitSucceeded);
    connect(downloadJob.get(), &NetJob::failed, this, &LibrariesTask::jarlibFailed);
    connect(downloadJob.get(), &NetJob::aborted, this, [this]{ emitFailed(tr("Aborted")); });
    connect(downloadJob.get(), &NetJob::progress, this, &LibrariesTask::progress);
    downloadJob->start();
}

bool LibrariesTask::canAbort() const
{
    return true;
}

void LibrariesTask::jarlibFailed(QString reason)
{
    emitFailed(tr("Game update failed: it was impossible to fetch the required libraries.\nReason:\n%1").arg(reason));
}

bool LibrariesTask::abort()
{
    if (downloadJob)
        return downloadJob->abort();
    qWarning() << "Prematurely aborted LibrariesTask";
    return true;
}
