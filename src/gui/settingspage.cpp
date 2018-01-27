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
{
    m_ui->setupUi(this);

    ExperimentsMgr* expMgr = mainGUI->mainApp()->getExperimentsMgr();
    m_ui->threads->setValue(expMgr->maxThreadsCount());
    m_ui->threads->setMaximum(QThread::idealThreadCount());
    connect(m_ui->threads, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [expMgr](int newValue) { expMgr->setMaxThreadCount(newValue); });

    QStringList cmaps = mainGUI->colorMapMgr()->names();
    cmaps.sort();
    m_ui->colormaps->insertItems(0, cmaps);
    m_ui->colormaps->setCurrentText(mainGUI->colorMapMgr()->defaultColorMap());
    connect(m_ui->colormaps, &QComboBox::currentTextChanged,
            [mainGUI](QString df) { mainGUI->colorMapMgr()->setDefaultColorMap(df); });
}

SettingsPage::~SettingsPage()
{
    delete m_ui;
}

} // evoplex
