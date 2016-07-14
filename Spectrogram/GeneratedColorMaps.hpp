// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <string>
#include <vector>
#include <utility>
#include <cstddef>

class QwtColorMap;

//! Get a listing of all title, name pairs for each color map
std::vector<std::pair<const std::string, const std::string>> availableColorMaps(void);

//! Get a color mapping given a name in [Z, R, G, B, A] format
std::vector<std::vector<double>> lookupColorMap(const std::string &name);

//! Make a QwtColorMap given the name of a mapping
QwtColorMap *makeQwtColorMap(const std::string &name);
