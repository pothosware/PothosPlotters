// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <QString>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <qwt_text.h>

static inline QPen PothosPlotGridPen(void)
{
    return QPen(QBrush("#999999"), 0.5, Qt::DashLine);
}

static inline QBrush PothosPlotCanvasBg(void)
{
    return QBrush(Qt::white);
}

static inline QFont PothosPlotAxisFontSize(void)
{
    QFont f;
    f.setPointSize(7);
    f.setWeight(QFont::Normal);
    f.setStretch(QFont::SemiCondensed);
    return f;
}

static inline QwtText PothosPlotAxisTitle(const QString &title)
{
    QwtText text(title);
    QFont f;
    f.setPointSize(7);
    f.setWeight(QFont::DemiBold);
    f.setStretch(QFont::SemiExpanded);
    text.setFont(f);
    return text;
}

static inline QwtText PothosPlotTitle(const QString &title)
{
    QwtText text(title);
    QFont f;
    f.setPointSize(8);
    f.setWeight(QFont::Bold);
    f.setStretch(QFont::SemiExpanded);
    text.setFont(f);
    return text;
}

static inline QwtText PothosMarkerLabel(const QString &label)
{
    QwtText text(label);
    QFont f;
    f.setPointSize(7);
    f.setWeight(QFont::DemiBold);
    f.setStretch(QFont::SemiExpanded);
    text.setFont(f);
    static const QColor lightGray("#D0D0D0");
    text.setBackgroundBrush(QBrush(lightGray));
    return text;
}
