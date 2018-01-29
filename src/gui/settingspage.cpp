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

    ExperimentsMgr* expMgr = mainGUI->mainApp()->expMgr();
    m_ui->threads->setValue(expMgr->maxThreadsCount());
    m_ui->threads->setMaximum(QThread::idealThreadCount());
    connect(m_ui->threads, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [expMgr](int newValue) { expMgr->setMaxThreadCount(newValue); });

    m_ui->colormaps->insertItems(0, mainGUI->colorMapMgr()->names());
    m_ui->colormaps->setCurrentText(mainGUI->colorMapMgr()->defaultColorMap().first);
    setDfCMapName(m_ui->colormaps->currentText()); // fill sizes
    setDfCMapSize(QString::number(mainGUI->colorMapMgr()->defaultColorMap().second));
    connect(m_ui->colormaps, SIGNAL(currentIndexChanged(QString)), SLOT(setDfCMapName(QString)));
    connect(m_ui->colormapsize, SIGNAL(currentTextChanged(QString)), SLOT(setDfCMapSize(QString)));
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
