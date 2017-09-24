/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QThread>

#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "core/experimentsmgr.h"

namespace evoplex {

SettingsWidget::SettingsWidget(MainApp* mainApp, QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui_SettingsWidget)
    , m_mainApp(mainApp)
{
    m_ui->setupUi(this);

    m_ui->threads->setValue(m_mainApp->getExperimentsMgr()->maxThreadsCount());
    m_ui->threads->setMaximum(QThread::idealThreadCount());

    connect(m_ui->threads, SIGNAL(valueChanged(int)), this, SLOT(slotNumThreads(int)));
}

SettingsWidget::~SettingsWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

void SettingsWidget::slotNumThreads(int newValue)
{
    m_mainApp->getExperimentsMgr()->setMaxThreadCount(newValue);
}
}
