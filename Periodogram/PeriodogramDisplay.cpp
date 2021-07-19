// Copyright (c) 2014-2021 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "PeriodogramDisplay.hpp"
#include "PeriodogramChannel.hpp"
#include "PothosPlotter.hpp"
#include <QResizeEvent>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_plot_zoomer.h>
#include <QHBoxLayout>
#include <algorithm> //min/max

PeriodogramDisplay::PeriodogramDisplay(void):
    _mainPlot(new PothosPlotter(this, POTHOS_PLOTTER_GRID | POTHOS_PLOTTER_ZOOM)),
    _sampleRate(1.0),
    _sampleRateWoAxisUnits(1.0),
    _centerFreq(0.0),
    _centerFreqWoAxisUnits(0.0),
    _numBins(1024),
    _refLevel(0.0),
    _dynRange(100.0),
    _autoScale(false),
    _freqLabelId("rxFreq"),
    _rateLabelId("rxRate"),
    _averageFactor(0.0),
    _fullScale(1.0),
    _fftModeComplex(true),
    _fftModeAutomatic(true)
{
    //setup block
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, widget));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setTitle));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setSampleRate));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setCenterFrequency));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setNumFFTBins));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setWindowType));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setFullScale));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setFFTMode));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setReferenceLevel));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setDynamicRange));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setAutoScale));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, title));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, sampleRate));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, centerFrequency));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, numFFTBins));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, referenceLevel));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, dynamicRange));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, autoScale));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setAverageFactor));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, enableXAxis));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, enableYAxis));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setYAxisTitle));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setFreqLabelId));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, setRateLabelId));
    this->registerCall(this, POTHOS_FCN_TUPLE(PeriodogramDisplay, clearChannels));
    this->registerSlot("clearChannels");
    this->registerSignal("frequencySelected");
    this->registerSignal("relativeFrequencySelected");
    this->setupInput(0);

    //layout
    auto layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(QMargins());
    layout->addWidget(_mainPlot);

    //setup plotter
    {
        connect(_mainPlot->zoomer(), QOverload<const QPointF &>::of(&QwtPlotZoomer::selected), this, &PeriodogramDisplay::handlePickerSelected);
        connect(_mainPlot->zoomer(), &QwtPlotZoomer::zoomed, this, &PeriodogramDisplay::handleZoomed);

        auto legend = new QwtLegend(_mainPlot);
        legend->setDefaultItemMode(QwtLegendData::Checkable);
        connect(legend, &QwtLegend::checked, this, &PeriodogramDisplay::handleLegendChecked);
        _mainPlot->insertLegend(legend);
    }
}

PeriodogramDisplay::~PeriodogramDisplay(void)
{
    return;
}

void PeriodogramDisplay::setTitle(const QString &title)
{
    QMetaObject::invokeMethod(_mainPlot, "setTitle", Qt::QueuedConnection, Q_ARG(QString, title));
}

