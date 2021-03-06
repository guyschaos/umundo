/**
 *  Copyright (C) 2012  Stefan Radomski (stefan.radomski@cs.tu-darmstadt.de)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the FreeBSD license as published by the FreeBSD
 *  project.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  You should have received a copy of the FreeBSD license along with this
 *  program. If not, see <http://www.opensource.org/licenses/bsd-license>.
 */

#include "umundo/protoc-rpc/ServiceGeneratorCPP.h"

#include <set>
#include <iostream>

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/cpp/cpp_file.h>
#include <google/protobuf/compiler/cpp/cpp_helpers.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.pb.h>

namespace umundo {

using namespace google::protobuf::compiler::cpp;

bool ServiceGeneratorCPP::Generate(const FileDescriptor* file,
                                   const string& parameter,
                                   GeneratorContext* generator_context,
                                   string* error) const {
	vector<pair<string, string> > options;
	ParseGeneratorParameter(parameter, &options);

	string dllexport_decl;
	for (size_t i = 0; i < options.size(); i++) {
		if (options[i].first == "dllexport_decl") {
			dllexport_decl = options[i].second;
		} else {
			*error = "Unknown generator option: " + options[i].first;
			return false;
		}
	}

	string basename = StripProto(file->name());
	string filenameID = FilenameIdentifier(file->name());

	// Generate header.
	{
		scoped_ptr<io::ZeroCopyOutputStream> output(generator_context->Open(basename + ".rpc.pb.h"));
		io::Printer printer(output.get(), '$');

		// header prolog
		printer.Print(
		    "// Generated by the umundo protocol buffer compiler. DO NOT EDIT!\n"
		    "// source: $filename$\n\n"
		    "#ifndef __PROTOBUF_$filename_identifier$\n"
		    "#define __PROTOBUF_$filename_identifier$\n\n"
		    "#include <umundo/rpc.h>\n"
		    "#include \"$basename$.pb.h\" // generated by protoc\n"
		    "#include \"$basename$.rpc.pb.h\" // generated by protoc\n",

		    "basename", basename,
		    "filename", file->name(),
		    "filename_identifier", filenameID);

		if (file->dependency_count() > 0) {
			for (int i = 0; i < file->dependency_count(); i++) {
				const FileDescriptor* fileDesc = file->dependency(i);
				printer.Print("#include \"$dependency$.pb.h\" // generated by protoc\n", "dependency", StripProto(fileDesc->name()));
			}
		}

		printer.Print("\n\nnamespace umundo {\n\n");

		if (file->service_count() > 0) {
			for (int i = 0; i < file->service_count(); i++) {
				const ServiceDescriptor* svcDesc = file->service(i);
				writeServiceStubHeader(printer, svcDesc);
				printer.Print("\n");
				writeServiceHeader(printer, svcDesc);
				printer.Print("\n\n");
			}
		}

		// header epilog
		printer.Print(
		    "\n}\n"
		    "#endif\n");
	}

	// Generate cc file.
	{
		scoped_ptr<io::ZeroCopyOutputStream> output(generator_context->Open(basename + ".rpc.pb.cc"));
		io::Printer printer(output.get(), '$');

		if (file->service_count() > 0) {
			// implementation prolog
			printer.Print(
			    "#include \"umundo/rpc/Service.h\"\n"
			    "#include \"umundo/rpc/ServiceManager.h\"\n"
			    "#include \"$basename$.rpc.pb.h\"\n\n"
			    "namespace umundo {\n\n",
			    "basename", basename
			);

			for (int i = 0; i < file->service_count(); i++) {
				const ServiceDescriptor* svcDesc = file->service(i);
				writeServiceImplConstructor(printer, svcDesc);
				printer.Print("\n");
				writeServiceImplDispatcher(printer, svcDesc);
				printer.Print("\n");
				writeServiceImplCleanUp(printer, svcDesc);
				printer.Print("\n");
				writeServiceStubImpl(printer, svcDesc);
				printer.Print("\n\n");
			}
		}

		if (file->service_count() > 0) {
			// implementation epilog
			printer.Print("\n}\n");
		}
	}

	return true;

}

void ServiceGeneratorCPP::writeServiceStubHeader(io::Printer& printer, const ServiceDescriptor* svcDesc) const {
	printer.Print(
	    "class $svcName$Stub : public ServiceStub {\n"
	    "public:\n"
	    "\t$svcName$Stub(ServiceDescription*);\n",
	    "svcName", svcDesc->name()
	);

	if (svcDesc->method_count() > 0) {
		for (int i = 0; i < svcDesc->method_count(); i++) {
			const MethodDescriptor* methodDesc = svcDesc->method(i);
			printer.Print("\t$outType$* $methodName$($inType$*);\n",
			              "inType", container(methodDesc->input_type())->name(),
			              "methodName", methodDesc->name(),
			              "outType", container(methodDesc->output_type())->name()
			             );
		}
	}
	printer.Print("};\n");
}

void ServiceGeneratorCPP::writeServiceHeader(io::Printer &printer, const ServiceDescriptor* svcDesc) const {
	printer.Print(
	    "class $svcName$Base : public Service {\n"
	    "public:\n"
	    "\t$svcName$Base();\n",
	    "svcName", svcDesc->name()
	);

	if (svcDesc->method_count() > 0) {
		for (int i = 0; i < svcDesc->method_count(); i++) {
			const MethodDescriptor* methodDesc = svcDesc->method(i);
			printer.Print("\tvirtual $outType$* $methodName$($inType$*) = 0;\n",
			              "inType", container(methodDesc->input_type())->name(),
			              "methodName", methodDesc->name(),
			              "outType", container(methodDesc->output_type())->name()
			             );
		}

		printer.Print(
		    "protected:\n"
		    "\tvoid callMethod(string&, void*, const string&, void*&, const string&);\n"
		    "\tvoid cleanUpObjects(string&, void*, void*);\n"
		);
	}
	printer.Print("};\n");
}

const Descriptor* ServiceGeneratorCPP::container(const Descriptor* desc) {
//	fprintf(stderr, "checking %s\n", desc->name().c_str());
	while(desc->containing_type() != NULL) {
		desc = desc->containing_type();
//		fprintf(stderr, "\t-> %s\n", desc->name().c_str());
	}
	return desc;
}

void ServiceGeneratorCPP::writeServiceImplConstructor(io::Printer& printer, const ServiceDescriptor* svcDesc) const {

	std::set<string> inTypes;
	std::set<string> outTypes;
	std::set<string>::iterator inTypeIter;
	std::set<string>::iterator outTypeIter;

	if (svcDesc->method_count() > 0) {
		for (int i = 0; i < svcDesc->method_count(); i++) {
			const MethodDescriptor* methodDesc = svcDesc->method(i);
			inTypes.insert(methodDesc->input_type()->name());
			outTypes.insert(methodDesc->output_type()->name());
		}
	}

	printer.Print(
	    "$className$Stub::$className$Stub(ServiceDescription* svcDesc) : ServiceStub(svcDesc)  {\n"
	    "\t_serviceName = \"$className$\";\n",
	    "className", svcDesc->name()
	);

	// register intypes at publisher for stub, outtypes at subscriber
	for (inTypeIter = inTypes.begin(); inTypeIter != inTypes.end(); inTypeIter++) {
		printer.Print(
		    "\t_rpcPub->registerType(\"$inType$\", new $inType$());\n",
		    "inType", (*inTypeIter)
		);
	}
	for (outTypeIter = outTypes.begin(); outTypeIter != outTypes.end(); outTypeIter++) {
		printer.Print(
		    "\t_rpcSub->registerType(\"$outType$\", new $outType$());\n",
		    "outType", (*outTypeIter)
		);
	}

	printer.Print("}\n\n");

	printer.Print(
	    "$className$Base::$className$Base() {\n"
	    "\t_serviceName = \"$className$\";\n",
	    "className", svcDesc->name()
	);

	// register intypes at sublisher for impl, outtypes at publisher
	for (inTypeIter = inTypes.begin(); inTypeIter != inTypes.end(); inTypeIter++) {
		printer.Print(
		    "\t_rpcSub->registerType(\"$inType$\", new $inType$());\n",
		    "inType", (*inTypeIter)
		);
	}
	for (outTypeIter = outTypes.begin(); outTypeIter != outTypes.end(); outTypeIter++) {
		printer.Print(
		    "\t_rpcPub->registerType(\"$outType$\", new $outType$());\n",
		    "outType", (*outTypeIter)
		);
	}

	printer.Print("}\n");

}

void ServiceGeneratorCPP::writeServiceImplDispatcher(io::Printer& printer, const ServiceDescriptor* svcDesc) const {

	printer.Print(
	    "void $className$Base::callMethod(string& methodName, void* in, const string& inType, void* &out, const string& outType) {\n"
	    "\tif (false) {\n",
	    "className", svcDesc->name()
	);

	if (svcDesc->method_count() > 0) {
		for (int i = 0; i < svcDesc->method_count(); i++) {
			const MethodDescriptor* methodDesc = svcDesc->method(i);
			printer.Print(
			    "\t} else if (methodName.compare(\"$methodName$\") == 0) {\n"
			    "\t\tout = (void*)$methodName$(($inType$*)in);\n",
			    "inType", methodDesc->input_type()->name(),
			    "methodName", methodDesc->name()
			);
		}
	}
	printer.Print("\t}\n}\n");
}

void ServiceGeneratorCPP::writeServiceImplCleanUp(io::Printer& printer, const ServiceDescriptor* svcDesc) const {
	printer.Print(
	    "void $className$Base::cleanUpObjects(string& methodName, void* in, void* out) {\n"
	    "\tif (false) {\n",
	    "className", svcDesc->name()
	);

	if (svcDesc->method_count() > 0) {
		for (int i = 0; i < svcDesc->method_count(); i++) {
			const MethodDescriptor* methodDesc = svcDesc->method(i);
			printer.Print(
			    "\t} else if (methodName.compare(\"$methodName$\") == 0) {\n"
			    "\t\tif (in != NULL) delete(($inType$*)in);\n"
			    "\t\tif (out != NULL) delete(($outType$*)out);\n",
			    "inType", methodDesc->input_type()->name(),
			    "outType", methodDesc->output_type()->name(),
			    "methodName", methodDesc->name()
			);
		}
	}
	printer.Print("\t}\n}\n");
}

void ServiceGeneratorCPP::writeServiceStubImpl(io::Printer& printer, const ServiceDescriptor* svcDesc) const {
	if (svcDesc->method_count() > 0) {
		for (int i = 0; i < svcDesc->method_count(); i++) {

			const MethodDescriptor* methodDesc = svcDesc->method(i);
			printer.Print(
			    "$outType$* $className$Stub",
			    "className", svcDesc->name(),
			    "outType", methodDesc->output_type()->name());

			printer.Print(
			    "::$methodName$($inType$* in) {\n"
			    "\tvoid* out = NULL;\n"
			    "\tcallStubMethod(\"$methodName$\", in, \"$inType$\", out, \"$outType$\");\n"
			    "\treturn ($outType$*)out;\n"
			    "}\n",
			    "inType", methodDesc->input_type()->name(),
			    "methodName", methodDesc->name(),
			    "outType", methodDesc->output_type()->name()
			);
		}
	}
}

}

int main(int argc, char* argv[]) {
	umundo::ServiceGeneratorCPP generator;
	return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
