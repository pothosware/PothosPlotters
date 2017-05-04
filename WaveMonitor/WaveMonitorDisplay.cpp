// Copyright (c) 2014-2017 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "WaveMonitorDisplay.hpp"
#include "PothosPlotter.hpp"
#include "PothosPlotStyler.hpp"
#include "PothosPlotUtils.hpp"
#include <QResizeEvent>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_legend.h>
#include <QHBoxLayout>
#include <iostream>

WaveMonitorDisplay::WaveMonitorDisplay(void):
    _mainPlot(new PothosPlotter(this, POTHOS_PLOTTER_GRID | POTHOS_PLOTTER_ZOOM)),
    _sampleRate(1.0),
    _sampleRateWoAxisUnits(1.0),
    _numPoints(1024),
    _autoScale(false),
    _rateLabelId("rxRate"),
    _curveCount(0)
{
    //setup block
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, widget));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, setTitle));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, setSampleRate));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, setNumPoints));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, numInputs));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, title));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, sampleRate));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, numPoints));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, setAutoScale));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, setYRange));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, enableXAxis));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, enableYAxis));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, setYAxisTitle));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, setChannelLabel));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, setChannelStyle));
    this->registerCall(this, POTHOS_FCN_TUPLE(WaveMonitorDisplay, setRateLabelId));
    this->setupInput(0);

    //layout
    auto layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(QMargins());
    layout->addWidget(_mainPlot);

    //setup plotter
    {
        connect(_mainPlot->zoomer(), SIGNAL(zoomed(const QRectF &)), this, SLOT(handleZoomed(const QRectF &)));
    }

    //register types passed to gui thread from work
    qRegisterMetaType<Pothos::Packet>("Pothos::Packet");

    //setup trigger marker label
    _triggerMarkerLabel = PothosMarkerLabel("T");
    static const QColor orange("#FFA500");
    _triggerMarkerLabel.setBackgroundBrush(QBrush(orange));
}

WaveMonitorDisplay::~WaveMonitorDisplay(void)
{
    return;
}

void WaveMonitorDisplay::setTitle(const QString &title)
{
    QMetaObject::invokeMethod(_mainPlot, "setTitle", Qt::QueuedConnection, Q_ARG(QString, title));
}

