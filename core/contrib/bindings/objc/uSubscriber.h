//
//  uSubscriber.h
//  uMundoIOS
//
//  Created by Stefan Radomski on 14/1/12.
//  Copyright (c) 2012 TU Darmstadt. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <umundo.h>

@protocol uSubscriberListener
- (void)received:(NSData*)data;
@end

@interface uSubscriber : NSObject {
  boost::shared_ptr<umundo::Subscriber> _cppSub;
  id<uSubscriberListener> _listener;
}
- (id) initWithChannel:(NSString*)name andListener:(id<uSubscriberListener>)listener;
@end
