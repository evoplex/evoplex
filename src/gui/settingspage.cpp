/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QThread>

#include "settingspage.h"
#include "ui_settingspage.h"
#include "core/experimentsmgr.h"

namespace evoplex {

SettingsPage::SettingsPage(MainApp* mainApp, QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui_SettingsPage)
    , m_mainApp(mainApp)
{
    m_ui->setupUi(this);

    m_ui->threads->setValue(m_mainApp->getExperimentsMgr()->maxThreadsCount());
    m_ui->threads->setMaximum(QThread::idealThreadCount());

    connect(m_ui->threads, SIGNAL(valueChanged(int)), this, SLOT(slotNumThreads(int)));
}

SettingsPage::~SettingsPage()
{
    delete m_ui;
    m_ui = nullptr;
}

void SettingsPage::slotNumThreads(int newValue)
{
    m_mainApp->getExperimentsMgr()->setMaxThreadCount(newValue);
}
}
