// Copyright (c) 2014-2017 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include "PlotUtilsConfig.hpp"
#include <qwt_plot.h>
#include <QVariant>
#include <QBitArray>

class QwtPlotItem;
class QwtPlotZoomer;
class QwtPlotGrid;

#define POTHOS_PLOTTER_GRID (1 << 0)
#define POTHOS_PLOTTER_ZOOM (1 << 1)

/*!
 * A QwtPlot extension class that has slots for certain things.
 */
class POTHOS_PLOTTER_UTILS_EXPORT PothosPlotter : public QwtPlot
{
    Q_OBJECT
public:
    PothosPlotter(QWidget *parent, const int enables = 0);

    ~PothosPlotter(void);

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
    void enableAxis(const int axisId, const bool tf = true);

private slots:
    void handleItemAttached(QwtPlotItem *plotItem, bool on);

private:
    QwtPlotZoomer *_zoomer;
    QwtPlotGrid *_grid;
    QBitArray _visible;
};
