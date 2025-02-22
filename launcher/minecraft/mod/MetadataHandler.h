// SPDX-License-Identifier: GPL-3.0-only
/*
*  PolyMC - Minecraft Launcher
*  Copyright (c) 2022 flowln <flowlnlnln@gmail.com>
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
*/

#pragma once

#include <memory>

#include "modplatform/packwiz/Packwiz.h"

// launcher/minecraft/mod/Mod.h
class Mod;

/* Abstraction file for easily changing the way metadata is stored / handled
 * Needs to be a class because of -Wunused-function and no C++17 [[maybe_unused]]
 * */
class Metadata {
   public:
    using ModStruct = Packwiz::V1::Mod;

    static auto create(const QDir& index_dir, const ModPlatform::IndexedPack& mod_pack, const ModPlatform::IndexedVersion& mod_version) -> ModStruct
    {
        return Packwiz::V1::createModFormat(mod_pack, mod_version);
    }

    static auto create(const QDir& index_dir, const Mod& internal_mod, const QString& mod_slug) -> ModStruct
    {
        return Packwiz::V1::createModFormat(index_dir, mod_slug);
    }

    static void update(const QDir& index_dir, ModStruct& mod)
    {
        Packwiz::V1::updateModIndex(index_dir, mod);
    }

    static void remove(const QDir& index_dir, const QString& mod_slug)
    {
        Packwiz::V1::deleteModIndex(index_dir, mod_slug);
    }

    static void remove(const QDir& index_dir, const QVariant& mod_id)
    {
        Packwiz::V1::deleteModIndex(index_dir, mod_id);
    }

    static auto get(const QDir& index_dir, const QString& mod_slug) -> ModStruct
    {
        return Packwiz::V1::getIndexForMod(index_dir, mod_slug);
    }

    static auto get(const QDir& index_dir, const QVariant& mod_id) -> ModStruct
    {
        return Packwiz::V1::getIndexForMod(index_dir, mod_id);
    }
};
