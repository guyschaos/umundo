#ifndef PBDESERIALIZER_H_DD3C36Z7
#define PBDESERIALIZER_H_DD3C36Z7

#include "umundo/s11n/TypedSubscriber.h"
#include <google/protobuf/message_lite.h>

using google::protobuf::MessageLite;

namespace umundo {
	class PBDeserializer : public TypeDeserializerImpl {
	public:
		PBDeserializer() {}
		virtual ~PBDeserializer() {}
		
		void destroy();
		void init(shared_ptr<Configuration>);
		shared_ptr<Implementation> create();
		void* deserialize(const string& type, const string& data);
//		void* deserialize(const std::string& data);

		void registerType(const std::string&, void*);

	protected:
		map<string, MessageLite*> _deserializers;
				
	};
}

#endif /* end of include guard: PBDESERIALIZER_H_DD3C36Z7 */
