#include <QApplication>
#include <QSurfaceFormat>
#include <QIcon>

#include "gui/MainWindow.hpp"
#include "controller/GameController.hpp"

int main(int argc, char *argv[])
{
    // Инициализация Qt-приложения
    QApplication app(argc, argv);

    // Базовая информация о приложении
    QCoreApplication::setOrganizationName("YourOrgName");
    QCoreApplication::setOrganizationDomain("example.com");
    QCoreApplication::setApplicationName("Omega Chess");
    QCoreApplication::setApplicationVersion("0.1.0");

    // (Опционально) Настройка формата поверхности, если планируется OpenGL/ускоренный рендер
    // QSurfaceFormat fmt;
    // fmt.setDepthBufferSize(24);
    // fmt.setStencilBufferSize(8);
    // fmt.setSamples(4); // MSAA, если нужно сглаживание
    // QSurfaceFormat::setDefaultFormat(fmt);

    // (Опционально) Загрузка иконки приложения
    // QIcon appIcon(":/icons/omega_chess.png");
    // QApplication::setWindowIcon(appIcon);

    // Создаём контроллер игры (логика, модель)
    GameController controller;

    // Создаём главное окно и передаём контроллер внутрь
    MainWindow mainWindow(&controller);
    mainWindow.setWindowTitle(QObject::tr("Omega-шахматы"));
    mainWindow.show();

    // Запуск основного цикла приложения
    return app.exec();
}
