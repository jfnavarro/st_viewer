/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SELECTIONDIALOG_H
#define SELECTIONDIALOG_H

#include <QDialog>

#include <QSharedPointer>
#include <QList>

#include <controller/data/DataProxy.h>

namespace Ui
{
    class SelectionDialog;
} // namespace Ui //

// Selection dialog implementing support to select genes by their names
// using regular expressions.
class SelectionDialog : public QDialog
{
    Q_OBJECT

public:
    typedef DataProxy::GeneList GeneList;

    SelectionDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
    virtual ~SelectionDialog();

    void setDatasetId(const QString &datasetId);
    const QString &datasetId() const;

    const GeneList selectedGenes() const;

    static const GeneList selectGenes(const QString &datasetId, QWidget *parent = 0);

signals:
    void signalValidRegExp(bool validRegExp);

public slots:
    virtual void accept();

    void slotValidateRegExp(const QString &pattern);
    void slotIncludeAmbiguous(bool includeAmbiguous);
    void slotCaseSensitive(bool caseSensitive);
    void slotEnableAcceptAction(bool enableAcceptAction);

private:
    Ui::SelectionDialog *ui;

    QString m_datasetId;

    bool m_includeAmbiguous;
    bool m_caseSensitive;
    bool m_regExpValid;

    QRegExp m_regExp;

    GeneList m_selectedGeneList;
};

#endif // SELECTIONDIALOG_H //
