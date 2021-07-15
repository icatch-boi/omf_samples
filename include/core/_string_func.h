//
// Created by jg.wang on 2020/11/2.
//
#pragma once

namespace omf{
	constexpr inline char Lower(char c) { return (c <= 'Z' && c >= 'A') ? (c + 'a' - 'A') : c; };
	constexpr inline char Upper(char c) { return (c <= 'z' && c >= 'a') ? (c + 'A' - 'a') : c; };

	void ToLower(char*);
	void ToUpper(char*);
	bool Replace(char*, char, char);
	bool Replace(char*, char*, char);
	char* Strdup(const char*);
	char* StrdupTo(const char*,char);
}
