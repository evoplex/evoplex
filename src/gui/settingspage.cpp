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

    m_ui->threads->setMinimum(1);
    m_ui->threads->setMaximum(QThread::idealThreadCount());
    m_ui->threads->setValue(mainGUI->mainApp()->expMgr()->maxThreadsCount());
    connect(m_ui->threads, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [mainGUI](int newValue) { mainGUI->mainApp()->expMgr()->setMaxThreadCount(newValue); });

    const CMapKey cmap = mainGUI->colorMapMgr()->defaultColorMap();
    m_ui->colormaps->insertItems(0, mainGUI->colorMapMgr()->names());
    m_ui->colormaps->setCurrentText(cmap.first);
    setDfCMapName(m_ui->colormaps->currentText()); // make sure we fill the cbox of sizes
    connect(m_ui->colormaps, SIGNAL(currentIndexChanged(QString)), SLOT(setDfCMapName(QString)));

    m_ui->colormapsize->setCurrentText(QString::number(cmap.second));
    connect(m_ui->colormapsize, SIGNAL(currentTextChanged(QString)), SLOT(setDfCMapSize(QString)));

    m_ui->delay->setValue(mainGUI->mainApp()->defaultStepDelay());
    connect(m_ui->delay, &QSlider::valueChanged, [mainGUI](int v) { mainGUI->mainApp()->setDefaultStepDelay(v); });

    m_ui->stepsToFlush->setMinimum(1);
    m_ui->stepsToFlush->setMaximum(EVOPLEX_MAX_STEPS);
    m_ui->stepsToFlush->setValue(mainGUI->mainApp()->stepsToFlush());
    connect(m_ui->stepsToFlush, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [mainGUI](int newValue) { mainGUI->mainApp()->setStepsToFlush(newValue); });
}

SettingsPage::~SettingsPage()
{
    delete m_ui;
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
