#include <exception>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include "Config.h"
#include "modifyWithConfig.h"

using namespace std;
using namespace filesystem;


const path config_path("config.cfg");


int main() {
	setlocale(LC_ALL, "Russian");
	
	exception_ptr eptr;

	try {
		Config::setConfigPath(config_path);
		const Config* cfg = Config::getInstance();

		vector<thread> threads;

		exception exc("");

		for (int i = 0; i < cfg->getNumberOfThreads(); ++i) {
			threads.push_back(thread(modifyFilesMultithreadedUsingConfig, ref(exc)));
		}

		for (int i = 0; i < (int)threads.size(); ++i) {
			threads[i].join();
		}

		if (strcmp(exc.what(), "") != 0) {
			throw exc;
		}


		cout << "¬се шаблоны в указанной директории заменены" << endl;
	}
	catch (...) {
		eptr = current_exception();
		try {
			if (eptr) {
				rethrow_exception(eptr);
			}
		}
		catch (const exception& e) {
			cout << e.what() << endl;
		}
	}


	return 0;
}