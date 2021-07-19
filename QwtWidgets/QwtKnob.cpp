// Copyright (c) 2016-2021 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <qwt_knob.h>
#include <qwt_scale_engine.h>
#include <QVariant>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMouseEvent>

/***********************************************************************
 * |PothosDoc QWT Knob
 *
 * A knob widget for angular control of a floating point value.
 * http://qwt.sourceforge.net/class_qwt_knob.html
 *
 * |category /Widgets
 * |keywords knob turn dial
 *
 * |param title The name of the value displayed by this widget
 * |default "My Knob"
 * |widget StringEntry()
 *
 * |param scaleEngine[Scale Engine] The scaling engine, linear or logarithmic.
 * |default "Linear"
 * |option [Linear] "Linear"
 * |option [Log2] "Log2"
 * |option [Log10] "Log10"
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
 * Use a value of 0.0 for automatic step-size calculation.
 * |default 0.0
 * |widget DoubleSpinBox()
 * |preview valid
 *
 * |mode graphWidget
 * |factory /widgets/qwt_knob()
 * |setter setTitle(title)
 * |setter setScaleEngine(scaleEngine)
 * |setter setLowerBound(lowerBound)
 * |setter setUpperBound(upperBound)
 * |setter setStepSize(stepSize)
 * |setter setValue(value)
 **********************************************************************/
class QwtKnobBlock : public QGroupBox, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(void)
    {
        return new QwtKnobBlock();
    }

    QwtKnobBlock(void):
        _knob(new QwtKnob(this))
    {
        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(QMargins());
        layout->addWidget(_knob);
        this->setStyleSheet("QGroupBox {font-weight: bold;}");

        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setTitle));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setLowerBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setUpperBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setStepSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setScaleEngine));
        this->registerSignal("valueChanged");
        connect(_knob, &QwtKnob::valueChanged, this, &QwtKnobBlock::handleValueChanged);
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
        return _knob->value();
    }

    void setValue(const double value)
    {
        _knob->setValue(value);
    }

    void setLowerBound(const double value)
    {
        _knob->setLowerBound(value);
    }

    void setUpperBound(const double value)
    {
        _knob->setUpperBound(value);
    }

    void setStepSize(const double step)
    {
        _knob->setScaleStepSize(step);
    }

    void setScaleEngine(const QString &engine)
    {
        if (engine == "Linear") _knob->setScaleEngine(new QwtLinearScaleEngine());
        if (engine == "Log2") _knob->setScaleEngine(new QwtLogScaleEngine(2));
        if (engine == "Log10") _knob->setScaleEngine(new QwtLogScaleEngine(10));
        _knob->setScale(_knob->lowerBound(), _knob->upperBound()); //refresh
    }

    void activate(void)
    {
        //emit current value when design becomes active
        this->emitSignal("valueChanged", this->value());
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
        this->emitSignal("valueChanged", value);
    }

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
    QwtKnob *_knob;
};

static Pothos::BlockRegistry registerQwtKnobBlock(
    "/widgets/qwt_knob", &QwtKnobBlock::make);

#include "QwtKnob.moc"
