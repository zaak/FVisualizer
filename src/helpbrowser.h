#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QTextEdit>
#include <QUrl>

class HelpBrowser : public QTextEdit
{
    Q_OBJECT

public:
    HelpBrowser(QWidget *parent = 0);
    void setContents(const QString &fileName);

private:
    QVariant loadResource(int type, const QUrl &name);
    QUrl srcUrl;
};

#endif // HELPBROWSER_H
