#include <QApplication>
#include <QLabel>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QLabel label("Hello Qt - environment works");
    label.resize(320, 120);
    label.setWindowTitle("Hello Qt");
    label.show();
    return app.exec();
}
