/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDialog>

namespace Ui
{
class AboutDialog;
} // namespace Ui //

// Simple about dialog. Contains the application logo as well as some about
// text (so... simple).
class AboutDialog : public QDialog
{
    Q_OBJECT

public:

    explicit AboutDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~AboutDialog();

private:

    Ui::AboutDialog *m_ui;

    Q_DISABLE_COPY(AboutDialog)
};
