#ifndef SERVICEGENERATORJAVA_H_8QGXJG9I
#define SERVICEGENERATORJAVA_H_8QGXJG9I

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

namespace umundo {

using namespace google::protobuf;
using namespace google::protobuf::compiler;

class ServiceGeneratorJava : public compiler::CodeGenerator {
public:
	ServiceGeneratorJava() {};
	~ServiceGeneratorJava() {};
	bool Generate(const FileDescriptor* file, const string& parameter, GeneratorContext* generator_context, string* error) const;

};

}

#endif /* end of include guard: SERVICEGENERATORJAVA_H_8QGXJG9I */
