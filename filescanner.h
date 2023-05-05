#ifndef FILESCANNER_H
#define FILESCANNER_H
#include <QString>

class fileScanner
{
public:
    fileScanner();

    QString getQssString(QString& s); ///< 从文件名得到样式表字符串
};

#endif // FILESCANNER_H
