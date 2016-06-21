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
 * |param orientation The widget orientation (horizontal or veritical).
 * |default "Horizontal"
 * |option [Horizontal] "Horizontal"
 * |option [Veritical] "Veritical"
 * |preview disable
 *
 * |param scalePosition[Scale Position] The widget scale markers.
 * |default "NoScale"
 * |option [None] "NoScale"
 * |option [Leading] "LeadingScale"
 * |option [Trailing] "TrailingScale"
 * |preview disable
 *
 * |param value The initial value of this widget.
 * |default 0.0
 * |widget DoubleSpinBox()
 *
 * |param lowerBound[Lower Bound] The minimum value of this widget.
 * |default -1.0
 * |widget DoubleSpinBox()
 *
 * |param upperBound[Upper Bound] The maximum value of this widget.
 * |default 1.0
 * |widget DoubleSpinBox()
 *
 * |param stepSize[Step Size] The delta in-between discrete values in this widget.
 * |default 0.1
 * |widget DoubleSpinBox()
 *
 * |param fillColor[Fill Color] The color of the liquid in the thermometer display.
 * |widget ColorPicker(mode=pastel)
 * |default "#b19cd9"
 * |preview disable
 *
 * |param alarmEnabled[Alarm Enabled] True to enable the alarm state coloring.
 * |default false
 * |option [Enabled] true
 * |option [Disabled] false
 * |tab Alarm
 * |preview disable
 *
 * |param alarmLevel[Alarm Level] The threshold for entering the alarm state.
 * |default 0.5
 * |widget DoubleSpinBox()
 * |tab Alarm
 * |preview when(enum=alarmEnabled, true)
 *
 * |param alarmColor[Alarm Color] The color of the liquid in the thermometer display in alarm mode.
 * |widget ColorPicker(mode=pastel)
 * |default "#ff6961"
 * |tab Alarm
 * |preview disable
 *
 * |mode graphWidget
 * |factory /widgets/qwt_thermo()
 * |setter setOrientation(orientation)
 * |setter setScalePosition(scalePosition)
 * |setter setLowerBound(lowerBound)
 * |setter setUpperBound(upperBound)
 * |setter setStepSize(stepSize)
 * |setter setValue(value)
 * |setter setFillColor(fillColor)
 * |setter setAlarmEnabled(alarmEnabled)
 * |setter setAlarmLevel(alarmLevel)
 * |setter setAlarmColor(alarmColor)
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
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setFillColor));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setAlarmEnabled));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setAlarmLevel));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setAlarmColor));
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

    void setFillColor(const QString &color)
    {
        this->setFillBrush(QBrush(QColor(color)));
    }

    void setAlarmColor(const QString &color)
    {
        this->setAlarmBrush(QBrush(QColor(color)));
    }
};

static Pothos::BlockRegistry registerQwtThermoBlock(
    "/widgets/qwt_thermo", &QwtThermoBlock::make);

#include "QwtThermo.moc"
