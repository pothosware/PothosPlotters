// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <string>
#include <vector>
#include <utility>
#include <cstddef>

//! Get a listing of all title, name pairs for each color map
std::vector<std::pair<const std::string, const std::string>> available_color_maps(void);

//! Get a color mapping given a name in [Z, R, G, B, A] format
std::vector<std::vector<double>> make_color_map(const std::string &name);
