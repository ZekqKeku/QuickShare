#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QObject>
#include <QGuiApplication>
#include <QClipboard>

class Clipboard : public QObject
{
    Q_OBJECT
public:
    explicit Clipboard(QObject *parent = nullptr) : QObject(parent) {}
    
    Q_INVOKABLE void setText(const QString &text) {
        QGuiApplication::clipboard()->setText(text);
    }
};

#endif // CLIPBOARD_H
