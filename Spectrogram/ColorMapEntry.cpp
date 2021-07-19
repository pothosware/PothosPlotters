// Copyright (c) 2016-2021 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Plugin.hpp>
#include <QJsonObject>
#include <QJsonArray>
#include <QComboBox>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QAbstractItemView>
#include <qwt_color_map.h>
#include "GeneratedColorMaps.hpp"

/***********************************************************************
 * ComboBox for drop-down entry
 **********************************************************************/
class ColorMapEntry : public QComboBox
{
    Q_OBJECT
public:
    ColorMapEntry(QWidget *parent):
        QComboBox(parent)
    {
        connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int){emit this->widgetChanged();});
        connect(this, &QComboBox::editTextChanged, [=](const QString &){emit this->entryChanged();});
        this->view()->setObjectName("BlockPropertiesEditWidget"); //to pick up eval color style
    }

public slots:
    QString value(void) const
    {
        const auto index = QComboBox::currentIndex();
        if (index < 0 or QComboBox::currentText() != QComboBox::itemText(index)) return QComboBox::currentText();
        else return QComboBox::itemData(index).toString();
    }

    void setValue(const QString &value)
    {
        int index = -1;
        for (int i = 0; i < QComboBox::count(); i++)
        {
            if (QComboBox::itemData(i).toString() == value) index = i;
        }
        if (index < 0) QComboBox::setEditText(value);
        else QComboBox::setCurrentIndex(index);
    }

signals:
    void commitRequested(void);
    void widgetChanged(void);
    void entryChanged(void);
};

/***********************************************************************
 * Helpful icons to preview color map
 **********************************************************************/
static QIcon makeColorMapIcon(const std::string &name)
{
    std::unique_ptr<QwtColorMap> colorMap(makeQwtColorMap(name));
    QPixmap pixmap(100, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::transparent);
    const QwtInterval interval(0.0, double(pixmap.width()));
    for (int i = 0; i < pixmap.width(); i++)
    {
        painter.setBrush(QColor(colorMap->rgb(interval, double(i))));
        painter.drawRect(QRectF(qreal(i), qreal(0), qreal(1), pixmap.height()));
    }
    return QIcon(pixmap);
}

/***********************************************************************
 * Factory function and registration
 **********************************************************************/
static QWidget *makeColorMapEntry(const QJsonArray &, const QJsonObject &, QWidget *parent)
{
    auto colorMapEntry =  new ColorMapEntry(parent);
    colorMapEntry->setIconSize(QSize(100, 20));
    for (const auto &pair : availableColorMaps())
    {
        colorMapEntry->addItem(
            makeColorMapIcon(pair.second),
            QString::fromStdString(pair.first),
            QString("\"%1\"").arg(QString::fromStdString(pair.second)));
    }
    return colorMapEntry;
}

pothos_static_block(registerColorMapEntry)
{
    Pothos::PluginRegistry::add("/flow/EntryWidgets/ColorMapEntry", Pothos::Callable(&makeColorMapEntry));
}

#include "ColorMapEntry.moc"
