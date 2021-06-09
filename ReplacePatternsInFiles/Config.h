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
		����� ��� ������� ������ ����� ������������� ��������� Config
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
		����� ��������� ������ �� config ����� � ������������� ������� � �������� �� � ��������������� ���� ������.
		(�� ���������� ��� ������ ������ ������������)

		Throws:
		ConfigException - ���������� ���������� ������ �� config �����, �������� ������
			(� ������������� �������) ����������� ��� ���������� ������ �����������
	 */
	void readDataFromConfigFile();

public:

	Config(Config&) = delete;

	Config& operator=(const Config&) = delete;

	/**
		����� ������������� ����, �� �������� ����� ������������� ������ config �����

		Parameters:
		_config_path - ������ �� ������ ������ std::filesystem::path, � ������� ���������� ����� config �����
	*/
	static void setConfigPath(const std::filesystem::path& _config_path);

	/**
		����� ������ ������ �������� ������, ���� ������� ��� �� ��� ������

		Returns:
		const Config const* - ��������� �� ������������ ��������� �������� ������

		Throws:
		ConfigException - ��� �� ���������� ���� �� config �����,
			���������� ���������� ������ �� config �����, �������� ������
			(� ������������� �������) ����������� ��� ���������� ������ �����������
	*/
	static const Config* const getInstance();

	const std::filesystem::path& getStartDirectory() const { return start_directory; }

	int getNumberOfThreads() const { return n_threads; }

	const std::map<std::wstring, std::wstring>& getPatterns() const { return patterns; }
};

#endif // _CONFIG_H