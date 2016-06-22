// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <qwt_dial.h>
#include <qwt_scale_engine.h>
#include <qwt_dial_needle.h>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMouseEvent>

/***********************************************************************
 * |PothosDoc QWT Dial
 *
 * The dial widget is a circular status indicator for a floating point value.
 * http://qwt.sourceforge.net/class_qwt_dial.html
 *
 * |category /Widgets
 * |keywords dial turn compass
 *
 * |param title The name of the value displayed by this widget
 * |default "My Dial"
 * |widget StringEntry()
 *
 * |param mode The dial mode (rotate scale or rotate needle).
 * |default "RotateNeedle"
 * |option [Rotate Needle] "RotateNeedle"
 * |option [Rotate Scale] "RotateScale"
 * |preview disable
 *
 * |param needle The needle type (ray, arrow, compass, etc...).
 * |default "SimpleArrow"
 * |option [Simple Arrow] "SimpleArrow"
 * |option [Simple Ray] "SimpleRay"
 * |option [Compass Triangle] "CompassTriangle"
 * |option [Compass Thin] "CompassThin"
 * |option [Wind Arrow 1] "WindArrow1"
 * |option [Wind Arrow 2] "WindArrow2"
 * |preview disable
 * |tab Needle
 *
 * |param color1[Color 1] The first color for the dial's needle.
 * |widget ColorPicker()
 * |default "blue"
 * |preview disable
 * |tab Needle
 *
 * |param color2[Color 2] The second color for the dial's needle.
 * |widget ColorPicker()
 * |default "red"
 * |preview disable
 * |tab Needle
 *
 * |param scaleEngine[Scale Engine] The scaling engine, linear or logarithmic.
 * |default "Linear"
 * |option [Linear] "Linear"
 * |option [Log2] "Log2"
 * |option [Log10] "Log10"
 * |preview disable
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
 * |factory /widgets/qwt_dial()
 * |setter setTitle(title)
 * |setter setMode(mode)
 * |setter setNeedle(needle, color1, color2)
 * |setter setScaleEngine(scaleEngine)
 * |setter setLowerBound(lowerBound)
 * |setter setUpperBound(upperBound)
 * |setter setStepSize(stepSize)
 **********************************************************************/
class QwtDialBlock : public QGroupBox, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(void)
    {
        return new QwtDialBlock();
    }

    QwtDialBlock(void):
        _dial(new QwtDial(this))
    {
        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(QMargins());
        layout->addWidget(_dial);
        this->setStyleSheet("QGroupBox {font-weight: bold;}");

        this->registerCall(this, POTHOS_FCN_TUPLE(QwtDialBlock, setTitle));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtDialBlock, setMode));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtDialBlock, setNeedle));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtDialBlock, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtDialBlock, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtDialBlock, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtDialBlock, setLowerBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtDialBlock, setUpperBound));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtDialBlock, setStepSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(QwtDialBlock, setScaleEngine));
    }

    QWidget *widget(void)
    {
        return this;
    }

    void setTitle(const QString &title)
    {
        QMetaObject::invokeMethod(this, "handleSetTitle", Qt::QueuedConnection, Q_ARG(QString, title));
    }

    void setMode(const QString &mode)
    {
        if (mode == "RotateNeedle") _dial->setMode(QwtDial::RotateNeedle);
        if (mode == "RotateScale") _dial->setMode(QwtDial::RotateScale);
    }

    void setNeedle(const QString &type, const QString &color1, const QString &color2)
    {
        if (type == "SimpleArrow") _dial->setNeedle(new QwtDialSimpleNeedle(QwtDialSimpleNeedle::Arrow, true, color1, color2));
        if (type == "SimpleRay") _dial->setNeedle(new QwtDialSimpleNeedle(QwtDialSimpleNeedle::Ray, true, color1, color2));
        if (type == "CompassTriangle") _dial->setNeedle(new QwtCompassMagnetNeedle(QwtCompassMagnetNeedle::TriangleStyle, color1, color2));
        if (type == "CompassThin") _dial->setNeedle(new QwtCompassMagnetNeedle(QwtCompassMagnetNeedle::ThinStyle, color1, color2));
        if (type == "WindArrow1") _dial->setNeedle(new QwtCompassWindArrow(QwtCompassWindArrow::Style1, color1, color2));
        if (type == "WindArrow2") _dial->setNeedle(new QwtCompassWindArrow(QwtCompassWindArrow::Style2, color1, color2));
    }

    double value(void) const
    {
        return _dial->value();
    }

    void setValue(const double value)
    {
        _dial->setValue(value);
    }

    void setLowerBound(const double value)
    {
        _dial->setLowerBound(value);
    }

    void setUpperBound(const double value)
    {
        _dial->setUpperBound(value);
    }

    void setStepSize(const double step)
    {
        _dial->setScaleStepSize(step);
    }

    void setScaleEngine(const QString &engine)
    {
        if (engine == "Linear") _dial->setScaleEngine(new QwtLinearScaleEngine());
        if (engine == "Log2") _dial->setScaleEngine(new QwtLogScaleEngine(2));
        if (engine == "Log10") _dial->setScaleEngine(new QwtLogScaleEngine(10));
        _dial->setScale(_dial->lowerBound(), _dial->upperBound()); //refresh
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
    QwtDial *_dial;
};

static Pothos::BlockRegistry registerQwtDialBlock(
    "/widgets/qwt_dial", &QwtDialBlock::make);

#include "QwtDial.moc"
