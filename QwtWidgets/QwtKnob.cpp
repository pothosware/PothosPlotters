// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <qwt_knob.h>
#include <QVariant>

/***********************************************************************
 * |PothosDoc QWT Knob
 *
 * A knob widget for angular control of a floating point value.
 * http://qwt.sourceforge.net/class_qwt_knob.html
 *
 * |category /Widgets
 * |keywords knob turn dial
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
 * |mode graphWidget
 * |factory /widgets/qwt_knob()
 * |setter setLowerBound(lowerBound)
 * |setter setUpperBound(upperBound)
 * |setter setStepSize(stepSize)
 * |setter setValue(value)
 **********************************************************************/
class QwtKnobBlock : public QwtKnob, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(void)
    {
        return new QwtKnobBlock();
    }

    QwtKnobBlock(void):
        QwtKnob(nullptr)
    {
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setLowerBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setUpperBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setStepSize));
        this->registerSignal("valueChanged");
        connect(this, SIGNAL(valueChanged(const double)), this, SLOT(handleValueChanged(const double)));
    }

    QWidget *widget(void)
    {
        return this;
    }

    void setStepSize(const double step)
    {
        this->setScaleStepSize(step);
    }

    void activate(void)
    {
        //emit current value when design becomes active
        this->callVoid("valueChanged", this->value());
    }

public slots:

    QVariant saveState(void) const
    {
        return this->value();
    }

    void restoreState(const QVariant &state)
    {
        this->setValue(state.toDouble());
    }

private slots:
    void handleValueChanged(const double value)
    {
        this->callVoid("valueChanged", value);
    }
};

static Pothos::BlockRegistry registerQwtKnobBlock(
    "/widgets/qwt_knob", &QwtKnobBlock::make);

#include "QwtKnob.moc"
