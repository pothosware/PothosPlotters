// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "SpectrogramDisplay.hpp"
#include <qwt_plot.h>
#include <QTimer>
#include <complex>

/***********************************************************************
 * initialization functions
 **********************************************************************/
void SpectrogramDisplay::activate(void)
{
    QMetaObject::invokeMethod(_replotTimer, "start", Qt::QueuedConnection);
}

void SpectrogramDisplay::deactivate(void)
{
    QMetaObject::invokeMethod(_replotTimer, "stop", Qt::QueuedConnection);
}

/***********************************************************************
 * work function
 **********************************************************************/
void SpectrogramDisplay::work(void)
{
    auto updateRate = this->height()/_timeSpan;
    if (updateRate != _lastUpdateRate) this->call("updateRateChanged", updateRate);
    _lastUpdateRate = updateRate;

    auto inPort = this->input(0);
    if (not inPort->hasMessage()) return;
    const auto msg = inPort->popMessage();

    //label-based messages have in-line commands
    if (msg.type() == typeid(Pothos::Label))
    {
        const auto &label = msg.convert<Pothos::Label>();
        if (label.id == _freqLabelId and label.data.canConvert(typeid(double)))
        {
            this->setCenterFrequency(label.data.convert<double>());
        }
        if (label.id == _rateLabelId and label.data.canConvert(typeid(double)))
        {
            this->setSampleRate(label.data.convert<double>());
        }
    }

    //packet-based messages have payloads to FFT
    if (msg.type() == typeid(Pothos::Packet))
    {
        const auto &buff = msg.convert<Pothos::Packet>().payload;
        auto floatBuff = buff.convert(Pothos::DType(typeid(std::complex<float>)), buff.elements());

        //safe guard against FFT size changes, old buffers could still be in-flight
        if (floatBuff.elements() != this->numFFTBins()) return;

        //handle automatic FFT mode
        if (_fftModeAutomatic)
        {
            const bool isComplex = buff.dtype.isComplex();
            const bool changed = _fftModeComplex != isComplex;
            _fftModeComplex = isComplex;
            if (changed) QMetaObject::invokeMethod(this, "handleUpdateAxis", Qt::QueuedConnection);
        }

        //power bins to points on the curve
        CArray fftBins(floatBuff.as<const std::complex<float> *>(), this->numFFTBins());
        const auto powerBins = _fftPowerSpectrum.transform(fftBins, _fullScale);
        this->appendBins(powerBins);
    }
}
