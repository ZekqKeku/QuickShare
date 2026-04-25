#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H

class SystemUtils {
public:
    static void setAutoStart(bool enabled);
    static void setHideInDock(bool enabled);
    static void registerContextMenu(bool enabled);
    static void applySystemSettings();
};

#ifdef Q_OS_MAC
void setMacDockIconVisible(bool visible);
#endif

#endif
