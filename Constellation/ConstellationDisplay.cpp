// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "ConstellationDisplay.hpp"
#include "PothosPlotter.hpp"
#include "PothosPlotUtils.hpp"
#include <QResizeEvent>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_curve.h>
#include <QHBoxLayout>

ConstellationDisplay::ConstellationDisplay(void):
    _mainPlot(new PothosPlotter(this, POTHOS_PLOTTER_GRID | POTHOS_PLOTTER_ZOOM)),
    _autoScale(false),
    _queueDepth(0),
    _curveStyle("DOTS"),
    _curveColor("blue")
{
    //setup block
    this->registerCall(this, POTHOS_FCN_TUPLE(ConstellationDisplay, widget));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConstellationDisplay, setTitle));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConstellationDisplay, setAutoScale));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConstellationDisplay, title));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConstellationDisplay, autoScale));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConstellationDisplay, setXRange));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConstellationDisplay, setYRange));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConstellationDisplay, enableXAxis));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConstellationDisplay, enableYAxis));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConstellationDisplay, setCurveStyle));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConstellationDisplay, setCurveColor));
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
    qRegisterMetaType<Pothos::BufferChunk>("Pothos::BufferChunk");
}

ConstellationDisplay::~ConstellationDisplay(void)
{
    return;
}

void ConstellationDisplay::setTitle(const QString &title)
{
    QMetaObject::invokeMethod(_mainPlot, "setTitle", Qt::QueuedConnection, Q_ARG(QString, title));
}

void ConstellationDisplay::setAutoScale(const bool autoScale)
{
    _autoScale = autoScale;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void ConstellationDisplay::setXRange(const std::vector<double> &range)
{
    if (range.size() != 2) throw Pothos::RangeException("ConstellationDisplay::setXRange()", "range vector must be size 2");
    _xRange = range;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void ConstellationDisplay::setYRange(const std::vector<double> &range)
{
    if (range.size() != 2) throw Pothos::RangeException("ConstellationDisplay::setYRange()", "range vector must be size 2");
    _yRange = range;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

QVariant ConstellationDisplay::saveState(void) const
{
    return _mainPlot->state();
}

void ConstellationDisplay::restoreState(const QVariant &state)
{
    _mainPlot->setState(state);
}

void ConstellationDisplay::handleUpdateAxis(void)
{
    if (_curve)
    {
        QwtPlotCurve::CurveStyle style(QwtPlotCurve::Dots);
        Qt::PenStyle penStyle(Qt::SolidLine);
        qreal width = 1.0;
        if (_curveStyle == "LINE") {style = QwtPlotCurve::Lines; penStyle = Qt::SolidLine; width = 1.0;}
        if (_curveStyle == "DASH") {style = QwtPlotCurve::Lines; penStyle = Qt::DashLine; width = 1.0;}
        if (_curveStyle == "DOTS") {style = QwtPlotCurve::Dots; penStyle = Qt::DotLine; width = 2.0;}
        _curve->setPen(pastelize(_curveColor), width, penStyle);
        _curve->setStyle(style);
    }

    if (_xRange.size() == 2) _mainPlot->setAxisScale(QwtPlot::xBottom, _xRange[0], _xRange[1]);
    if (_yRange.size() == 2) _mainPlot->setAxisScale(QwtPlot::yLeft, _yRange[0], _yRange[1]);

    _mainPlot->setAxisTitle(QwtPlot::xBottom, "In-Phase");
    _mainPlot->setAxisTitle(QwtPlot::yLeft, "Quadrature");

    _mainPlot->updateAxes(); //update after axis changes
    _mainPlot->zoomer()->setZoomBase(); //record current axis settings
    this->handleZoomed(_mainPlot->zoomer()->zoomBase()); //reload
}

void ConstellationDisplay::handleZoomed(const QRectF &rect)
{
    //when zoomed all the way out, return to autoscale
    if (rect == _mainPlot->zoomer()->zoomBase() and _autoScale)
    {
        _mainPlot->setAxisAutoScale(QwtPlot::xBottom);
        _mainPlot->setAxisAutoScale(QwtPlot::yLeft);
    }
}

QString ConstellationDisplay::title(void) const
{
    return _mainPlot->title().text();
}

void ConstellationDisplay::enableXAxis(const bool enb)
{
    _mainPlot->enableAxis(QwtPlot::xBottom, enb);
}

void ConstellationDisplay::enableYAxis(const bool enb)
{
    _mainPlot->enableAxis(QwtPlot::yLeft, enb);
}

void ConstellationDisplay::setCurveStyle(const std::string &style)
{
    _curveStyle = style;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}

void ConstellationDisplay::setCurveColor(const QString &color)
{
    _curveColor = color;
    QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
}
