#include <QtGlobal>

#ifdef Q_OS_MAC
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

void setMacDockIconVisible(bool visible) {
    dispatch_async(dispatch_get_main_queue(), ^{
        NSApplicationActivationPolicy policy = visible ? NSApplicationActivationPolicyRegular : NSApplicationActivationPolicyAccessory;
        if ([NSApp activationPolicy] != policy) {
            [NSApp setActivationPolicy:policy];
            
            if (visible) {
                [NSApp activateIgnoringOtherApps:YES];
            }
        }
    });
}
#endif
