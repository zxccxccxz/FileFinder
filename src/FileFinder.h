#pragma once
#include <string>

namespace filefinder {
	bool FindFile(const std::string& name, size_t max_threads = 8);
}
