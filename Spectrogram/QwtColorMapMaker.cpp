// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Exception.hpp>
#include "GeneratedColorMaps.hpp"
#include <qwt_color_map.h>

static QColor vecToColor(const std::vector<double> &vec)
{
    std::vector<double> colorsF(vec);
    colorsF.resize(5, 1.0); //last 4 entries are r, g, b, a with 1.0 default
    return QColor::fromRgbF(colorsF[1], colorsF[2], colorsF[3], colorsF[4]);
}

QwtColorMap *makeQwtColorMap(const std::string &name)
{
    const auto colorMapData = lookupColorMap(name);
    if (colorMapData.size() < 2) throw Pothos::InvalidArgumentException("color map lookup failed: "+name);
    auto cMap = new QwtLinearColorMap(vecToColor(colorMapData.front()), vecToColor(colorMapData.back()));
    for (size_t i = 1; i < colorMapData.size()-1; i++)
    {
        const auto &vec = colorMapData.at(i);
        cMap->addColorStop(vec.at(0), vecToColor(vec));
    }
    return cMap;
}
