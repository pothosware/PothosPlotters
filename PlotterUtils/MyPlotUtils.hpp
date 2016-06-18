// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include "PlotUtilsConfig.hpp"
#include <QColor>
#include <qwt_plot.h>
#include <QVariant>

class QwtPlotItem;
class QwtPlotZoomer;

//! Get a color for a plotter curve given an index
POTHOS_PLOTTER_UTILS_EXPORT QColor getDefaultCurveColor(const size_t whichCurve);

//! make a color have pastel-properties
POTHOS_PLOTTER_UTILS_EXPORT QColor pastelize(const QColor &c);

/*!
 * A QwtPlot extension class that has slots for certain things.
 */
class POTHOS_PLOTTER_UTILS_EXPORT MyQwtPlot : public QwtPlot
{
    Q_OBJECT
public:
    MyQwtPlot(QWidget *parent);

    //! update checked status based on visibility
    void updateChecked(QwtPlotItem *item);

    //! query the plot state as a QVariant
    QVariant state(void) const;

    //! restore the state from QVariant
    void setState(const QVariant &state);

    //! Get the plot zoomer for this canvas
    QwtPlotZoomer *zoomer(void) const
    {
        return _zoomer;
    }

public slots:
    void setTitle(const QString &text);
    void setAxisTitle(const int id, const QString &text);

private:
    QwtPlotZoomer *_zoomer;
};
