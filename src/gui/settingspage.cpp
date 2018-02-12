/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
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
