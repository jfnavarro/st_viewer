/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ScatterPlot.h"

#include <QPushButton>

#include "ui_ddaWidget.h"

ScatterPlot::ScatterPlot(int size, QWidget *parent) :
    QWidget(parent),
    m_ui(nullptr),
    m_customPlot(nullptr)
{

    // create UI
    m_ui = new Ui::ddaWidget;
    m_ui->setupUi(this);

    m_ui->tableWidget->setRowCount(size);
    m_ui->tableWidget->setColumnCount(3);
    QStringList headers;
    headers << "Gene" << "Exp. Selection A" << "Exp.s Selection B";
    m_ui->tableWidget->setHorizontalHeaderLabels(headers);

    // creating plotting object
    m_customPlot = new QCustomPlot(m_ui->plotWidget);

    //make connections
    connect(m_ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    //TODO handle the saving with a slot that launches a file chooser
    connect(m_ui->saveButton, &QPushButton::clicked,
            [=]() { m_customPlot->savePdf("scatter_plot"); });

}

ScatterPlot::~ScatterPlot()
{
    if (m_ui != nullptr) {
        delete m_ui;
    }
    m_ui = nullptr;

    m_customPlot->deleteLater();
    m_customPlot = nullptr;
}

void ScatterPlot::setNumberGenes(const QString& textA, const QString& textB)
{
    m_ui->numGenesSelectionA->setText(textA);
    m_ui->numGenesSelectionB->setText(textB);
}

void ScatterPlot::setMean(const QString& textA, const QString& textB)
{
    m_ui->meanSelectionA->setText(textA);
    m_ui->meanSelectionB->setText(textB);
}

void ScatterPlot::setStdDev(const QString& textA, const QString& textB)
{
    m_ui->stdDevSelectionA->setText(textA);
    m_ui->stdDevSelectionB->setText(textB);
}

void ScatterPlot::setCorrelation(const QString& text)
{
    m_ui->correlation->setText(text);
}

void ScatterPlot::setOverlapping(const QString& both, const QString& onlyA, const QString& onlyB)
{
    m_ui->overlappingGenes->setText(both);
    m_ui->genesOnlyA->setText(onlyA);
    m_ui->genesOnlyB->setText(onlyB);
}

void ScatterPlot::setSelection(const QString& textA, const QString& textB)
{
    m_ui->selectionA->setText(textA);
    m_ui->selectionB->setText(textB);
}

QTableWidget* ScatterPlot::getTable() const
{
    return m_ui->tableWidget;
}

