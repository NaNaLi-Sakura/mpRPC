/*------------------------------------------------------------------------
 程序 : _violet.cpp

 功能 : 此程序是公共函数和类的定义文件

 作者 : 雪与冰心丶
------------------------------------------------------------------------*/

#include "_violet.h"
#include <cstring>

namespace vio
{
	//==========================< 字符串操作（一） >==========================

  char* deletelchr(char* str, const int ch)
  {
    if (str == nullptr) return nullptr;

    char* p = str;
    while (*p == ch) p++;

    memmove(str, p, strlen(str) - (p - str) + 1);

    return str;
  }
  std::string& deletelchr(std::string& str, const int ch)
  {
    size_t pos = str.find_first_not_of(ch);
    if ((pos != std::string::npos) && (pos != 0)) str.replace(0, pos, "");

    return str;
  }

  char* deleterchr(char* str, const int ch)
  {
    if (str == nullptr) return nullptr;

    char* pTraverse = str;
    char* pTarget = nullptr;

    while (*pTraverse != '\0') {
      if (*pTraverse == ch && pTarget == nullptr) pTarget = pTraverse;
      if (*pTraverse != ch) pTarget = nullptr;
      pTraverse++;
    }
    if (pTarget != nullptr) *pTarget = '\0';

    return str;
  }
  std::string& deleterchr(std::string& str, const int ch)
  {
    size_t pos = str.find_last_not_of(ch);
    if (pos != std::string::npos) str.erase(pos + 1);

    return str;
  }

  char* deletelrchr(char* str, const int ch)
  {
    deletelchr(str, ch);
    deleterchr(str, ch);

    return str;
  }
  std::string& deletelrchr(std::string& str, const int ch)
  {
    deletelchr(str, ch);
    deleterchr(str, ch);

    return str;
  }

  char* toupper(char* str)
  {
    if (!str) return nullptr;

    char* tP = str;
    while (*tP != '\0') {
      if ((*tP >= 'a') && (*tP <= 'z')) *tP -= 32;
      tP++;
    }

    return str;
  }
  std::string& toupper(std::string& str)
  {
    for (char& ch : str) {
      if ((ch >= 'a') && (ch <= 'z')) ch -= 32;
    }

    return str;
  }

  std::string& picknumber(const std::string& src_str, std::string& dst_str, const bool contain_sign, const bool contain_dot)
  {
    std::string tempstr{};
    for (char ch : src_str) {
      if (isdigit(ch)) {
        tempstr.append(1, ch); continue;
      }
      if ((contain_sign == true) && (ch == '+' || ch == '-')) {
        tempstr.append(1, ch); continue;
      }
      if ((contain_dot == true) && (ch == '.')) tempstr.append(1, ch);
    }
    dst_str = tempstr;

    return dst_str;
  }
  std::string picknumber(const std::string& src_str, const bool contain_sign, const bool contain_dot)
  {
    std::string tempstr{};
    picknumber(src_str, tempstr, contain_sign, contain_dot);

    return tempstr;
  }
  char* picknumber(const std::string& src_str, char* dst_str, const bool contain_sign, const bool contain_dot)
  {
    if (!dst_str) return nullptr;

    std::string tempstr = picknumber(src_str, contain_sign, contain_dot);
    tempstr.copy(dst_str, tempstr.length());
    dst_str[tempstr.length()] = '\0';

    return dst_str;
  }

	//==========================< 字符串操作（二） >==========================

  ccmdstr::ccmdstr(const std::string& strbuffer, const std::string& separator, const bool is_delspace)
  {
    splittocmd(strbuffer, separator, is_delspace);
  }

  void ccmdstr::splittocmd(const std::string& strbuffer, const std::string& separator, const bool is_delspace)
  {
    _vcmdstr.clear();

    int startPos = 0;
    int endPos = 0;
    std::string substr{};
    while ((endPos = strbuffer.find(separator, startPos)) != std::string::npos) {
      substr = strbuffer.substr(startPos, endPos - startPos);
      if (is_delspace) deletelrchr(substr);
      _vcmdstr.push_back(std::move(substr));

      startPos = endPos + separator.length();
    }
    substr = strbuffer.substr(startPos);
    if (is_delspace) deletelrchr(substr);
    _vcmdstr.push_back(std::move(substr));
  }

