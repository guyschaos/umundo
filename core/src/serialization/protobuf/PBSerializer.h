#ifndef PBSERIALIZER_H_2GMB2BW7
#define PBSERIALIZER_H_2GMB2BW7

namespace umundo {

class PBSerializer : public Serializer {
public:
	virtual void serialize(std::string type, void* obj, int length);
	virtual void* unserialize(char* data, int length);
	
};

}
#endif /* end of include guard: PBSERIALIZER_H_2GMB2BW7 */
