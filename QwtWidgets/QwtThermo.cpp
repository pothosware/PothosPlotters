// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <qwt_thermo.h>
#include <qwt_scale_engine.h>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMouseEvent>

/***********************************************************************
 * |PothosDoc QWT Thermo
 *
 * A thermometer-like status indicator for a floating point value.
 * http://qwt.sourceforge.net/class_qwt_thermo.html
 *
 * |category /Widgets
 * |keywords thermo indicator status
 *
 * |param title The name of the value displayed by this widget
 * |default "My Thermo"
 * |widget StringEntry()
 *
 * |param orientation The widget orientation (horizontal or vertical).
 * |default "Horizontal"
 * |option [Horizontal] "Horizontal"
 * |option [Vertical] "Vertical"
 * |preview disable
 *
 * |param scalePosition[Scale Position] The widget scale markers.
 * |default "NoScale"
 * |option [None] "NoScale"
 * |option [Leading] "LeadingScale"
 * |option [Trailing] "TrailingScale"
 * |preview disable
 * |tab Scale
 *
 * |param scaleEngine[Scale Engine] The scaling engine, linear or logarithmic.
 * |default "Linear"
 * |option [Linear] "Linear"
 * |option [Log2] "Log2"
 * |option [Log10] "Log10"
 * |preview disable
 * |tab Scale
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
 * Use a value of 0.0 for automatic step-size calculation.
 * |default 0.0
 * |widget DoubleSpinBox()
 * |preview valid
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
 * |setter setTitle(title)
 * |setter setOrientation(orientation)
 * |setter setScalePosition(scalePosition)
 * |setter setScaleEngine(scaleEngine)
 * |setter setLowerBound(lowerBound)
 * |setter setUpperBound(upperBound)
 * |setter setStepSize(stepSize)
 * |setter setFillColor(fillColor)
 * |setter setAlarmEnabled(alarmEnabled)
 * |setter setAlarmLevel(alarmLevel)
 * |setter setAlarmColor(alarmColor)
 **********************************************************************/
class QwtThermoBlock : public QGroupBox, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(void)
    {
        return new QwtThermoBlock();
    }

    QwtThermoBlock(void):
        _thermo(new QwtThermo(this))
    {
        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(QMargins());
        layout->addWidget(_thermo);
        this->setStyleSheet("QGroupBox {font-weight: bold;}");

        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setTitle));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setLowerBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setUpperBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setStepSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setOrientation));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setScalePosition));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setScaleEngine));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setFillColor));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setAlarmEnabled));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setAlarmLevel));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtThermoBlock, setAlarmColor));
    }

    QWidget *widget(void)
    {
        return this;
    }

    void setTitle(const QString &title)
    {
        QMetaObject::invokeMethod(this, "handleSetTitle", Qt::QueuedConnection, Q_ARG(QString, title));
    }

    double value(void) const
    {
        return _thermo->value();
    }

    void setValue(const double value)
    {
        _thermo->setValue(value);
    }

    void setLowerBound(const double value)
    {
        _thermo->setLowerBound(value);
    }

    void setUpperBound(const double value)
    {
        _thermo->setUpperBound(value);
    }

    void setAlarmEnabled(const bool enb)
    {
        _thermo->setAlarmEnabled(enb);
    }

    void setAlarmLevel(const double level)
    {
        _thermo->setAlarmLevel(level);
    }

    void setOrientation(const QString &orientation)
    {
        if ((orientation == "Horizontal")) _thermo->setOrientation(Qt::Horizontal);
        if ((orientation == "Vertical")) _thermo->setOrientation(Qt::Vertical);
    }

    void setScalePosition(const QString &scale)
    {
        if ((scale == "NoScale")) _thermo->setScalePosition(_thermo->NoScale);
        if ((scale == "LeadingScale")) _thermo->setScalePosition(_thermo->LeadingScale);
        if ((scale == "TrailingScale")) _thermo->setScalePosition(_thermo->TrailingScale);
    }

    void setScaleEngine(const QString &engine)
    {
        if (engine == "Linear") _thermo->setScaleEngine(new QwtLinearScaleEngine());
        if (engine == "Log2") _thermo->setScaleEngine(new QwtLogScaleEngine(2));
        if (engine == "Log10") _thermo->setScaleEngine(new QwtLogScaleEngine(10));
        _thermo->setScale(_thermo->lowerBound(), _thermo->upperBound()); //refresh
    }

    void setStepSize(const double step)
    {
        _thermo->setScaleStepSize(step);
    }

    void setFillColor(const QString &color)
    {
        _thermo->setFillBrush(QBrush(QColor(color)));
    }

    void setAlarmColor(const QString &color)
    {
        _thermo->setAlarmBrush(QBrush(QColor(color)));
    }

private slots:

    void handleSetTitle(const QString &title)
    {
        QGroupBox::setTitle(title);
    }

protected:
    void mousePressEvent(QMouseEvent *event)
    {
        QGroupBox::mousePressEvent(event);
        event->ignore(); //allows for dragging from QGroupBox title
    }

private:
    QwtThermo *_thermo;
};

static Pothos::BlockRegistry registerQwtThermoBlock(
    "/widgets/qwt_thermo", &QwtThermoBlock::make);

#include "QwtThermo.moc"
