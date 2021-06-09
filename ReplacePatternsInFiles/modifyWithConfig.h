#pragma once

#ifndef _MODIFYWITHCONFIG_H
#define _MODIFYWITHCONFIG_H

#include <filesystem>
#include <map>
#include <string>


/**
	Функция заменяет все вхождения шаблонов в строке на соответствующие им замены.
	Все шаблоны и их замены берутся из синглтона Config.

	Parameters:
	line - объект класса std::wstring, в котором содержится исходная строка

	Returns:
	wstring - изменённая строка

	Throws:
	ConfigException - если синглтон Config ещё не был инициализирован
 */
std::wstring replacePatternsInLineUsingConfig(std::wstring line);


/**
	Функция заменяет все вхождения шаблонов в файле, находящемся по пути path_to_file.
	Все шаблоны и их замены берутся из синглтона Config.

	Parameters:
	path_to_file - ссылка на объект класса std::filesystem::path, в котором содержится адрес файла

	Throws:
	ConfigException - если синглтон Config ещё не был инициализирован
 */
void replacePatternsInFileUsingConfig(const std::filesystem::path& path_to_file);


/**
	Функция заменяет все всождения шаблонов в файлах, находящихся в стартовой директории
	и во всех её поддиректориях. Если в процессе выполнения возникло некоторое исключение,
	помещает в параметр _Exception это исключение.
	Стартовая директория и все шаблоны с их заменами берутся из синглтона Config.

	Parameters:
	_Exception - ссылка на объект класса std::exception, в который, в случае возникновения
		исключения, помещается это исключение
 */
void modifyFilesMultithreadedUsingConfig(std::exception& _Exception);

#endif // _MODIFYWITHCONFIG_H