#include "UMTypedSubscriber.h"
#import "umundo/core.h"


@implementation UMTypedSubscriber

- (id) init {
  NSLog(@"Use UMSubscriber::initWithChannel:andListener");
  return nil;
}

- (id) initWithChannel:(NSString*)name andReceiver:(id<UMTypedSubscriberReceiver>)listener {
  self = [super init];
  if(self) {
    if(name == nil || listener == nil) {
      return nil;
    } else {
      _cListener = new umundoTypedReceiverWrapper(listener);
      std::string cppChannelName([name cStringUsingEncoding: NSASCIIStringEncoding]);
      _cppSub = boost::shared_ptr<umundo::Subscriber>(new umundo::Subscriber(cppChannelName, _cListener));
    }
  }
  return self;
}

- (void) registerType:(NSString*)type withDeserializer:(google::protobuf::MessageLite*)deserializer {
  _cListener->_deserializers[[type cStringUsingEncoding:NSASCIIStringEncoding]] = deserializer;
}

@end

void umundoTypedReceiverWrapper::receive(umundo::Message* msg) {    
  @autoreleasepool {
    google::protobuf::MessageLite* pbObj = _deserializers[msg->getMeta("type")]->New();
    pbObj->ParseFromString(msg->getData());
    assert(pbObj->SerializeAsString().compare(msg->getData()) == 0);
    NSMutableDictionary* meta = [[NSMutableDictionary alloc] init];
    
    std::map<std::string, std::string>::const_iterator metaIter;
    for (metaIter = msg->getMeta().begin(); metaIter != msg->getMeta().end(); metaIter++) {
      NSString* key = [NSString stringWithCString:metaIter->first.c_str() encoding:NSASCIIStringEncoding];
      NSString* value = [NSString stringWithCString:metaIter->second.c_str() encoding:NSASCIIStringEncoding];
      [meta setValue:value forKey:key];
    }
    [_objcTypedReceiver received:pbObj andMeta:meta];
  }
}

