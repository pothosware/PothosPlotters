// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <qwt_slider.h>
#include <qwt_scale_engine.h>
#include <QVariant>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMouseEvent>

/***********************************************************************
 * |PothosDoc QWT Slider
 *
 * A slider widget for graphical control of a floating point value.
 * http://qwt.sourceforge.net/class_qwt_slider.html
 *
 * |category /Widgets
 * |keywords slider
 *
 * |param title The name of the value displayed by this widget
 * |default "My Slider"
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
 * |factory /widgets/qwt_slider()
 * |setter setTitle(title)
 * |setter setOrientation(orientation)
 * |setter setScalePosition(scalePosition)
 * |setter setScaleEngine(scaleEngine)
 * |setter setLowerBound(lowerBound)
 * |setter setUpperBound(upperBound)
 * |setter setStepSize(stepSize)
 * |setter setValue(value)
 **********************************************************************/
class QwtSliderBlock : public QGroupBox, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(void)
    {
        return new QwtSliderBlock();
    }

    QwtSliderBlock(void):
        _slider(new QwtSlider(this))
    {
        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(QMargins());
        layout->addWidget(_slider);
        this->setStyleSheet("QGroupBox {font-weight: bold;}");

        _slider->setHandleSize(QSize(14, 14));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtSliderBlock, setTitle));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtSliderBlock, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtSliderBlock, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtSliderBlock, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtSliderBlock, setLowerBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtSliderBlock, setUpperBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtSliderBlock, setStepSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtSliderBlock, setOrientation));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtSliderBlock, setScalePosition));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtSliderBlock, setScaleEngine));
        this->registerSignal("valueChanged");
        connect(_slider, SIGNAL(valueChanged(const double)), this, SLOT(handleValueChanged(const double)));
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
        return _slider->value();
    }

    void setValue(const double value)
    {
        _slider->setValue(value);
    }

    void setLowerBound(const double value)
    {
        _slider->setLowerBound(value);
    }

    void setUpperBound(const double value)
    {
        _slider->setUpperBound(value);
    }

    void setOrientation(const QString &orientation)
    {
        if ((orientation == "Horizontal")) _slider->setOrientation(Qt::Horizontal);
        if ((orientation == "Vertical")) _slider->setOrientation(Qt::Vertical);
    }

    void setScalePosition(const QString &scale)
    {
        if ((scale == "NoScale")) _slider->setScalePosition(_slider->NoScale);
        if ((scale == "LeadingScale")) _slider->setScalePosition(_slider->LeadingScale);
        if ((scale == "TrailingScale")) _slider->setScalePosition(_slider->TrailingScale);
    }

    void setScaleEngine(const QString &engine)
    {
        if (engine == "Linear") _slider->setScaleEngine(new QwtLinearScaleEngine());
        if (engine == "Log2") _slider->setScaleEngine(new QwtLogScaleEngine(2));
        if (engine == "Log10") _slider->setScaleEngine(new QwtLogScaleEngine(10));
        _slider->setScale(_slider->lowerBound(), _slider->upperBound()); //refresh
    }

    void setStepSize(const double step)
    {
        _slider->setScaleStepSize(step);
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
    QwtSlider *_slider;
};

static Pothos::BlockRegistry registerQwtSliderBlock(
    "/widgets/qwt_slider", &QwtSliderBlock::make);

#include "QwtSlider.moc"
