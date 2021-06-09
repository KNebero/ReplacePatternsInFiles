#include "pch.h"
#include "CppUnitTest.h"
#include "..\ReplacePatternsInFiles\Config.h"
#include "..\ReplacePatternsInFiles\modifyWithConfig.h"
#include <fstream>
#include <thread>
#include <mutex>
#include <set>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace std::filesystem;

namespace ReplacePatternsInFilesTest
{
	const string config_path = "..\\ReplacePatternsInFilesTest\\config.cfg";

	const path start_path(L"..\\ReplacePatternsInFilesTest\\Testing files\\test dir");

	const int n_threads = 4;

	const map<wstring, wstring> patterns = {
		{ L"<%COLOR>", L"BLACK" },
		{ L"'Font'", L"Calibri" },
		{ L"(#ЕДА)", L"пельмени" },
		{ L"<TV SHOW>", L"Поле чудес" },
		{ L"<pattern1>", L"<pattern2>" },
		{ L"<pattern2>", L"something" }
	};

	TEST_MODULE_INITIALIZE(setConfigPath) {
		Config::setConfigPath(config_path);
	}

	TEST_CLASS(TestConfig)
	{
	public:

		TEST_METHOD(TestConfigReadingAndGettingItems)
		{
			exception_ptr eptr;
			try {
				const Config* cfg = Config::getInstance();

				Assert::IsTrue(cfg->getStartDirectory() == start_path, L"Не свопадает стартовая директория");
				Assert::IsTrue(cfg->getNumberOfThreads() == n_threads, L"Не совпадает кол-во потоков");
				Assert::IsTrue(cfg->getPatterns() == patterns, L"Не совпадает список шаблонов");
			}
			catch (...) {
				eptr = current_exception();
				try {
					if (eptr) {
						rethrow_exception(eptr);
					}
				}
				catch (const exception& e) {
					Logger::WriteMessage(e.what());
					Assert::IsTrue(false, L"Вызвано исключение, см логи");
				}
			}
		}
	};

