/*------------------------------------------------------------------------
 程序 : _violet.h

 功能 : 此程序是公共函数和类的声明文件

 作者 : 雪与冰心丶
------------------------------------------------------------------------*/

#ifndef _VIOLET_H
#define _VIOLET_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

namespace vio
{
	//==========================< 字符串操作（一） >==========================

	/// @brief 删除字符串最左边第一个ch段
	/// @param str 待处理的字符串
	/// @param ch 需要删除的字符，缺省删除空格
	/// @return 返回处理之后的字符串
  char* deletelchr(char* str, const int ch = ' ');
  std::string& deletelchr(std::string& str, const int ch = ' ');

	/// @brief 删除字符串最右边第一个ch段
	/// @param str 待处理的字符串
	/// @param ch 需要删去的字符，缺省删除空格
	/// @return 返回处理之后的字符串
  char* deleterchr(char* str, const int ch = ' ');
  std::string& deleterchr(std::string& str, const int ch = ' ');

	/// @brief 将字符串中的小写字母转换成大写，忽略不是字母的字符
	/// @param str 待转换的字符串
	/// @return 返回处理之后的字符串
  char* toupper(char* str);
  std::string& toupper(std::string& str);

	/// @brief 各删去左右两边第一个ch段
	/// @param str 待处理的字符串
	/// @param ch 需要删去的字符，缺省删除空格
	/// @return 返回处理之后的字符串
  char* deletelrchr(char* str, const int ch = ' ');
  std::string& deletelrchr(std::string& str, const int ch = ' ');

	/// @brief 从一个字符串中提取出数字、符号和小数点，存放到另一个字符串中。
	/// @param src_str 原字符串
	/// @param dest_str 目标字符串
	/// @param contain_sign 是否提取符号（+和-），true-包括；false-不包括
	/// @param contain_dot 是否提取小数点（.），true-包括；false-不包括
	/// @return 返回目标字符串
	// 注意：src_str和dest_str可以是同一个变量
	// 若相同，表示覆盖原字符串
	// 不相同，则不改变原字符串
  std::string& picknumber(const std::string& src_str, std::string& dst_str, const bool contain_sign = false, const bool contain_dot = false);
  std::string picknumber(const std::string& src_str, const bool contain_sign = false, const bool contain_dot = false);
  char* picknumber(const std::string& src_str, char* dst_str, const bool contain_sign = false, const bool contain_dot = false);

	//==========================< 字符串操作（二） >==========================

	/// @brief ccmdstr类用于拆分有分隔符的字符串。
  class ccmdstr
  {
  private:
    std::vector<std::string> _vcmdstr; //存放拆分后的字段内容。

    ccmdstr(const ccmdstr&) = delete;
    ccmdstr& operator = (const ccmdstr&) = delete;
  public:
    ccmdstr() = default;
    ccmdstr(const std::string& strbuffer, const std::string& separator, const bool is_delspace = false);

    const std::string& operator [] (const int index) const { return _vcmdstr[index]; }

		/// @brief 将字符串拆分到_vcmdstr容器中
		/// @param strbuffer 待拆分的字符串
		/// @param separator 分隔符。注意，separator参数的数据类型不是字符，是字符串
		/// @param is_delspace 拆分后是否删除字段内容前后的空格。true-删除，false-不删除，缺省不删除
    void splittocmd(const std::string& strbuffer, const std::string& separator, const bool is_delspace = false);

    //获取拆分后字段的个数，即_vcmdstr容器的大小（元素个数）
    int size() const { return _vcmdstr.size(); }

		/// @brief 从_vcmdstr容器中获取/截取字段内容(提取对应类型内容并接收)
		/// @param index 字段的顺序号，类似数组的下标，从0开始
		/// @param value 传入变量的地址/引用，用于存放字段内容。
		/// @param len 指定截取的长度。缺省为0-全部长度。
		/// @return 返回true-成功；如果index的取值超出了cmdStrVec容器的大小，返回false。
    bool getvalue(const int index, std::string& value, const int len = 0) const;
    bool getvalue(const int index, char* value, const int len = 0) const;
    bool getvalue(const int index, int& value) const;
		bool getvalue(const int index, unsigned int& value) const;
		bool getvalue(const int index, long& value) const;
		bool getvalue(const int index, unsigned long& value) const;
		bool getvalue(const int index, double& value) const;
		bool getvalue(const int index, float& value) const;
		bool getvalue(const int index, bool& value) const;

    ~ccmdstr() { _vcmdstr.clear(); }
  };

  //重载<<运算符，输出ccmdstr::_vcmdstr中的内容，方便调试。
  std::ostream& operator << (std::ostream& out, const ccmdstr& cmdstr);

	//==========================< 读文件 >==========================

	/// @brief 读文件的类 
	class cifile
	{
	private:
		std::ifstream _fin; //读取文件的对象（输入流对象）
		std::string _filename; //文件名（建议绝对路径）
	public:
		//默认构造函数。
		cifile() = default;

		//判断文件是否已打开。
		bool isopen() const { return _fin.is_open(); }

		/// @brief 打开文件。
		/// @param filename 待打开的文件名。
		/// @param mode 打开文件的模式。默认为in，即只读。
		/// @return 返回文件的打开状态
		bool open(const std::string& filename, const std::ios::openmode mode = std::ios::in);

		/// @brief 以行的方式读取文本文件
		/// @param buffer 接收缓冲区
		/// @param endsign 行的结尾标志。缺省为空，没有结尾标志
		/// @return true-读取一行或读到了结尾标志；false-读到了文件结束
		/*
		此函数会在结尾标志前面的自然行末尾保留换行符，结尾标志所在行末尾并不会保留换行符
		*/
		// 说明：
		// 无endsign时，认"\\n"。
		// 有endsign时，认"<endl/>\\n"。
		// 保留前面的'\\n'，不保留最后的'\\n'。
		bool readline(std::string& buffer, const std::string& endsign = "");

		//重载 >> 运算符
		cifile& operator >> (std::string& buffer);

		/// @brief 读取二进制文件。
		/// @param buffer 接收缓冲区
		/// @param bufsize 要读取的大小
		/// @return 返回实际读取到的字节数。
		int read(void* buffer, const int bufsize);

		//关闭并删除文件。
		bool closeandremove();

		//只关闭文件。
		void close();

		//析构函数。
		~cifile() { close(); }
	};

	//重载 >> 运算符
	inline cifile& cifile::operator >> (std::string& buffer)
	{
		_fin >> buffer;

		return *this;
	}

} // namespace vio

#endif //_VIOLET_H