#include <memory>
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
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private:
    QScopedPointer<Ui::AboutDialog> m_ui;

    Q_DISABLE_COPY(AboutDialog)
};
