#pragma once
#include <string>

class MyUtility
{
public:
	/// <summary>
	/// ログを出力
	/// </summary>
	/// <param name="message">メッセージ</param>
	static void Log(const std::string& message);

	/// <summary>
	/// string -> wstring 変換
	/// </summary>
	/// <param name="str">string</param>
	/// <returns>wstring</returns>
	static std::wstring ConvertString(const std::string& str);
	/// <summary>
	/// wstring -> string 変換
	/// </summary>
	/// <param name="str">wstring</param>
	/// <returns>string</returns>
	static std::string ConvertString(const std::wstring& str);


	static std::wstring GetResourceRoot();
	static std::string GetResourceRootChar();

	/// <summary>
	/// セーフデリート
	/// </summary>
	/// <typeparam name="T">VariableClass</typeparam>
	/// <param name="p">Variable</param>
	template<class T>
	inline static void SafeDelete(T*& p) {
		delete p;
		p = nullptr;
	}

};

