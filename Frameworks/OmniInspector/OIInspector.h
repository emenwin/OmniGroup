// Copyright 2006-2008, 2010, 2013-2014 Omni Development, Inc. All rights reserved.
//
// This software may only be used and reproduced according to the
// terms in the file OmniSourceLicense.html, which should be
// distributed with this project and can also be found at
// <http://www.omnigroup.com/developer/sourcecode/sourcelicense/>.
//
// $Id$

#import <AppKit/NSViewController.h>

@class NSBundle, NSDictionary, NSPredicate; // Foundation
@class NSImage, NSMenuItem, NSView; // AppKit
@class OFEnumNameTable; // OmniFoundation
@class OIInspectorRegistry;

typedef enum {
    OIHiddenVisibilityState,
    OIVisibleVisibilityState,
    OIPinnedVisibilityState
} OIVisibilityState;

typedef NS_ENUM(NSUInteger, OIInspectorInterfaceType) {
    OIInspectorInterfaceTypeFloating = 0, // default, window-based floating inspectors
    OIInspectorInterfaceTypeEmbedded, // no windows; suitable for sticking in e.g. a sidebar
};

// This uses rendering code that is only available on 10.5, but this whole framework is 10.5+ now
#define OITabbedInspectorUnifiedLookDefaultsKey (@"OITabbedInspectorUnifiedLook")

@interface OIInspector : NSViewController

+ (OFEnumNameTable *)visibilityStateNameTable;

+ (instancetype)newInspectorWithDictionary:(NSDictionary *)dict inspectorRegistry:(OIInspectorRegistry *)inspectorRegistry bundle:(NSBundle *)sourceBundle;

- (id)initWithDictionary:(NSDictionary *)dict inspectorRegistry:(OIInspectorRegistry *)inspectorRegistry bundle:(NSBundle *)sourceBundle;

- (NSString *)identifier;
- (OIVisibilityState)defaultVisibilityState;
- (NSString *)shortcutKey;
- (NSUInteger)shortcutModifierFlags;
- (NSImage *)imageNamed:(NSString *)imageName; // Finds the image in the app wrapper (if allowImagesFromApplication is true) or resourceBundle
- (NSImage *)image;
- (NSImage *)tabImage;
- (NSBundle *)resourceBundle;
- (BOOL)allowImagesFromApplication;
- (NSString *)inspectorImageName;
- (NSString *)inspectorTabImageName;

- (NSString *)displayName;
- (CGFloat)additionalHeaderHeight;

- (NSUInteger)defaultOrderingWithinGroup;
- (void)setDefaultOrderingWithinGroup:(NSUInteger)defaultOrderingWithinGroup;

// TODO: Get rid of this
- (unsigned int)deprecatedDefaultDisplayGroupNumber;

- (NSMenuItem *)menuItemForTarget:(id)target action:(SEL)action;
- (NSArray *)menuItemsForTarget:(id)target action:(SEL)action;

- (void)setControlsEnabled:(BOOL)enabled;
- (void)setControlsEnabled:(BOOL)enabled inView:(NSView *)view;

- (BOOL)shouldBeUsedForObject:(id)object;
// This method is called by OITabbedInspector whenever the selection changes if the inspector is in auto-tab-select mode
- (NSPredicate *)shouldBeUsedForObjectPredicate;

// Subclasses should override this if they may need to do something in response to an inspector view programmatically resizing. They should also override this to pass it on to any inspectors they manage. (See OITabbedInspector and OISectionedInspector.) Inspectors which programmatically change the size of their inspectorView should then call this method on their inspectorController so it can notify the inspector chain. This allows an inspector view which contains a child inspector view to know to resize to accommodate changes in the size of that child.
- (void)inspectorDidResize:(OIInspector *)resizedInspector;

/// The interface type that this inspector would prefer the app to use. (Inspectors should support all interface types, but can have a weak preference for a certain type if that type suits its intended use better.)
- (OIInspectorInterfaceType)preferredInterfaceType;

@end

@protocol OIConcreteInspector
- (NSPredicate *)inspectedObjectsPredicate;
    // Return a predicate to filter the inspected objects down to what this inspector wants sent to its -inspectObjects: method

- (void)inspectObjects:(NSArray *)objects;
    // This method is called whenever the selection changes
@end

@class OITabbedInspector;
@interface NSObject (OITabbedInspectorOptional)
// Tabbed inspectors receive an -inspectObjects: with nil when they are hidden.  If the inspector wants its tab dimmed when there is a non-empty array that *would* be inspected if it were visible, then this method allows it to do that.  The default is for tabs to be dimmed when there are zero objects they could inspect, if visible.
- (BOOL)shouldBeDimmedForObjects:(NSArray *)objects;
- (BOOL)shouldBeDimmed;

- (void)setContainingTabbedInspector:(OITabbedInspector *)containingTabbedInspector;
@end

// This is not implemented; this just allows you to call the concrete methods.  -[OIInspector initWithDictionary:] asserts that the class conforms.
@interface OIInspector (OIConcreteInspector) <OIConcreteInspector>
@end

@class OIInspectorController;
@interface NSObject (OIInspectorOptionalMethods)
- (void)setInspectorController:(OIInspectorController *)aController;
    // If the inspector has any need to know its controller, it can implement this method
- (CGFloat)inspectorWillResizeToHeight:(CGFloat)height; // height of window content rect, excluding header button view
- (CGFloat)inspectorMinimumHeight; // returns minimum height of window content rect
- (id)windowTitle; 
    // If implemented, this will be used instead of -inspectorName, to let the window title be dynamic. NSAttributedString or NSString are ok.

- (NSDictionary *)configuration;
- (void)loadConfiguration:(NSDictionary *)dict;
    // These methods will be called to save and load any configuration information for the inspectors themselves on startup/shutdown and when workspaces are switched

- (BOOL)mayInspectObject:anObject;

@end

