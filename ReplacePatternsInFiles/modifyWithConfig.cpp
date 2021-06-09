#include <exception>
#include <fstream>
#include <mutex>
#include <regex>
#include <set>
#include "Config.h"
#include "modifyWithConfig.h"

using namespace std;
using namespace filesystem;


wstring replacePatternsInLineUsingConfig(wstring line) {
	const Config* cfg = Config::getInstance();

	for (size_t i = 0; i < line.size(); ++i) {
		for (auto& pattern : cfg->getPatterns()) {
			if (line.substr(i, pattern.first.size()) == pattern.first) {
				line.replace(i, pattern.first.size(), pattern.second);
				i += pattern.second.size() - 1;
				break;
			}
		}
	}
	return line;
}

void replacePatternsInFileUsingConfig(const path& path_to_file) {
	const Config* cfg = Config::getInstance();

	path path_to_buff_file = path_to_file.parent_path();

	path_to_buff_file /= L"~";
	path_to_buff_file.concat(path_to_file.filename().string());

	wifstream file(path_to_file);
	file.imbue(locale("ru_RU.utf8"));

	wofstream bfile(path_to_buff_file);
	bfile.imbue(locale("ru_RU.utf8"));

	if (!file.is_open()) {
		throw exception(("Directory " + path_to_file.string() + " was not found").c_str());
	}
	
	wstring line;
	while (!file.eof()) {
		getline(file, line);
		line = replacePatternsInLineUsingConfig(line);
		bfile << line;
		if (!file.eof()) {
			bfile << endl;
		}
	}

	file.close();
	bfile.close();

	remove(path_to_file);
	rename(path_to_buff_file, path_to_file);
}

void modifyFilesMultithreadedUsingConfig(exception& _Exception) {
	static mutex work_with_set;
	static mutex work_with_exception;
	static set<path> used_paths;

	exception_ptr eptr;

	try {
		const Config* cfg = Config::getInstance();

		bool path_found;

		for (recursive_directory_iterator cp(cfg->getStartDirectory()), end; cp != end; ++cp) {

			if (cp->path().extension().compare(L".txt") == 0 &&
				cp->path().filename().string()[0] != L'~') {

				work_with_set.lock();

				path_found = used_paths.find(cp->path()) != used_paths.end();

				if (!path_found) {
					used_paths.insert(cp->path());
				}

				work_with_set.unlock();

				if (!path_found) {
					replacePatternsInFileUsingConfig(cp->path());
				}
			}
		}
	}
	catch (...) {
		eptr = current_exception();
		try {
			if (eptr) {
				rethrow_exception(eptr);
			}
		}
		catch (const exception& exc) {
			work_with_exception.lock();
			_Exception = exc;
			work_with_exception.unlock();
		}
	}
}