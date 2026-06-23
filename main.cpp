#include "guiapp.h"
#include "mainwindow.h"

#include <QApplication>
#include <QIcon>
#include <QMessageBox>
#include <sodium.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (sodium_init() < 0)
    {
        QMessageBox::critical(
            nullptr,
            "Initialization Error",
            "Failed to initialize libsodium."
            );

        return 1;
    }

    MainWindow window;
    GUIApp guiApp(window);

    if (!guiApp.initialize())
    {
        return 1;
    }

    if (!guiApp.hasActiveSession())
    {
        window.show();
    }

    return app.exec();
}