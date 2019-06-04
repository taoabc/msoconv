/**
 * @file
 * @brief 提供字符串操作，这里使用inline函数，方便大家使用其返回值
 * @author huhaitao
 */
#ifndef ULT_STRING_OPERATE_H_
#define ULT_STRING_OPERATE_H_

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <Windows.h>
#include <cwctype>
#include <cctype>
#include <ctime>
#include <cstdlib>

namespace ult {

namespace detail {

struct MultiByteToUnicode {
  std::wstring operator()(const char* src, int len, unsigned int codepage) {
    if (len > 0) {
      int outlen = ::MultiByteToWideChar(codepage, 0, src, len, nullptr, 0);
      std::unique_ptr<wchar_t[]> buffer = std::make_unique<wchar_t[]>(outlen);
      outlen = ::MultiByteToWideChar(codepage, 0, src, len, buffer.get(), len);
      return std::wstring(buffer.get(), outlen);
    }
    return std::wstring();
  }
};

struct UnicodeToMultiByte {
  std::string operator()(const wchar_t* src, int len, unsigned int codepage) {
    if (len > 0) {
      int outlen = ::WideCharToMultiByte(codepage, 0, src, len, nullptr, 0, nullptr, nullptr);
      std::unique_ptr<char[]> buffer = std::make_unique<char[]>(outlen);
      outlen = ::WideCharToMultiByte(codepage, 0, src, len, buffer.get(), outlen, nullptr, nullptr);
      return std::string(buffer.get(), outlen);
    }
    return std::string();
  }
};

struct StringICompare {
  template <typename CharT>
  int operator()(const std::basic_string<CharT>& comp1, const std::basic_string<CharT>& comp2) {
    size_t len1 = comp1.length();
    size_t len2 = comp2.length();
    size_t cmplen = len1 < len2 ? len1 : len2;
    for (size_t i = 0; i < cmplen; ++i) {
      if (!EqCharT(comp1.at(i), comp2.at(i))) {
        return LtCharT(comp1.at(i), comp2.at(i)) ? -1 : 1;
      }
    }
    return len1 - len2;
  }

private:
  bool EqCharT(const wchar_t& c1, const wchar_t& c2) {
    return std::towupper(c1) == std::towupper(c2);
  }
  bool EqCharT(const char& c1, const char& c2) {
    return std::toupper(c1) == std::toupper(c2);
  }

