// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <qwt_math.h> //_USE_MATH_DEFINES
#include <Pothos/Framework.hpp>
#include <Pothos/Proxy.hpp>
#include <QVariant>
#include <QWidget>
#include <memory>
#include <map>
#include <atomic>
#include <vector>

class PothosPlotter;
class QwtPlotCurve;

class ConstellationDisplay : public QWidget, public Pothos::Block
{
    Q_OBJECT
public:

    ConstellationDisplay(void);

    ~ConstellationDisplay(void);

    QWidget *widget(void)
    {
        return this;
    }

    //! set the plotter's title
    void setTitle(const QString &title);

    void setAutoScale(const bool autoScale);

    void setXRange(const std::vector<double> &range);

    void setYRange(const std::vector<double> &range);

    QString title(void) const;

    bool autoScale(void) const
    {
        return _autoScale;
    }

    void enableXAxis(const bool enb);
    void enableYAxis(const bool enb);

    void setCurveStyle(const std::string &style);
    void setCurveColor(const QString &color);

    void work(void);

    //allow for standard resize controls with the default size policy
    QSize minimumSizeHint(void) const
    {
        return QSize(200, 200);
    }
    QSize sizeHint(void) const
    {
        return this->minimumSizeHint();
    }

public slots:

    QVariant saveState(void) const;

    void restoreState(const QVariant &value);

private slots:
    void handleUpdateAxis(void);
    void handleSamples(const Pothos::BufferChunk &buff);
    void handleZoomed(const QRectF &rect);

private:
    PothosPlotter *_mainPlot;
    bool _autoScale;
    std::vector<double> _xRange;
    std::vector<double> _yRange;
    std::unique_ptr<QwtPlotCurve> _curve;
    std::atomic<size_t> _queueDepth;
    std::string _curveStyle;
    QString _curveColor;
};
