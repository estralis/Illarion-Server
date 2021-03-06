//  illarionserver - server for the game Illarion
//  Copyright 2011 Illarion e.V.
//
//  This file is part of illarionserver.
//
//  illarionserver is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  illarionserver is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License
//  along with illarionserver.  If not, see <http://www.gnu.org/licenses/>.


#ifndef _WORLDMAP_HPP_
#define _WORLDMAP_HPP_

#include <vector>
#include <unordered_map>
#include "globals.hpp"
#include "Map.hpp"

class Field;

class WorldMap {
    std::vector<Map> maps;
    std::unordered_map<position, int> world_map;
    size_t ageIndex = 0;

public:
    void clear();

    Field &at(const position &pos);
    const Field &at(const position &pos) const;
    Field &walkableNear(position &pos);
    const Field &walkableNear(position &pos) const;
    bool intersects(const Map &map) const;

    bool allMapsAged();

    bool import(const std::string &importDir, const std::string &mapName);
    bool exportTo(const std::string &exportDir) const;
    bool loadFromDisk(const std::string &prefix);
    void saveToDisk(const std::string &prefix) const;
    bool createMap(const std::string &name, const position &origin,
                   uint16_t width, uint16_t height, uint16_t tile);

private:
    bool insert(Map&& map);
    static Map createMapFromHeaderFile(const std::string &importDir,
                                       const std::string &mapName);
    static int16_t readHeaderLine(const std::string &mapName, char header,
                                  std::ifstream &headerFile, int &lineNumber);
    static bool isCommentOrEmpty(const std::string &line);
};
#endif
