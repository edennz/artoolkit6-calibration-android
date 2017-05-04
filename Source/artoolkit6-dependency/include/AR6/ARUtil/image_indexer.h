/*
*  image_indexer.h
*  ARToolKit6
*
*  Implements a basic image indexing system.
*
*  This file is part of ARToolKit.
*
*  ARToolKit is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  ARToolKit is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public License
*  along with ARToolKit.  If not, see <http://www.gnu.org/licenses/>.
*
*  As a special exception, the copyright holders of this library give you
*  permission to link this library with independent modules to produce an
*  executable, regardless of the license terms of these independent modules, and to
*  copy and distribute the resulting executable under terms of your choice,
*  provided that you also meet, for each linked independent module, the terms and
*  conditions of the license of that module. An independent module is a module
*  which is neither derived from nor based on this library. If you modify this
*  library, you may extend this exception to your version of the library, but you
*  are not obligated to do so. If you do not wish to do so, delete this exception
*  statement from your version.
*
*  Copyright 2015-2016 Daqri, LLC.
*  Copyright 2007-2015 ARToolworks, Inc.
*
*  Author(s): Hirokazu Kato, Philip Lamb
*
*/

#ifndef image_indexer_h
#define image_indexer_h

#include <vector>
#include <string>
#include <sstream>
#include <map>

static const std::string imageIndexFileName = "imageIndex.dat";
static const std::string recognizerFileName = "recognizer.dat";
static const std::string unzipTargetDirectory = "/tmp/";
static const std::string processedTargetImageDirectory = "2DTrackerImages";

#include "file_utils.h"

class TrackableDatabaseInfo
{
public:
    TrackableDatabaseInfo() : index(-1), name(), fileSize(0.0f), height(0.0f), numberOfFeatures(0)
    {
    }
    int index;
    std::string name;
    std::string path;
    float fileSize;
    float height;
    int imageWidth;
    int imageHeight;
    int numberOfFeatures;
};

typedef std::map<int, TrackableDatabaseInfo>::iterator it_type;

void writeIndexToStream(std::string fileName, std::map<int, TrackableDatabaseInfo> indexMap);

void readIndexFromStream(std::string fileName, std::string imageDirectory, std::map<int, TrackableDatabaseInfo>& indexMap);

#endif /* image_indexer_h */
