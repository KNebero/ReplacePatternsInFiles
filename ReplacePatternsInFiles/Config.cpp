#include <fstream>
#include <regex>
#include <thread>
#include "Config.h"

using namespace std;
using namespace std::filesystem;


void Config::setConfigPath(const path& _config_path) {
	if (_instance == nullptr) {
		config_path = _config_path;
	}
}

const Config* const Config::getInstance() {
	if (_instance == nullptr) {
		if (config_path.empty()) {
			throw ConfigException("You must set the config path first");
		}

		_instance = new Config();
		
		try {
			_instance->readDataFromConfigFile();
		}
		catch (ConfigException& exc) {
			delete _instance;
			_instance = nullptr;
			throw exc;
		}

		_destroyer._instance = _instance;
	}

	return _instance;
}


bool preprocessText_IfSuccessFormatStringAndSetAsFound(wstring& s, const wregex& re,
											size_t first_n_to_delete, bool& found);

string getMessageIfNotCondition(bool condition, const string& if_wrong);


void Config::readDataFromConfigFile() {
	wifstream fin(config_path);

	if (!fin.is_open()) {
		throw ConfigException(("File " + config_path.string() + " was not found").c_str());
	}

	fin.imbue(locale("ru_RU.utf8"));

	patterns.clear();

	wstring line;

	bool path_found = false;
	bool threads_found = false;

	while (!fin.eof()) {
		getline(fin, line);

		if (preprocessText_IfSuccessFormatStringAndSetAsFound(line, wregex(L"path=.+[\\s|\\t]*"),
													sizeof("path=") - 1, path_found)) {
			start_directory = line;
			continue;
		}

		if (preprocessText_IfSuccessFormatStringAndSetAsFound(line, wregex(L"threads=[-]{0,1}[0-9]+[\\s|\\t]*"),
													sizeof("threads=") - 1, threads_found)) {
			n_threads = stoi(line);
			continue;
		}

		if (regex_match(line.c_str(), wregex(L"pattern\\s\"[^\"]+\"->\"[^\"]+\"[\\s|\\t]*"))) {
			wstring key, value;

			wregex word_regex(L"\"[^\"]+\"");
			auto word = wsregex_iterator(line.begin(), line.end(), word_regex);

			key = word->str();
			key = key.substr(1, key.size() - 2);

			++word;

			value = word->str();
			value = value.substr(1, value.size() - 2);

			patterns[key] = value;
		}
	}
	fin.close();

	string exc;

	exc = getMessageIfNotCondition(path_found, "Error reading path from " + config_path.string());

	if (path_found) {
		exc += getMessageIfNotCondition(exists(path(start_directory)),
				"No such directory " + start_directory.string());
	}

	exc += getMessageIfNotCondition(threads_found, string(exc == "" ? "" : "\n") +
													"Error reading threads from " +
													config_path.string());

	if (threads_found) {
		exc += getMessageIfNotCondition(n_threads > 0 && n_threads <= thread::hardware_concurrency(),
				string(exc == "" ? "" : "\n") + "The specified number of threads is out of a valid range\n" +
				"The valid range for your system is from 1 to " + to_string(thread::hardware_concurrency()));
	}

	exc += getMessageIfNotCondition(patterns.size() != 0, string(exc == "" ? "" : "\n") +
														"No pattern was found");

	if (exc != "") {
		throw ConfigException(exc.c_str());
	}
}

bool preprocessText_IfSuccessFormatStringAndSetAsFound(wstring& s, const wregex& re,
											size_t first_n_to_delete, bool& found) {
	if (regex_match(s.c_str(), re)) {
		int delete_from = (int)s.size();

		for (int i = (int)s.size() - 1; i >= 0; --i) {
			if (s[i] != L' ' && s[i] != L'\t') {
				delete_from = i + 1;
				break;
			}
		}

		s.erase(delete_from);
		s.erase(0, first_n_to_delete);

		found = true;
		return true;
	}

	return false;
}

string getMessageIfNotCondition(bool condition, const string& if_wrong) {
	return (condition ? "" : if_wrong);
}