void WaveMonitorDisplay::setSampleRate(const double sampleRate)
{
    _sampleRate = sampleRate;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void WaveMonitorDisplay::setNumPoints(const size_t numPoints)
{
    _numPoints = numPoints;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

QString WaveMonitorDisplay::title(void) const
{
    return _mainPlot->title().text();
}

void WaveMonitorDisplay::setAutoScale(const bool autoScale)
{
    _autoScale = autoScale;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void WaveMonitorDisplay::setYRange(const std::vector<double> &range)
{
    if (range.size() != 2) throw Pothos::RangeException("WaveMonitorDisplay::setYRange()", "range vector must be size 2");
    _yRange = range;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void WaveMonitorDisplay::enableXAxis(const bool enb)
{
    _mainPlot->enableAxis(QwtPlot::xBottom, enb);
}

void WaveMonitorDisplay::enableYAxis(const bool enb)
{
    _mainPlot->enableAxis(QwtPlot::yLeft, enb);
}

void WaveMonitorDisplay::setYAxisTitle(const QString &title)
{
    QMetaObject::invokeMethod(_mainPlot, "setAxisTitle", Qt::QueuedConnection, Q_ARG(int, QwtPlot::yLeft), Q_ARG(QString, title));
}

void WaveMonitorDisplay::handleUpdateAxis(void)
{
    if (_yRange.size() == 2) _mainPlot->setAxisScale(QwtPlot::yLeft, _yRange[0], _yRange[1]);

    QString timeAxisTitle("secs");
    double factor = 1.0;

    double timeSpan = _numPoints/_sampleRate;
    if (timeSpan <= 100e-9)
    {
        factor = 1e9;
        timeAxisTitle = "nsecs";
    }
    else if (timeSpan <= 100e-6)
    {
        factor = 1e6;
        timeAxisTitle = "usecs";
    }
    else if (timeSpan <= 100e-3)
    {
        factor = 1e3;
        timeAxisTitle = "msecs";
    }
    _mainPlot->setAxisTitle(QwtPlot::xBottom, timeAxisTitle);

    _mainPlot->zoomer()->setAxis(QwtPlot::xBottom, QwtPlot::yLeft);
    _sampleRateWoAxisUnits = _sampleRate/factor;
    _mainPlot->setAxisScale(QwtPlot::xBottom, 0, _numPoints/_sampleRateWoAxisUnits);
    _mainPlot->updateAxes(); //update after axis changes
    _mainPlot->zoomer()->setZoomBase(); //record current axis settings
    this->handleZoomed(_mainPlot->zoomer()->zoomBase()); //reload
}

void WaveMonitorDisplay::handleUpdateCurves(void)
{
    size_t count = 0;

    for (auto &pair : _curves)
    {
        const auto &index = pair.first;
        auto &curves = pair.second;
        const auto &label = _channelLabels[index];
        const auto &styleStr = _channelStyles[index];

        QwtPlotCurve::CurveStyle style(QwtPlotCurve::Dots);
        Qt::PenStyle penStyle(Qt::SolidLine);
        qreal width = 1.0;
        if (styleStr == "LINE") {style = QwtPlotCurve::Lines; penStyle = Qt::SolidLine; width = 1.0;}
        if (styleStr == "DASH") {style = QwtPlotCurve::Lines; penStyle = Qt::DashLine; width = 1.0;}
        if (styleStr == "DOTS") {style = QwtPlotCurve::Dots; penStyle = Qt::DotLine; width = 2.0;}

        for (const auto &curvePair : curves)
        {
            const auto &curve = curvePair.second;
            const auto color = pastelize(getDefaultCurveColor(count++));
            curve->setPen(color, width, penStyle);
            curve->setStyle(style);
            curve->detach();
            curve->attach(_mainPlot);
            _mainPlot->updateChecked(curve.get());
        }

        if (label.isEmpty())
        {
            if (curves.size() == 1)
            {
                curves[0]->setTitle(QString("Ch%1").arg(index));
            }
            if (curves.size() == 2)
            {
                curves[0]->setTitle(QString("Re%1").arg(index));
                curves[1]->setTitle(QString("Im%1").arg(index));
            }
        }
        else
        {
            if (curves.size() == 1)
            {
                curves[0]->setTitle(label);
            }
            if (curves.size() == 2)
            {
                curves[0]->setTitle(QString("%1I").arg(label));
                curves[1]->setTitle(QString("%1Q").arg(label));
            }
        }
    }

    _mainPlot->replot();
}

void WaveMonitorDisplay::handleZoomed(const QRectF &rect)
{
    //when zoomed all the way out, return to autoscale
    if (rect == _mainPlot->zoomer()->zoomBase() and _autoScale)
    {
        _mainPlot->setAxisAutoScale(QwtPlot::yLeft);
        _mainPlot->updateAxes(); //update after axis changes
    }
}

QVariant WaveMonitorDisplay::saveState(void) const
{
    return _mainPlot->state();
}

void WaveMonitorDisplay::restoreState(const QVariant &state)
{
    _mainPlot->setState(state);
}

void WaveMonitorDisplay::installLegend(void)
{
    if (_mainPlot->legend() != nullptr) return;
    auto legend = new QwtLegend(_mainPlot);
    legend->setDefaultItemMode(QwtLegendData::Checkable);
    connect(legend, SIGNAL(checked(const QVariant &, bool, int)), this, SLOT(handleLegendChecked(const QVariant &, bool, int)));
    _mainPlot->insertLegend(legend);
}

void WaveMonitorDisplay::handleLegendChecked(const QVariant &itemInfo, bool on, int)
{
    _mainPlot->infoToItem(itemInfo)->setVisible(on);
    _mainPlot->replot();

    for (const auto &pair : _curves)
    {
        const bool visible = pair.second.at(0)->isVisible();
        for (const auto &m : _markers[pair.first]) m->setVisible(visible);
    }
}

std::shared_ptr<QwtPlotCurve> &WaveMonitorDisplay::getCurve(const size_t index, const size_t which, const size_t width)
{
    auto &curves = _curves[index];

    //detect width change (was complex data, now its real)
    if (curves.size() > width)
    {
        _curveCount -= curves.size();
        curves.clear();
        if (_curveCount <= 1) _mainPlot->insertLegend(nullptr);
    }

    auto &curve = curves[which];
    if (not curve)
    {
        curve.reset(new QwtPlotCurve());
        if (_curveCount++ == 1) this->installLegend();
        this->handleUpdateCurves();
    }
    return curve;
}
