#pragma once

#ifndef _MODIFYWITHCONFIG_H
#define _MODIFYWITHCONFIG_H

#include <filesystem>
#include <map>
#include <string>


/**
	������� �������� ��� ��������� �������� � ������ �� ��������������� �� ������.
	��� ������� � �� ������ ������� �� ��������� Config.

	Parameters:
	line - ������ ������ std::wstring, � ������� ���������� �������� ������

	Returns:
	wstring - ��������� ������

	Throws:
	ConfigException - ���� �������� Config ��� �� ��� ���������������
 */
std::wstring replacePatternsInLineUsingConfig(std::wstring line);


/**
	������� �������� ��� ��������� �������� � �����, ����������� �� ���� path_to_file.
	��� ������� � �� ������ ������� �� ��������� Config.

	Parameters:
	path_to_file - ������ �� ������ ������ std::filesystem::path, � ������� ���������� ����� �����

	Throws:
	ConfigException - ���� �������� Config ��� �� ��� ���������������
 */
void replacePatternsInFileUsingConfig(const std::filesystem::path& path_to_file);


/**
	������� �������� ��� ��������� �������� � ������, ����������� � ��������� ����������
	� �� ���� � ��������������. ���� � �������� ���������� �������� ��������� ����������,
	�������� � �������� _Exception ��� ����������.
	��������� ���������� � ��� ������� � �� �������� ������� �� ��������� Config.

	Parameters:
	_Exception - ������ �� ������ ������ std::exception, � �������, � ������ �������������
		����������, ���������� ��� ����������
 */
void modifyFilesMultithreadedUsingConfig(std::exception& _Exception);

#endif // _MODIFYWITHCONFIG_H