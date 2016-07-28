// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <QString>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <qwt_text.h>

static inline QFont PothosMarkerLabelFont(void)
{
    QFont f;
    f.setPointSize(7);
    f.setWeight(QFont::DemiBold);
    f.setStretch(QFont::SemiExpanded);
    return f;
}

static inline QwtText PothosMarkerLabel(const QString &label)
{
    QwtText text(label);
    static const QFont font(PothosMarkerLabelFont());
    text.setFont(font);
    static const QColor lightGray("#D0D0D0");
    text.setBackgroundBrush(QBrush(lightGray));
    return text;
}
