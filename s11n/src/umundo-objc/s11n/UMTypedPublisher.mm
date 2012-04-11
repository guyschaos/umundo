#include "UMTypedPublisher.h"

@implementation UMTypedPublisher

- (id) init {
  NSLog(@"Use UMPublisher::initWithChannel");
  return nil;
}

- (id) initWithChannel:(NSString*) channelName {
  self = [super initWithChannel:channelName];
  return self;
}

- (void)sendObj:(google::protobuf::MessageLite*)data asType:(NSString*)type {
  std::string buffer = data->SerializeAsString();
  umundo::Message* msg = new umundo::Message(buffer.data(), buffer.size());
  msg->setMeta("type", std::string([type cStringUsingEncoding: NSASCIIStringEncoding]));
  _cppPub->send(msg);
}

- (void)registerType:(NSString*)type withSerializer:(google::protobuf::MessageLite*)serializer {
  // nothing needed here for protobuf
}

@end