	TEST_CLASS(TestFunctions)
	{
	public:

		TEST_METHOD(replacePatternsInLineUsingConfigTest)
		{
			exception_ptr eptr;
			try {
				const Config* cfg = Config::getInstance();

				wstring line = L"Покрасил стены в <%COLOR> цвет и пошёл смотреть <TV SHOW>";
				wstring ans_line = L"Покрасил стены в " + patterns.at(L"<%COLOR>") +
					L" цвет и пошёл смотреть " + patterns.at(L"<TV SHOW>");

				Assert::IsTrue(replacePatternsInLineUsingConfig(line) == ans_line, L"Замена шаблонов в строке работает некорректно");
			}
			catch (...) {
				eptr = current_exception();
				try {
					if (eptr) {
						rethrow_exception(eptr);
					}
				}
				catch (const exception& e) {
					Logger::WriteMessage(e.what());
					Assert::IsTrue(false, L"Вызвано исключение, см логи");
				}
			}
		}

		static wstring getMessageIfNotEqualFiles(const path& file1, const path& file2)
		{
			wifstream test_file(file1);
			wifstream ans_file(file2);

			wstring test_line, ans_line;
			wstring mssg = L"";

			while (!test_file.eof() && !ans_file.eof()) {
				getline(test_file, test_line);
				getline(ans_file, ans_line);

				if (test_line != ans_line) {
					mssg = L"Строка \n" + test_line + L"\nИз файла\n" + file1.wstring() +
						L"\n\nНе совпадает со строкой\n" + ans_line + L"\nИз файла\n" + file2.wstring();
					break;
				}
			}

			test_file.close();
			ans_file.close();

			return mssg;
		}

		static void rollbackFile(const path& file, const path& file_from) {
			remove(file);
			copy(file_from, file);
		}

		TEST_METHOD(replacePatternsInFileUsingConfigTest)
		{
			const path test_file_path = start_path.parent_path() / L"Test two files\\test.txt";
			const path ans_file_path = start_path.parent_path() / L"Test two files\\ans.txt";
			const path rollback_path = start_path.parent_path() / L"Test two files\\rollback test.txt";
			wstring mssg = L"";

			exception_ptr eptr;
			try {
				replacePatternsInFileUsingConfig(test_file_path);

				mssg = getMessageIfNotEqualFiles(test_file_path, ans_file_path);
			}
			catch (...) {
				eptr = current_exception();
				try {
					if (eptr) {
						rethrow_exception(eptr);
					}
				}
				catch (const exception& e) {
					Logger::WriteMessage(e.what());
					mssg =  L"Вызвано исключение, см логи";
				}
			}

			try {
				rollbackFile(test_file_path, rollback_path);
			}
			catch (...) {
				eptr = current_exception();
				try {
					if (eptr) {
						rethrow_exception(eptr);
					}
				}
				catch (const exception& e) {
					Logger::WriteMessage(e.what());
					Assert::IsTrue(false, L"Вызвано исключение, см логи");
				}
			}

			Assert::IsTrue(mssg == L"", mssg.c_str());
		}

		static wstring getMessageIfNotEqualFilesInDirectories(const path& dir1, const path& dir2)
		{
			const Config* cfg = Config::getInstance();

			wstring mssg, Message = L"";

			for (recursive_directory_iterator cp1(dir1), cp2(dir2), end;
				cp1 != end && cp2 != end; ++cp1, ++cp2) {

				if (cp1->path().filename() == cp2->path().filename() &&
					cp1->path().extension().compare(L".txt") == 0) {
					
					mssg = getMessageIfNotEqualFiles(cp1->path(), cp2->path());

					if (Message == L"") {
						Message = mssg;
					}
				}
			}

			return Message;
		}

		static void rollbackFilesInDirectory(const path& dir, const path& dir_from) {
			for (recursive_directory_iterator cp1(dir), cp2(dir_from), end;
				cp1 != end && cp2 != end; ++cp1, ++cp2) {

				if (cp1->path().filename() == cp2->path().filename() &&
					cp1->path().extension().compare(L".txt") == 0) {

					rollbackFile(cp1->path(), cp2->path());
				}
			}
		}

		TEST_METHOD(modifyFilesMultithreadedUsingConfigTest)
		{
			const path test_path = start_path;
			const path ans_path = start_path.parent_path() / L"ans dir";
			const path rollback_path = start_path.parent_path() / L"rollback test dir";
			wstring mssg = L"";

			exception_ptr eptr;
			try {
				const Config* cfg = Config::getInstance();

				exception exc("");

				vector<thread> threads;

				for (int i = 0; i < cfg->getNumberOfThreads(); ++i) {
					threads.push_back(thread(modifyFilesMultithreadedUsingConfig, ref(exc)));
				}

				for (int i = 0; i < (int)threads.size(); ++i) {
					threads[i].join();
				}

				if (strcmp(exc.what(), "") != 0) {
					throw exc;
				}

				mssg = getMessageIfNotEqualFilesInDirectories(test_path, ans_path);
			}
			catch (...) {
				eptr = current_exception();
				try {
					if (eptr) {
						rethrow_exception(eptr);
					}
				}
				catch (const exception& e) {
					Logger::WriteMessage(e.what());
					mssg = L"Вызвано исключение, см логи";
				}
			}

			try {
				rollbackFilesInDirectory(test_path, rollback_path);
			}
			catch (...) {
				eptr = current_exception();
				try {
					if (eptr) {
						rethrow_exception(eptr);
					}
				}
				catch (const exception& e) {
					Logger::WriteMessage(e.what());
					Assert::IsTrue(false, L"Вызвано исключение, см логи");
				}
			}

			Assert::IsTrue(mssg == L"", mssg.c_str());
		}
	};
}
