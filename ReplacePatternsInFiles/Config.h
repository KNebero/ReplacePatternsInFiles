#pragma once

#ifndef _CONFIG_H
#define _CONFIG_H

#include <filesystem>
#include <map>
#include <string>
#include <iostream>


class ConfigException : public std::exception {
public:
	ConfigException() : std::exception::exception() {}

	ConfigException(const ConfigException& _Exception) : std::exception::exception(_Exception.what()) {}

	ConfigException(const char const* _Message) : std::exception::exception(_Message) {}
};


class Config {
private:

	/**
		Класс для очистки памяти после использования синглтона Config
	*/
	class ConfigDestroyer {
	public:
		Config* _instance;

		ConfigDestroyer() { _instance = nullptr; }

		ConfigDestroyer(const ConfigDestroyer&) = delete;

		ConfigDestroyer& operator=(const ConfigDestroyer&) = delete;

		~ConfigDestroyer() { if (_instance != nullptr) delete _instance; }
	};

	inline static ConfigDestroyer _destroyer = ConfigDestroyer();
	inline static Config* _instance = nullptr;
	inline static std::filesystem::path config_path = std::filesystem::path("");

	std::filesystem::path start_directory;
	int n_threads;
	std::map<std::wstring, std::wstring> patterns;

	Config() {}

	/**
		Метод считывает данные из config файла в фиксированном формате и помещает их в соответствующие поля класса.
		(не подходящие под формат данные игнорируются)

		Throws:
		ConfigException - невозможно произвести чтение из config файла, некторые данные
			(в фиксированном формате) отсутствуют или полученные данные некорректны
	 */
	void readDataFromConfigFile();

public:

	Config(Config&) = delete;

	Config& operator=(const Config&) = delete;

	/**
		Метод устанавливает путь, из которого будет производиться чтение config файла

		Parameters:
		_config_path - ссылка на объект класса std::filesystem::path, в котором содержится адрес config файла
	*/
	static void setConfigPath(const std::filesystem::path& _config_path);

	/**
		Метод создаёт объект текущего класса, если таковой ещё не был создан

		Returns:
		const Config const* - указатель на единственный экземпляр текущего класса

		Throws:
		ConfigException - ещё не установлен путь до config файла,
			невозможно произвести чтение из config файла, некторые данные
			(в фиксированном формате) отсутствуют или полученные данные некорректны
	*/
	static const Config* const getInstance();

	const std::filesystem::path& getStartDirectory() const { return start_directory; }

	int getNumberOfThreads() const { return n_threads; }

	const std::map<std::wstring, std::wstring>& getPatterns() const { return patterns; }
};

#endif // _CONFIG_H