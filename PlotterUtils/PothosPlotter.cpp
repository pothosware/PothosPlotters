// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "PothosPlotter.hpp"
#include "PothosPlotPicker.hpp"
#include <QList>
#include <valarray>
#include <qwt_legend_data.h>
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_text.h>
#include <qwt_plot_grid.h>
#include <QMouseEvent>

/***********************************************************************
 * Custom Fonts for styling
 **********************************************************************/
static QFont PothosPlotAxisFont(void)
{
    QFont f;
    f.setPointSize(7);
    f.setWeight(QFont::Normal);
    f.setStretch(QFont::SemiCondensed);
    return f;
}

static QFont PothosPlotAxisTitleFont(void)
{
    QFont f;
    f.setPointSize(7);
    f.setWeight(QFont::DemiBold);
    f.setStretch(QFont::SemiExpanded);
    return f;
}

static QFont PothosPlotTitleFont(void)
{
    QFont f;
    f.setPointSize(8);
    f.setWeight(QFont::Bold);
    f.setStretch(QFont::SemiExpanded);
    return f;
}

/***********************************************************************
 * Custom QwtPlotCanvas that accepts the mousePressEvent
 * (needed to make mouse-based events work within QGraphicsItem)
 **********************************************************************/
class PothosPlotterCanvas : public QwtPlotCanvas
{
    Q_OBJECT
public:
    PothosPlotterCanvas(QwtPlot *parent):
        QwtPlotCanvas(parent)
    {
        return;
    }
protected:
    void mousePressEvent(QMouseEvent *event)
    {
        QwtPlotCanvas::mousePressEvent(event);
        event->accept();
    }
};

/***********************************************************************
 * Custom QwtPlot implementation
 **********************************************************************/
PothosPlotter::PothosPlotter(QWidget *parent, const int enables):
    QwtPlot(parent),
    _zoomer(nullptr),
    _grid(nullptr)
{
    //setup canvas
    this->setCanvas(new PothosPlotterCanvas(this));
    this->setCanvasBackground(QBrush(Qt::white));

    //font style
    this->setAxisFont(QwtPlot::xBottom, PothosPlotAxisFont());
    this->setAxisFont(QwtPlot::yLeft, PothosPlotAxisFont());
    this->setAxisFont(QwtPlot::yRight, PothosPlotAxisFont());

    //setup optional plot zoomer
    if ((enables & POTHOS_PLOTTER_ZOOM) != 0)
    {
        _zoomer = new PothosPlotPicker(this->canvas());
    }

    //setup optional plot grid
    if ((enables & POTHOS_PLOTTER_GRID) != 0)
    {
        auto plotGrid = new QwtPlotGrid();
        plotGrid->attach(this);
        plotGrid->setPen(QPen(QColor("#999999"), 0.5, Qt::DashLine));
    }

    //connections
    qRegisterMetaType<QList<QwtLegendData>>("QList<QwtLegendData>"); //missing from qwt
    qRegisterMetaType<std::valarray<float>>("std::valarray<float>"); //used for plot data
    connect(this, SIGNAL(itemAttached(QwtPlotItem *, bool)), this, SLOT(handleItemAttached(QwtPlotItem *, bool)));
}

PothosPlotter::~PothosPlotter(void)
{
    delete _zoomer;
    delete _grid;
}

void PothosPlotter::setTitle(const QString &text)
{
    static const QFont font(PothosPlotTitleFont());
    QwtText t(text);
    t.setFont(font);
    QwtPlot::setTitle(t);
}

void PothosPlotter::setAxisTitle(const int id, const QString &text)
{
    static const QFont font(PothosPlotAxisTitleFont());
    QwtText t(text);
    t.setFont(font);
    QwtPlot::setAxisTitle(id, t);
}

void PothosPlotter::updateChecked(QwtPlotItem *item)
{
    auto legend = dynamic_cast<QwtLegend *>(this->legend());
    if (legend == nullptr) return; //no legend
    auto info = legend->legendWidget(this->itemToInfo(item));
    auto label = dynamic_cast<QwtLegendLabel *>(info);
    if (label == nullptr) return; //no label
    label->setChecked(item->isVisible());
    this->updateLegend();
}

void PothosPlotter::handleItemAttached(QwtPlotItem *item, bool on)
{
    //only cuves are supported by this logic
    if (item->rtti() != QwtPlotItem::Rtti_PlotCurve) return;

    const auto items = this->itemList();
    if (on)
    {
        const int i = items.size()-1;

        //apply stashed visibility when the item count matches
        //this handles curves which are added on-demand
        if (_visible.size() > i)
        {
            items[i]->setVisible(_visible.at(i));
            this->updateChecked(items[i]);
        }

        //otherwise expand the cache to match the item size
        else _visible.resize(items.size());
    }

    else
    {
        //store the visibility status when removed
        //so it can be restored when added back
        if (items.size() < _visible.size())
            _visible.setBit(items.size(), item->isVisible());
    }
}

QVariant PothosPlotter::state(void) const
{
    QVariantMap state;

    //zoom stack
    QVariantList stack;
    for (const auto &rect : zoomer()->zoomStack()) stack.append(rect);
    state["stack"] = stack;
    state["index"] = zoomer()->zoomRectIndex();

    //item visibility
    const auto items = this->itemList();
    QBitArray visible(_visible);
    for (int i = 0; i < items.size() and i < _visible.size(); i++)
    {
        visible.setBit(i, items[i]->isVisible());
    }
    state["visible"] = visible;

    return state;
}

void PothosPlotter::setState(const QVariant &state)
{
    const auto map = state.toMap();

    //zoom stack
    QStack<QRectF> stack;
    for (const auto &rect : map["stack"].toList()) stack.push(rect.toRectF());
    zoomer()->setZoomStack(stack, map["index"].toInt());

    //item visibility
    const auto items = this->itemList();
    _visible = map["visible"].toBitArray();
    for (int i = 0; i < items.size() and i < _visible.size(); i++)
    {
        if (items[i]->rtti() != QwtPlotItem::Rtti_PlotCurve) continue;
        items[i]->setVisible(_visible.at(i));
        this->updateChecked(items[i]);
    }
}

#include "PothosPlotter.moc"