  bool ccmdstr::getvalue(const int index, std::string& value, const int len) const
  {
    if (index >= _vcmdstr.size()) return false;

    int tmplen = _vcmdstr[index].length();
    if ((len > 0) && (len < tmplen)) tmplen = len;
    value = _vcmdstr[index].substr(0, tmplen);

    return true;
  }
  bool ccmdstr::getvalue(const int index, char* value, const int len) const
  {
    if ((value == nullptr) || (index >= _vcmdstr.size())) return false;
    if (len > 0) memset(value, 0, len + 1);

    int tmplen = _vcmdstr[index].length();
    if ((len > 0) && (len < tmplen)) tmplen = len;
    _vcmdstr[index].copy(value, tmplen);
    value[tmplen] = '\0';

    return true;
  }
  bool ccmdstr::getvalue(const int index, int& value) const
  {
    if (index >= _vcmdstr.size()) return false;

    try {
      value = std::stoi(picknumber(_vcmdstr[index], true, false));
    }
    catch (const std::exception& except) {
      // cerr << except.what() << endl;
      return false;
    }

    return true;
  }
	bool ccmdstr::getvalue(const int index, unsigned int& value) const
  {
    if (index >= _vcmdstr.size()) return false;

    try {
      value = stoi(picknumber(_vcmdstr[index], false, false));
    }
    catch (const std::exception& except) {
      // cerr << except.what() << endl;
      return false;
    }

    return true;
  }
	bool ccmdstr::getvalue(const int index, long& value) const
  {
    if (index >= _vcmdstr.size()) return false;

    try {
      value = stol(picknumber(_vcmdstr[index], true, false));
    }
    catch (const std::exception& except) {
      // cerr << except.what() << endl;
      return false;
    }

    return true;
  }
	bool ccmdstr::getvalue(const int index, unsigned long& value) const
  {
    if (index >= _vcmdstr.size()) return false;

    try {
      value = stol(picknumber(_vcmdstr[index], false, false));
    }
    catch (const std::exception& except) {
      // cerr << except.what() << endl;
      return false;
    }

    return true;
  }
	bool ccmdstr::getvalue(const int index, double& value) const
  {
    if (index >= _vcmdstr.size()) return false;

    try {
      value = stod(picknumber(_vcmdstr[index], true, true));
    }
    catch (const std::exception& except) {
      // cerr << except.what() << endl;
      return false;
    }

    return true;
  }
	bool ccmdstr::getvalue(const int index, float& value) const
  {
    if (index >= _vcmdstr.size()) return false;

    try {
      value = stof(picknumber(_vcmdstr[index], true, true));
    }
    catch (const std::exception& except) {
      // cerr << except.what() << endl;
      return false;
    }

    return true;
  }
	bool ccmdstr::getvalue(const int index, bool& value) const
  {
    if (index >= _vcmdstr.size()) return false;

    std::string numberstr = _vcmdstr[index];
    if (toupper(numberstr) == "TRUE") value = true;
    else value = false;

    return true;
  }

  std::ostream& operator << (std::ostream& out, const ccmdstr& cmdstr)
  {
    for (int i = 0; i < cmdstr.size(); ++i) {
      out << "[" << i << "]" << cmdstr[i] << std::endl;
    }

    return out;
  }

	//==========================< 读文件 >==========================

	bool cifile::open(const std::string& filename, const std::ios::openmode mode)
  {
    //关闭旧文件
    if (_fin.is_open()) _fin.close();

    _filename = filename;
    _fin.open(_filename, mode);

    return _fin.is_open();
  }

	bool cifile::readline(std::string& buffer, const std::string& endsign)
  {
    buffer.clear();
    std::string strline{};
    while (true) {
      getline(_fin, strline);
      if (_fin.eof()) break;

      buffer += strline;
      if (endsign == "") return true;
      if (buffer.find(endsign, buffer.length() - endsign.length()) != std::string::npos)
        return true;
      buffer += '\n';
    }

    return false;
  }

	int cifile::read(void* buffer, const int bufsize)
  {
    _fin.read(static_cast<char*>(buffer), bufsize);

    return _fin.gcount();
  }

	bool cifile::closeandremove()
  {
    if (_fin.is_open() == false) return false;
    _fin.close();
    if (remove(_filename.c_str()) == -1) return false;

    return true;
  }

	void cifile::close()
  {
    if (!_fin.is_open()) return;
    _fin.close();
  }

} //namespace vio