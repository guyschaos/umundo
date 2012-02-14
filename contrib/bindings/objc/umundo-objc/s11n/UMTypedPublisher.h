#ifndef TYPEDPUBLISHER_H_9RDF6TXT
#define TYPEDPUBLISHER_H_9RDF6TXT

#import <Foundation/Foundation.h>

#include <google/protobuf/message_lite.h>
#import <umundo/core.h>
#import <umundo-objc/core.h>
#import <umundo-objc/s11n.h>

@interface UMTypedPublisher : UMPublisher {
@public
}

- (id) initWithChannel:(NSString*) channelName;
- (void)sendObj:(google::protobuf::MessageLite*)data asType:(NSString*)type;
- (void)registerType:(NSString*)type withSerializer:(google::protobuf::MessageLite*)serializer;

@end

#endif /* end of include guard: TYPEDPUBLISHER_H_9RTI6TXT */