  bool LtCharT(const wchar_t& c1, const wchar_t& c2) {
    return std::towupper(c1) < std::towupper(c2);
  }
  bool LtCharT(const char& c1, const char& c2) {
    return std::toupper(c1) < std::toupper(c2);
  }
};

struct StringToLower {
	template <typename CharT>
	std::basic_string<CharT> operator()(const std::basic_string<CharT>& str) {
    std::basic_string<CharT> result(str);
    for (auto iter = result.begin(); iter != result.end(); ++iter) {
      *iter = ToLowerCharT(*iter);
    }
    return result;
	}
private:
	wchar_t ToLowerCharT(const wchar_t& c) {
		return std::towlower(c);
	}
	char ToLowerCharT(const char& c) {
		return std::tolower(c);
	}
};

struct StringToUpper {
  template <typename CharT>
  std::basic_string<CharT> operator()(const std::basic_string<CharT>& str) {
    std::basic_string<CharT> result(str);
    for (auto iter = result.begin(); iter != result.end(); ++iter) {
      *iter = ToUpperCharT(*iter);
    }
    return result;
	}
private:
	wchar_t ToUpperCharT(const wchar_t& c) {
		return std::towupper(c);
	}
	char ToUpperCharT(const char& c) {
		return std::toupper(c);
	}
};

#define ISUTF8_CHECK_BIT(ub, b) (ub & (1<<b))
struct IsUtf8 {
	bool operator()(const char *text, int len) {
		int i = 0;
		const unsigned char *ut = reinterpret_cast<const unsigned char *>(text);
		int ret = 0;
		int count = 0;
		int b = 0;

		while (i < len) {
			count = 0;
			b = 7;
			while (b >= 0 && b <= 7) {
				if (ISUTF8_CHECK_BIT(ut[i], b)) {
					count++;
					b--;
				} else
					break;
			}

			if (count >= 2) {
				count--;
			} else if (count != 0) {  
				return 0;
			}
			while (count && i < len) {
				i++;
				if (ISUTF8_CHECK_BIT(ut[i], 7) > 0 && 0 == (ISUTF8_CHECK_BIT(ut[i], 6))) {
					count--;
					if (len - 1 == i) {     //finish
						ret = 1;
						break;
					}
				} else {
					break;
				}                   //if
			}                       // while count && i<len
			if (count > 0)
				return 0;
			i++;
		}                           //while i<len
		return 1;
	}
};

struct UInt64ToString {
  std::wstring operator()(unsigned __int64 num) {
    wchar_t temp[32];
    std::wstring result;
    int pos = 0;
    do {
      temp[pos++] = (wchar_t)(L'0' + (int)(num % 10));
      num /= 10;
    } while (0 != num);
    do {
      result += temp[--pos];
    } while (pos > 0);
    return result;
  }
};

struct UrlEncode {
  std::string operator()(const char* s, size_t len) {
    std::string encoded;
    char* buf = new char[16];
    unsigned char c;
    for (size_t i = 0; i < len; ++i) {
      c = s[i];
      if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_' ||
        c == '!' || c == '~' || c == '*' || c == '\'' || c == '(' || c == ')') {
          encoded += c;
          continue;
      }
      if (c == ' ') {
        encoded += '+';
        continue;
      }
      sprintf_s(buf, 16, "%x", c);
      encoded += '%';
      encoded += buf;
    }
    delete[] buf;
    return encoded;
  }
};

struct GetRandomString {
  std::wstring operator()(const size_t len, const std::wstring& random_table) {
    std::wstring random_string;
    std::wstring random_table_real(random_table);
    if (random_table_real.empty()) {
      random_table_real = L"1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    }
    int sreal = (int)random_table_real.length();
    for (size_t i = 0; i < len; ++i) {
      int r = GetRandomInteger(0, sreal);
      random_string.push_back(random_table_real.at(r));
    }
    return random_string;
  }

  GetRandomString(void) {
    SRand();
  }

private:
  int GetRandomInteger(int min_number, int max_number) {
    if (min_number > max_number) {
      std::swap(min_number, max_number);
    }
    return (int)((double)std::rand() / (RAND_MAX + 1) * (max_number - min_number) + min_number);
  }

  void SRand(void) {
    std::srand((unsigned int)std::time(NULL));
  }
};
} //namespace detail

inline std::wstring Utf8ToUnicode(const char* src, int len) {
  return detail::MultiByteToUnicode()(src, len, CP_UTF8);
}

inline std::wstring Utf8ToUnicode(const std::string& src) {
  return Utf8ToUnicode(src.c_str(), static_cast<int>(src.length()));
}

inline std::string UnicodeToUtf8(const wchar_t* src, int len) {
  return detail::UnicodeToMultiByte()(src, len, CP_UTF8);
}

inline std::string UnicodeToUtf8(const std::wstring& src) {
  return UnicodeToUtf8(src.c_str(), static_cast<int>(src.length()));
}

inline std::wstring AnsiToUnicode(const char* src, int len) {
  return detail::MultiByteToUnicode()(src, len, CP_ACP);
}

inline std::wstring AnsiToUnicode(const std::string& src) {
  return AnsiToUnicode(src.c_str(), static_cast<int>(src.length()));
}

inline std::string UnicodeToAnsi(const wchar_t* src, int len) {
  return detail::UnicodeToMultiByte()(src, len, CP_ACP);
}

inline std::string UnicodeToAnsi(const std::wstring& src) {
  return UnicodeToAnsi(src.c_str(), static_cast<int>(src.length()));
}

inline std::string AnsiToUtf8(const char* src, int len) {
  return UnicodeToUtf8(AnsiToUnicode(src, len));
}

inline std::string AnsiToUtf8(const std::string& src) {
  return UnicodeToUtf8(AnsiToUnicode(src));
}

inline std::string Utf8ToAnsi(const char* src, int len) {
  return UnicodeToAnsi(Utf8ToUnicode(src, len));
}

inline std::string Utf8ToAnsi(const std::string& src) {
  return UnicodeToAnsi(Utf8ToUnicode(src));
}

inline std::wstring IntToString(__int64 num) {
  std::wstring result;
  if (num < 0) {
    result += L'-';
    num = -num;
  }
  result.append(detail::UInt64ToString()(num));
  return result;
}

inline std::wstring UIntToString(unsigned __int64 num) {
  return detail::UInt64ToString()(num);
}

inline std::string UrlEncode(const char* s, size_t len) {
  return detail::UrlEncode()(s, len);
}

inline std::string UrlEncode(const std::string& s) {
  return UrlEncode(s.c_str(), s.length());
}

inline std::string UrlEncode(const wchar_t* s, size_t len) {
  return UrlEncode(UnicodeToAnsi(s, (int)len));
}

inline std::string UrlEncode(const std::wstring& s) {
  return UrlEncode(UnicodeToAnsi(s));
}

inline std::wstring GetRandomString(const size_t len, const std::wstring& random_table = L"") {
  return detail::GetRandomString()(len, random_table);
}

template <typename CharT>
void StringSplit(const std::basic_string<CharT>& str,
    const CharT& separator, std::vector<std::basic_string<CharT> >* vec) {
  if (str.empty()) {
    return;
  }
  std::basic_istringstream<CharT> iss(str);
  std::basic_string<CharT> s;
  while (std::getline(iss, s, separator)) {
    if (!s.empty()) {
      vec->push_back(s);
    }
  }
}

template <typename CharT>
void StringSplit(const std::basic_string<CharT>& str,
  const std::basic_string<CharT>& separator, std::vector<std::basic_string<CharT> >* vec) {
  if (str.empty()) {
    return;
  }
  if (separator.empty()) {
    vec->push_back(str);
    return;
  }
  size_t pos;
  std::basic_string<CharT> tmp(str);
  std::basic_string<CharT> item;
  size_t separator_len = separator.length();
  while ((pos = tmp.find(separator)) != std::basic_string<CharT>::npos) {
    item = tmp.substr(0, pos);
    if (!item.empty()) {
      vec->push_back(item);
    }
    tmp = tmp.substr(pos + separator_len);
  }
  if (!tmp.empty()) {
    vec->push_back(tmp);
  }
}

template <typename CharT>
int StringICompare(const std::basic_string<CharT>& comp1, const std::basic_string<CharT>& comp2) {
  return detail::StringICompare()(comp1, comp2);
}

template <typename CharT>
const std::basic_string<CharT> StringToLower(const std::basic_string<CharT>& str) {
	return detail::StringToLower()(str);
}

template <typename CharT>
const std::basic_string<CharT> StringToUpper(const std::basic_string<CharT>& str) {
	return detail::StringToUpper()(str);
}

inline bool IsUtf8(const std::string& str) {
	return detail::IsUtf8()(str.data(), (int)str.size());
}

template <typename CharT>
std::basic_string<CharT> StringReplace(const std::basic_string<CharT>& str,
    const std::basic_string<CharT>& match, const std::basic_string<CharT>& replaced) {
  std::basic_string<CharT> result(str);
  size_t pos = 0;
  while ((pos = result.find(match, pos)) != std::basic_string<CharT>::npos) {
    result.replace(pos, match.length(), replaced);
    pos += replaced.length();
  }
  return result;
}

template <typename CharT>
std::basic_string<CharT> StringLTrim(const std::basic_string<CharT>& str, const CharT* trim_chars) {
  size_t pos = str.find_first_not_of(trim_chars);
  if (pos == std::basic_string<CharT>::npos) {
    return std::basic_string<CharT>();
  }
  return str.substr(pos);
}

template <typename CharT>
std::basic_string<CharT> StringRTrim(const std::basic_string<CharT>& str, const CharT* trim_chars) {
  size_t pos = str.find_last_not_of(trim_chars);
  if (pos == std::basic_string<CharT>::npos) {
    return std::basic_string<CharT>();
  }
  return str.substr(0, pos + 1);
}

template <typename CharT>
std::basic_string<CharT> StringTrim(const std::basic_string<CharT>& str, const CharT* trim_chars) {
  return StringLTrim(StringRTrim(str, trim_chars), trim_chars);
}
} //namespace ult

#endif // ULT_STRING_H_