/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SELECTIONDIALOG_H
#define SELECTIONDIALOG_H

#include <QDialog>
#include <QList>

#include "data/DataProxy.h"

namespace Ui{
class SelectionDialog;
} // namespace Ui //

// Selection dialog implementing support to select genes by their names
// using regular expressions.
class SelectionDialog : public QDialog
{
    Q_OBJECT

public:
    
    typedef DataProxy::GeneList GeneList;

    explicit SelectionDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
    virtual ~SelectionDialog();

    const GeneList& selectedGenes() const;
    static const GeneList selectGenes(QWidget *parent = 0);

signals:
    
    void signalValidRegExp(bool validRegExp);

public slots:
    
    void accept() override;

    void slotValidateRegExp(const QString &pattern);
    void slotIncludeAmbiguous(bool includeAmbiguous);
    void slotCaseSensitive(bool caseSensitive);
    void slotEnableAcceptAction(bool enableAcceptAction);

private:
    
    Ui::SelectionDialog *ui;
    bool m_includeAmbiguous;
    bool m_caseSensitive;
    bool m_regExpValid;
    QRegExp m_regExp;
    GeneList m_selectedGeneList;

    Q_DISABLE_COPY(SelectionDialog)
};

#endif // SELECTIONDIALOG_H //
