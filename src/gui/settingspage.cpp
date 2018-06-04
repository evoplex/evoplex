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

#include <QStringList>
#include <QThread>

#include "settingspage.h"
#include "ui_settingspage.h"
#include "core/experimentsmgr.h"

namespace evoplex
{

SettingsPage::SettingsPage(MainGUI* mainGUI)
    : QWidget(mainGUI)
    , m_ui(new Ui_SettingsPage)
    , m_mainGUI(mainGUI)
{
    m_ui->setupUi(this);

    connect(m_ui->reset, SIGNAL(pressed()), SLOT(resetDefaults()));

    connect(m_ui->fontSize, SIGNAL(valueChanged(int)), mainGUI, SLOT(setFontSize(int)));

    m_ui->threads->setMinimum(1);
    m_ui->threads->setMaximum(QThread::idealThreadCount());
    connect(m_ui->threads, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [mainGUI](int newValue) { mainGUI->mainApp()->expMgr()->setMaxThreadCount(newValue); });

    m_ui->colormaps->insertItems(0, m_mainGUI->colorMapMgr()->names());
    connect(m_ui->colormaps, SIGNAL(currentIndexChanged(QString)), SLOT(setDfCMapName(QString)));
    connect(m_ui->colormapsize, SIGNAL(currentTextChanged(QString)), SLOT(setDfCMapSize(QString)));

    connect(m_ui->delay, &QSlider::valueChanged, [mainGUI](int v) { mainGUI->mainApp()->setDefaultStepDelay(v); });

    m_ui->stepsToFlush->setMinimum(1);
    m_ui->stepsToFlush->setMaximum(EVOPLEX_MAX_STEPS);
    connect(m_ui->stepsToFlush, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [mainGUI](int newValue) { mainGUI->mainApp()->setStepsToFlush(newValue); });

    refreshFields();
}

SettingsPage::~SettingsPage()
{
    delete m_ui;
}

void SettingsPage::refreshFields()
{
    m_ui->fontSize->setValue(m_mainGUI->fontSize());

    m_ui->threads->setValue(m_mainGUI->mainApp()->expMgr()->maxThreadsCount());

    const CMapKey cmap = m_mainGUI->colorMapMgr()->defaultColorMap();
    m_ui->colormaps->setCurrentText(cmap.first);
    m_ui->colormapsize->setCurrentText(QString::number(cmap.second));

    m_ui->delay->setValue(m_mainGUI->mainApp()->defaultStepDelay());

    m_ui->stepsToFlush->setValue(m_mainGUI->mainApp()->stepsToFlush());
}

void SettingsPage::resetDefaults()
{
    m_mainGUI->mainApp()->expMgr()->resetSettingsToDefault();
    m_mainGUI->colorMapMgr()->resetSettingsToDefault();
    m_mainGUI->mainApp()->resetSettingsToDefault();
    m_mainGUI->resetSettingsToDefault();
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

} // evoplex
