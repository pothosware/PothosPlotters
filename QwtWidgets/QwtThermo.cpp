// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <qwt_thermo.h>
#include <QVariant>

/***********************************************************************
 * |PothosDoc QWT Thermo
 *
 * A thermometer-like status indicator for a floating point value.
 * http://qwt.sourceforge.net/class_qwt_thermo.html
 *
 * |category /Widgets
 * |keywords thermo indicator status
 *
 * |param orientation The thermo orientation (horizontal or veritical).
 * |default "Horizontal"
 * |option [Horizontal] "Horizontal"
 * |option [Veritical] "Veritical"
 * |preview disable
 *
 * |param scalePosition[Scale Position] The thermo scale markers.
 * |default "NoScale"
 * |option [None] "NoScale"
 * |option [Leading] "LeadingScale"
 * |option [Trailing] "TrailingScale"
 * |preview disable
 *
 * |param value The initial value of this slider.
 * |default 0.0
 * |widget DoubleSpinBox()
 *
 * |param lowerBound[Lower Bound] The minimum value of this slider.
 * |default -1.0
 * |widget DoubleSpinBox()
 *
 * |param upperBound[Upper Bound] The maximum value of this slider.
 * |default 1.0
 * |widget DoubleSpinBox()
 *
 * |param stepSize[Step Size] The delta inbetween discrete values in this slider.
 * |default 0.1
 * |widget DoubleSpinBox()
 *
 * |mode graphWidget
 * |factory /widgets/qwt_thermo()
 * |setter setOrientation(orientation)
 * |setter setScalePosition(scalePosition)
 * |setter setLowerBound(lowerBound)
 * |setter setUpperBound(upperBound)
 * |setter setStepSize(stepSize)
 * |setter setValue(value)
 **********************************************************************/
class QwtThermoBlock : public QwtThermo, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(void)
    {
        return new QwtThermoBlock();
    }

    QwtThermoBlock(void):
        QwtThermo(nullptr)
    {
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setLowerBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setUpperBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setStepSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setOrientation));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setScalePosition));
    }

    QWidget *widget(void)
    {
        return this;
    }

    void setOrientation(const QString &orientation)
    {
        if ((orientation == "Horizontal")) QwtThermo::setOrientation(Qt::Horizontal);
        if ((orientation == "Vertical")) QwtThermo::setOrientation(Qt::Vertical);
    }

    void setScalePosition(const QString &scale)
    {
        if ((scale == "NoScale")) QwtThermo::setScalePosition(QwtThermo::NoScale);
        if ((scale == "LeadingScale")) QwtThermo::setScalePosition(QwtThermo::LeadingScale);
        if ((scale == "TrailingScale")) QwtThermo::setScalePosition(QwtThermo::TrailingScale);
    }

    void setStepSize(const double step)
    {
        this->setScaleStepSize(step);
    }
};

static Pothos::BlockRegistry registerQwtThermoBlock(
    "/widgets/qwt_thermo", &QwtThermoBlock::make);

#include "QwtThermo.moc"
