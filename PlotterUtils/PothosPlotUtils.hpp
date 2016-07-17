// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include "PlotUtilsConfig.hpp"
#include <QColor>

//! Get a color for a plotter curve given an index
POTHOS_PLOTTER_UTILS_EXPORT QColor getDefaultCurveColor(const size_t whichCurve);

//! make a color have pastel-properties
POTHOS_PLOTTER_UTILS_EXPORT QColor pastelize(const QColor &c);
