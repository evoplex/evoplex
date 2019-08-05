/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QDebug>
#include <QTranslator>
#include <QMessageBox>
#include <QStringList>
#include <QThread>

#include "core/experimentsmgr.h"
#include "core/include/constants.h"

#include "settingspage.h"
#include "ui_settingspage.h"
#include "fontstyles.h"

namespace evoplex
{

SettingsPage::SettingsPage(MainGUI* mainGUI)
    : QWidget(mainGUI)
    , m_ui(new Ui_SettingsPage)
    , m_mainGUI(mainGUI)
{
    m_ui->setupUi(this);

    m_ui->labelSettings->setFont(FontStyles::subtitle1());

    connect(m_ui->reset, SIGNAL(pressed()), SLOT(resetDefaults()));

    m_ui->threads->setMinimum(1);
    m_ui->threads->setMaximum(QThread::idealThreadCount());
    connect(m_ui->threads, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    [this, mainGUI](int newValue) {
        QString error;
        mainGUI->mainApp()->expMgr()->setMaxThreadCount(newValue, &error);
        if (!error.isEmpty()) {
            QMessageBox::warning(this, "Evoplex", error);
            m_ui->threads->blockSignals(true);
            m_ui->threads->setValue(m_mainGUI->mainApp()->expMgr()->maxThreadsCount());
            m_ui->threads->blockSignals(false);
        }
    });

    m_ui->colormaps->insertItems(0, m_mainGUI->colorMapMgr()->names());
    connect(m_ui->colormaps, SIGNAL(currentIndexChanged(QString)), SLOT(setDfCMapName(QString)));
    connect(m_ui->colormapsize, SIGNAL(currentTextChanged(QString)), SLOT(setDfCMapSize(QString)));

    connect(m_ui->delay, &QSlider::valueChanged, [mainGUI](int v) {
        mainGUI->mainApp()->setDefaultStepDelay(static_cast<quint16>(v));
    });

    m_ui->stepsToFlush->setMinimum(1);
    m_ui->stepsToFlush->setMaximum(EVOPLEX_MAX_STEPS);
    connect(m_ui->stepsToFlush, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        [mainGUI](int v) { mainGUI->mainApp()->setStepsToFlush(v); });

    connect(m_ui->checkUpdates, &QCheckBox::toggled, [mainGUI](bool b) {
        mainGUI->mainApp()->setCheckUpdatesAtStart(b);
    });

    connect(m_ui->imageQuality, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        [](int v) { QSettings s; s.setValue("settings/imgQuality", v); });

    connect(m_ui->changeToGerman, SIGNAL(clicked()), SLOT(slotChangeToGerman()));

    refreshFields();
}

SettingsPage::~SettingsPage()
{
    delete m_ui;
}

void SettingsPage::refreshFields()
{
    m_ui->threads->setValue(m_mainGUI->mainApp()->expMgr()->maxThreadsCount());

    const CMapKey cmap = m_mainGUI->colorMapMgr()->defaultCMapKey();
    m_ui->colormaps->setCurrentText(cmap.first);
    m_ui->colormapsize->setCurrentText(QString::number(cmap.second));

    m_ui->delay->setValue(m_mainGUI->mainApp()->defaultStepDelay());

    m_ui->stepsToFlush->setValue(m_mainGUI->mainApp()->stepsToFlush());

    m_ui->checkUpdates->setChecked(m_mainGUI->mainApp()->checkUpdatesAtStart());

    QSettings s;
    m_ui->imageQuality->setValue(s.value("settings/imgQuality", 90).toInt());
}

void SettingsPage::resetDefaults()
{
    m_mainGUI->mainApp()->expMgr()->resetSettingsToDefault();
    m_mainGUI->colorMapMgr()->resetSettingsToDefault();
    m_mainGUI->mainApp()->resetSettingsToDefault();
    QSettings s;
    s.setValue("settings/imgQuality", 90);
    refreshFields();
}

void SettingsPage::setDfCMapName(const QString& name)
{
    m_ui->colormapsize->clear();
    m_ui->colormapsize->insertItems(0, m_mainGUI->colorMapMgr()->sizes(name));
    m_mainGUI->colorMapMgr()->setDefaultColorMap(name, m_ui->colormapsize->currentText().toInt());
}

void SettingsPage::setDfCMapSize(const QString& sz)
{
    m_mainGUI->colorMapMgr()->setDefaultColorMap(m_ui->colormaps->currentText(), sz.toInt());
}

void SettingsPage::slotChangeToGerman()
{
    QTranslator translator;
    qDebug() << "Translator:" << translator.load(":/lan/lan/settings_de.qm");
    qDebug() << "Install translator:" <<  qApp->installTranslator(&translator);
    m_ui->retranslateUi(this);
}

} // evoplex
