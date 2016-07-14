// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Plugin.hpp>
#include <Poco/JSON/Object.h>
#include <QComboBox>
#include <QAbstractItemView>
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
 * Factory function and registration
 **********************************************************************/
static QWidget *makeColorMapEntry(const Poco::JSON::Object::Ptr &, QWidget *parent)
{
    auto colorMapEntry =  new ColorMapEntry(parent);
    for (size_t i = 0; i < NUM_COLOR_MAPS; i++)
    {
        colorMapEntry->addItem(COLOR_MAP_TITLES[i], QString("\"%1\"").arg(COLOR_MAP_NAMES[i]));
    }
    return colorMapEntry;
}

pothos_static_block(registerComboBox)
{
    Pothos::PluginRegistry::add("/gui/EntryWidgets/ColorMapEntry", Pothos::Callable(&makeColorMapEntry));
}

#include "ColorMapEntry.moc"
