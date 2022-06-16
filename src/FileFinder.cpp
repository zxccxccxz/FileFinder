#include "FileFinder.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <deque>
#include <mutex>

namespace filefinder {

	namespace fs = std::filesystem;

	size_t max_threads = 0;
	size_t threads_num = 0;
	std::deque<std::thread> threads;
	std::mutex m;

	bool file_found = false;
	std::string to_find = "";
	fs::path path_found;

	static void SearchFile(const fs::path& p) {
		if (fs::exists(p / fs::path(to_find))) {
			std::scoped_lock<std::mutex> sl(m);
			file_found = true;
			path_found = (p / fs::path(to_find));
		}
	}

	static void DirLoop(const fs::directory_entry& entry) {
		try {
			for (const auto& ent : fs::directory_iterator(entry.path())) {
				if (ent.is_directory()) {
					SearchFile(ent.path());

					if (file_found) break;
					//std::cout << ent.path().string() << std::endl;

					DirLoop(ent);
				}
			}
		}
		catch (fs::filesystem_error ex) {
			// std::cout << "\t <filesystem_error> Access denied: " << ex.path1() << std::endl;
		}
		catch (std::exception ex) {
			std::cout << "Exception thrown: " << ex.what() << std::endl;
		}
	}

	bool FindFile(const std::string& filename, size_t threads_max) {
		to_find = filename;
		max_threads = threads_max;

		std::cout << "Trying to find " << filename << std::endl;
		//auto start_time = std::chrono::steady_clock::now();

		// setlocale(LC_ALL, "en_US");
		fs::path root = fs::current_path().root_path();


		// search for file in root dir
		SearchFile(root);
		if (file_found) return true;

		for (const auto& entry : fs::directory_iterator(root)) {
			if (entry.is_directory()) {
				std::cout << entry.path().string() << std::endl;

				SearchFile(entry.path());
				if (file_found) break;

				// add threads
				if (threads_num < max_threads) {
					threads.emplace_back(DirLoop, entry);
					++threads_num;
					//std::cout << threads.back().get_id() << " : " << threads_num << std::endl;
				}
				else {
					threads.front().join();
					threads.pop_front();
					threads.emplace_back(DirLoop, entry);
				}
			}
		}

		for (auto& thr : threads)
			thr.join();

		//auto end_time = std::chrono::steady_clock::now();
		//auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		//std::cout << elapsed_ms.count() << " ms\n";

		if (!file_found) {
			std::cout << "\n\nUnable to find file " << to_find << std::endl;
			return false;
		}
		else {
			std::cout << "\n\nFound file: \t" << path_found.string() << std::endl;
			return true;
		}
	}
}
