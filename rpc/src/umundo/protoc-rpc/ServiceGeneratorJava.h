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
