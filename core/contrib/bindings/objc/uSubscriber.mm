//
//  uSubscriber.m
//  uMundoIOS
//
//  Created by Stefan Radomski on 14/1/12.
//  Copyright (c) 2012 TU Darmstadt. All rights reserved.
//

#import "uSubscriber.h"

class umundoListenerWrapper : public umundo::Receiver {
public:
  umundoListenerWrapper(id<uSubscriberListener> listener) : _objcListener(listener) {}
  id<uSubscriberListener> _objcListener;
  void receive(char* data, int length) {
    NSData* nsData = [[NSData alloc] initWithBytes:data length:length];
    [_objcListener received:nsData];
    return 0;
  }
};

@implementation uSubscriber

- (id) init {
  NSLog(@"Use uSubscriber::initWithChannel:andListener");
  return nil;
}

- (id) initWithChannel:(NSString*)name andListener:(id<uSubscriberListener>)listener {
  self = [super init];
  if(self) {
    if(name == nil || listener == nil) {
      return nil;
    } else {
      boost::shared_ptr<umundo::Receiver> cListener = 
        boost::shared_ptr<umundo::Receiver>(new umundoListenerWrapper(listener));
      std::string cppChannelName([name cStringUsingEncoding: NSASCIIStringEncoding]);
      _cppSub = 
        boost::shared_ptr<umundo::Subscriber>(new umundo::Subscriber(cppChannelName, cListener));
    }
  }
  return self;
}
@end
