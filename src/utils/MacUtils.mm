#include <QtGlobal>

#ifdef Q_OS_MAC
#import <AppKit/AppKit.h>

void setMacDockIconVisible(bool visible) {
    if (visible) {
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    } else {
        [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
    }
}
#endif
