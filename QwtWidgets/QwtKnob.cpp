// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <qwt_knob.h>
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
 * |setter setTitle(title)
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
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(_knob);
        this->setStyleSheet("QGroupBox {font-weight: bold;}");

        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setTitle));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setLowerBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setUpperBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtKnobBlock, setStepSize));
        this->registerSignal("valueChanged");
        connect(_knob, SIGNAL(valueChanged(const double)), this, SLOT(handleValueChanged(const double)));
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
