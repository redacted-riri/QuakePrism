/*
Copyright (C) 2024 Lance Borden

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3.0
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.

*/


#include <iostream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>
#include "resources.h"
#include <unistd.h>

struct Diagnostic {
    std::string file;
    int line;
    std::string type;
    std::string message;
};

static std::vector<Diagnostic> parseCompilerOutput(const std::string& output) {
    std::vector<Diagnostic> diagnostics;
    std::regex combinedRegex(R"(([^:]+):(\d+): (warning|error): (.+))");
    std::smatch match;

    // Use a string stream to read the output line by line
    std::istringstream outputStream(output);
    std::string line;

    while (std::getline(outputStream, line)) {
        if (std::regex_match(line, match, combinedRegex)) {
            Diagnostic diag;
            diag.file = match[1];
            diag.line = std::stoi(match[2]);
            diag.type = match[3];
            diag.message = match[4];
            diagnostics.push_back(diag);
        }
    }

    return diagnostics;
}

namespace QuakePrism {

std::string getCompilerOutputString() {
	std::string compilerOutput;
	chdir((baseDirectory / "src").string().c_str());
#ifdef _WIN32
	std::string command = "fteqcc64.exe 2>&1";
#else
	std::string command = "./fteqcc64 2>&1";
#endif

	FILE *pipe = popen(command.c_str(), "r");
	if (!pipe)
		return "";
	char buffer[128];
	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		compilerOutput += buffer;
	}
	pclose(pipe);
	chdir(baseDirectory.string().c_str());
	return compilerOutput;
}

void createTextEditorDiagnostics(const std::string& compilerOutput) {
    std::vector<Diagnostic> diagnostics = parseCompilerOutput(compilerOutput);
    
    for (const auto& diag : diagnostics) {
    	std::cout << diag.file << ":" << diag.line << ": " << diag.type << ": " << diag.message << std::endl;
    }
}
}