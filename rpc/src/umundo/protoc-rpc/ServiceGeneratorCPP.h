#ifndef SERVICEGENERATORCPP_H_EBHB8ZPV
#define SERVICEGENERATORCPP_H_EBHB8ZPV

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

namespace umundo {

using namespace google::protobuf;
using namespace google::protobuf::compiler;

class ServiceGeneratorCPP : public compiler::CodeGenerator {
public:
	ServiceGeneratorCPP() {};
	~ServiceGeneratorCPP() {};
	bool Generate(const FileDescriptor* file, const string& parameter, GeneratorContext* generator_context, string* error) const;

	void writeServiceHeader(io::Printer& printer, const ServiceDescriptor* svcDesc) const;
	void writeServiceStubHeader(io::Printer& printer, const ServiceDescriptor* svcDesc) const;
	void writeServiceStubImpl(io::Printer& printer, const ServiceDescriptor* svcDesc) const;
	void writeServiceImplConstructor(io::Printer& printer, const ServiceDescriptor* svcDesc) const;
	void writeServiceImplDispatcher(io::Printer& printer, const ServiceDescriptor* svcDesc) const;
	void writeServiceImplCleanUp(io::Printer& printer, const ServiceDescriptor* svcDesc) const;

	static const Descriptor* container(const Descriptor* desc);

};

}

#endif /* end of include guard: SERVICEGENERATORCPP_H_EBHB8ZPV */
