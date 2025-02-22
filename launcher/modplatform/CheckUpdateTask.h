#pragma once

#include "minecraft/mod/Mod.h"
#include "modplatform/ModAPI.h"
#include "modplatform/ModIndex.h"
#include "tasks/Task.h"

class ModDownloadTask;
class ModFolderModel;

class CheckUpdateTask : public Task {
    Q_OBJECT

   public:
    CheckUpdateTask(QList<Mod*>& mods, std::list<Version>& mcVersions, ModAPI::ModLoaderTypes loaders, std::shared_ptr<ModFolderModel> mods_folder)
        : Task(nullptr), m_mods(mods), m_game_versions(mcVersions), m_loaders(loaders), m_mods_folder(mods_folder) {};

    struct UpdatableMod {
        QString name;
        QString old_hash;
        QString old_version;
        QString new_version;
        QString changelog;
        ModPlatform::Provider provider;
        ModDownloadTask* download;

       public:
        UpdatableMod(const QString & name, const QString & old_h, const QString & old_v, const QString & new_v, const QString & changelog, ModPlatform::Provider p, ModDownloadTask* t)
            : name(name), old_hash(old_h), old_version(old_v), new_version(new_v), changelog(changelog), provider(p), download(t)
        {}
    };

    auto getUpdatable() -> std::vector<UpdatableMod>&& { return std::move(m_updatable); }

   public slots:
    bool abort() override = 0;

   protected slots:
    void executeTask() override = 0;

   signals:
    void checkFailed(Mod* failed, QString reason, QUrl recover_url = {});

   protected:
    QList<Mod*>& m_mods;
    std::list<Version>& m_game_versions;
    ModAPI::ModLoaderTypes m_loaders;
    std::shared_ptr<ModFolderModel> m_mods_folder;

    std::vector<UpdatableMod> m_updatable;
};