void PeriodogramDisplay::setSampleRate(const double sampleRate)
{
    _sampleRate = sampleRate;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void PeriodogramDisplay::setCenterFrequency(const double freq)
{
    _centerFreq = freq;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void PeriodogramDisplay::setNumFFTBins(const size_t numBins)
{
    _numBins = numBins;
}

void PeriodogramDisplay::setWindowType(const std::string &windowType, const std::vector<double> &windowArgs)
{
    _fftPowerSpectrum.setWindowType(windowType, windowArgs);
}

void PeriodogramDisplay::setFullScale(const double fullScale)
{
    _fullScale = fullScale;
}

void PeriodogramDisplay::setFFTMode(const std::string &fftMode)
{
    if (fftMode == "REAL"){}
    else if (fftMode == "COMPLEX"){}
    else if (fftMode == "AUTO"){}
    else throw Pothos::InvalidArgumentException("PeriodogramDisplay::setFFTMode("+fftMode+")", "unknown mode");
    _fftModeComplex = (fftMode != "REAL");
    _fftModeAutomatic = (fftMode == "AUTO");
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void PeriodogramDisplay::setReferenceLevel(const double refLevel)
{
    _refLevel = refLevel;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void PeriodogramDisplay::setDynamicRange(const double dynRange)
{
    _dynRange = dynRange;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void PeriodogramDisplay::setAutoScale(const bool autoScale)
{
    _autoScale = autoScale;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void PeriodogramDisplay::handleUpdateAxis(void)
{
    QString axisTitle("Hz");
    double factor = std::max(_sampleRate, _centerFreq);
    if (factor >= 2e9)
    {
        factor = 1e9;
        axisTitle = "GHz";
    }
    else if (factor >= 2e6)
    {
        factor = 1e6;
        axisTitle = "MHz";
    }
    else if (factor >= 2e3)
    {
        factor = 1e3;
        axisTitle = "kHz";
    }
    _mainPlot->setAxisTitle(QwtPlot::xBottom, axisTitle);

    _mainPlot->zoomer()->setAxis(QwtPlot::xBottom, QwtPlot::yLeft);
    _sampleRateWoAxisUnits = _sampleRate/factor;
    _centerFreqWoAxisUnits = _centerFreq/factor;
    const qreal freqLow = _fftModeComplex?(_centerFreqWoAxisUnits-_sampleRateWoAxisUnits/2):0.0;
    _mainPlot->setAxisScale(QwtPlot::xBottom, freqLow, _centerFreqWoAxisUnits+_sampleRateWoAxisUnits/2);
    _mainPlot->setAxisScale(QwtPlot::yLeft, _refLevel-_dynRange, _refLevel);
    _mainPlot->updateAxes(); //update after axis changes
    _mainPlot->zoomer()->setZoomBase(); //record current axis settings
    this->handleZoomed(_mainPlot->zoomer()->zoomBase()); //reload
}

QVariant PeriodogramDisplay::saveState(void) const
{
    auto map = _mainPlot->state().toMap();
    map["isComplex"] = _fftModeComplex;
    return map;
}

void PeriodogramDisplay::restoreState(const QVariant &state)
{
    const auto map = state.toMap();
    const auto isComplex = map.constFind("isComplex");
    if (isComplex != map.constEnd()) _fftModeComplex = isComplex.value().toBool();
    _mainPlot->setState(state);
}

void PeriodogramDisplay::handleZoomed(const QRectF &rect)
{
    //when zoomed all the way out, return to autoscale
    if (rect == _mainPlot->zoomer()->zoomBase() and _autoScale)
    {
        _mainPlot->setAxisAutoScale(QwtPlot::yLeft);
    }
}

void PeriodogramDisplay::handleClearChannels(void)
{
    _curves.clear();
}

QString PeriodogramDisplay::title(void) const
{
    return _mainPlot->title().text();
}

void PeriodogramDisplay::enableXAxis(const bool enb)
{
    QMetaObject::invokeMethod(_mainPlot, "enableAxis", Qt::QueuedConnection, Q_ARG(int, QwtPlot::xBottom), Q_ARG(bool, enb));
}

void PeriodogramDisplay::enableYAxis(const bool enb)
{
    QMetaObject::invokeMethod(_mainPlot, "enableAxis", Qt::QueuedConnection, Q_ARG(int, QwtPlot::yLeft), Q_ARG(bool, enb));
}

void PeriodogramDisplay::setYAxisTitle(const QString &title)
{
    QMetaObject::invokeMethod(_mainPlot, "setAxisTitle", Qt::QueuedConnection, Q_ARG(int, QwtPlot::yLeft), Q_ARG(QString, title));
}

void PeriodogramDisplay::handlePickerSelected(const QPointF &p)
{
    const double freq = p.x()*_sampleRate/_sampleRateWoAxisUnits;
    this->emitSignal("frequencySelected", freq);
    this->emitSignal("relativeFrequencySelected", freq - _centerFreq);
}

void PeriodogramDisplay::handleLegendChecked(const QVariant &itemInfo, bool on, int)
{
    auto item = _mainPlot->infoToItem(itemInfo);
    for (const auto &c : _curves)
    {
        if (item->isVisible() != on)
            c.second->clearOnChange(item);
    }
    item->setVisible(on);
    _mainPlot->replot();
}
