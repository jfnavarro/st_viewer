/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "SelectionDialog.h"
#include "ui_selectiondialog.h"

SelectionDialog::SelectionDialog(QPointer<DataProxy> dataProxy,
                                 QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    m_ui(nullptr),
    m_includeAmbiguous(false),
    m_caseSensitive(false),
    m_regExpValid(false),
    m_dataProxy(dataProxy)
{
    Q_ASSERT(!m_dataProxy.isNull());

    m_ui = new Ui::SelectionDialog();
    m_ui->setupUi(this);

    // set default state
    slotCaseSensitive(false);
    slotIncludeAmbiguous(false);
}

SelectionDialog::~SelectionDialog()
{
    if (m_ui != nullptr) {
        delete m_ui;
    }
    m_ui = nullptr;
}

const SelectionDialog::GeneList& SelectionDialog::selectedGenes() const
{
    return m_selectedGeneList;
}

const SelectionDialog::GeneList SelectionDialog::selectGenes(QPointer<DataProxy> dataProxy,
                                                             QWidget *parent)
{
    SelectionDialog dialog(dataProxy, parent);
    if (dialog.exec() == QDialog::Accepted) {
        return dialog.selectedGenes();
    }
    return SelectionDialog::GeneList();
}

void SelectionDialog::accept()
{
    // early out, should "never" happen
    if (!m_regExp.isValid()) {
        reject();
        return;
    }

    // get the current list of genes
    const auto& geneList = m_dataProxy->getGeneList(m_dataProxy->getSelectedDataset());
    
    // find all genes that match the regular expression
    m_selectedGeneList.clear();
    foreach(DataProxy::GenePtr gene, geneList) {
        const QString name = gene->name();
        if (!m_includeAmbiguous && gene->isAmbiguous()) {
            continue;
        }
        if (name.contains(m_regExp)) {
            m_selectedGeneList.append(gene);
        }
    }

    // and propagate accept call
    QDialog::accept();
}

void SelectionDialog::slotValidateRegExp(const QString &pattern)
{
    m_regExp.setPattern(pattern);
    const bool regExpValid = m_regExp.isValid();
    if (regExpValid != m_regExpValid) {
        m_regExpValid = regExpValid;
        emit signalValidRegExp(m_regExpValid);
    }
}

void SelectionDialog::slotIncludeAmbiguous(bool includeAmbiguous)
{
    m_includeAmbiguous = includeAmbiguous;
    if (m_includeAmbiguous != m_ui->checkAmbiguous->isChecked()) {
        m_ui->checkAmbiguous->setChecked(m_includeAmbiguous);
    }
}

void SelectionDialog::slotCaseSensitive(bool caseSensitive)
{
    // toggle case sensitive
    m_regExp.setCaseSensitivity((caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive));
    m_caseSensitive = caseSensitive;
    if (m_caseSensitive != m_ui->checkCaseSense->isChecked()) {
        m_ui->checkCaseSense->setChecked(m_caseSensitive);
    }
}

void SelectionDialog::slotEnableAcceptAction(bool enableAcceptAction)
{
    // disable any "accept/yes/apply" type buttons when regexp is invalid
    foreach(QAbstractButton *button, m_ui->buttonBox->buttons()) {
        const QDialogButtonBox::ButtonRole role = m_ui->buttonBox->buttonRole(button);
        if (role == QDialogButtonBox::AcceptRole ||
            role == QDialogButtonBox::YesRole ||
            role == QDialogButtonBox::ApplyRole) {
            button->setEnabled(enableAcceptAction);
        }
    }
}
