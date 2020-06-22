#include <kde/kmainwindow.h>
class KDEMenu : public KMainWindow
{
    Q_OBJECT
public:
    KDEMenu(const char *name = 0);
private slots:
    void newFile();
    void aboutApp();
};