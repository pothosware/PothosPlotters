// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Plugin.hpp>
#include <Poco/JSON/Object.h>
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
        connect(this, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(handleWidgetChanged(const QString &)));
        connect(this, SIGNAL(editTextChanged(const QString &)), this, SLOT(handleEntryChanged(const QString &)));
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

private slots:
    void handleWidgetChanged(const QString &)
    {
        emit this->widgetChanged();
    }
    void handleEntryChanged(const QString &)
    {
        emit this->entryChanged();
    }
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
static QWidget *makeColorMapEntry(const Poco::JSON::Object::Ptr &, QWidget *parent)
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
    Pothos::PluginRegistry::add("/gui/EntryWidgets/ColorMapEntry", Pothos::Callable(&makeColorMapEntry));
}

#include "ColorMapEntry.moc"
