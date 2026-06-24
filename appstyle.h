#ifndef APPSTYLE_H
#define APPSTYLE_H

#include <QString>

class AppStyle final
{
public:
    static QString globalStyleSheet();

private:
    AppStyle() = delete;
};

#endif // APPSTYLE_H
