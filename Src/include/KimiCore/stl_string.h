#ifndef __KIMI_KERNEL_STL_STRING_H__
#define __KIMI_KERNEL_STL_STRING_H__

#include <io.h>
#include <string>

_KIMI_NAMESPACE_BEGIN

#define _STL_STRING_FORMAT_MAX_BUFFER	1024

class KIMI_CORE_CLASS stl_string
{
public:
	/**
	 * ��ָ�����Ƚض��ַ���������ַ����а��������ģ���������ְ�����ֵ����
	 *
	 * @param[in/out] s     ���ضϵ��ַ���
	 * @param[in]	  limit �����ַ�������
	 * @return				���ؽض��ַ�������
	 */
	static string::size_type truncate(string &s, string::size_type limit);

	/**
	 * ��һ���ַ����Կո�'\r'��'\n'��'\t'Ϊ���ָ������зָ�����طָ����ַ�������
	 *
	 * @param[in] limit     �������������
	 * @return				���طָ����ַ�������
	 */
	 static vector<string> split(const string &s, string::size_type limit = string::npos);

	/**
	 * ��һ���ַ���s��ָ�����ַ�separator���зָ�
	 *
	 * @param[in] s			���ָ���ַ���
	 * @param[in] separator �ָ��ַ�
	 * @param[in] limit     �������������
	 * @return				���طָ����ַ�������
	 *
	 * @ʹ������
	 * @code
	 * std::string s("This|is|a|test.");
	 * std::vector<std::string> v = s.split('|');
	 * std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
	 *
	 * This
	 * is
	 * a
	 * test.
	 * @endcode
	 */
	 static vector<string> split(const string &s, string::value_type separator, string::size_type limit = string::npos);

	/**
	 * ��һ���ַ���s��ָ�����ַ���separator���зָ�
	 *
	 * @param[in] s			���ָ���ַ���
	 * @param[in] separator �ָ��ַ���
	 * @param[in] limit     �������������
	 * @return				���طָ����ַ�������
	 *
	 * @ref split_ex
	 */
	static vector<string> split(const string &s, const string &separator, string::size_type limit = string::npos);

	/**
	 * ת���ַ���sΪ����
	 *
	 */
	static long int integer(const string &s);

	/**
	 * ���ַ�����ָ���Ŀ��С���зָ�������ַ�������
	 *
	 * @ʹ������
	 * @code
	 * string s("abcdefghijk");
	 * vector<string> v = s.chunk_split(3);
	 * std::copy(v.begin(), v.end(), ostream_iterator<string>(cout, " "));
	 *
	 * abc def ghi jk
	 * @endcode
	 */
	static vector<string> chunk_split(const string &s, string::size_type chunklen);

	/**
	 * ���ַ���s����������Ϊneedle�����ַ����������滻Ϊrs
	 *
	 * @ʹ������
	 * @code
	 * string s("This is a test.");
	 * s.replace("is", "ere");
	 * std::cout << s << std::endl;
	 *
	 * There ere a test.
	 * @endcode
	 */
	static void replace(string &s, const string &needle, const string &rs);

	/**
	 * ���ַ���s����������Ϊneedle���ַ��������滻Ϊc
	 *
	 */
	static void replace(string &s, string::value_type needle, string::value_type c);

	/**
	 * ���ַ���sת��ΪСд
	 *
	 */
	static void tolower(string &s);

	/**
	 * ���ַ���sת��Ϊ��д
	 *
	 */
	static void toupper(string &s);

	/**
	 * ͳ���ַ���str���ַ���s�г��ֵĴ���
	 *
	 */
	static string::size_type count(const string &s, const string &str);

	/**
	 * �ַ����Ƿ�ȫ����ĸ������a-z��A-Z��0-9
	 *
	 */
	static bool is_alnum(const string &s);

	/**
	 * �ַ����Ƿ�ȫ����ĸa-z��A-Z
	 *
	 */
	static bool is_alpha(const string &s);

	/**
	 * �ַ����Ƿ�ȫ��Ϊ����0-9
	 *
	 */
	static bool is_numeric(const string &s);

	/**
	 * �ַ����Ƿ�ΪСд
	 *
	 */
	static bool is_lower(const string &s);

	/**
	 * �ַ����Ƿ�Ϊ��д
	 *
	 */
	static bool is_upper(const string &s);

	/**
	 * ת���ַ��Ĵ�Сд
	 *
	 * @post ������Сд�ַ�ת��Ϊ��д�������д�д�ַ�ת��ΪСд
	 * @param[in/out] s	����һ���ַ����������ر�ת����Ľ��
	 * @return void
	 */
	static void swapcase(string &s);

	/**
	* 
	*/
	static void ltrim(string &s);
	static void rtrim(string &s);
	static void trim(string &s);

	/**
	 * ��ʽ���ַ���
	 *
	 * @param[in] fmt		�������ʽ�����Ʒ����ַ���
	 * @param[in] ...		�������ʽ�����Ʒ���Ӧ�ı��
	 * @return string		���ظ�ʽ������ַ������÷��ص��ַ�����null������1024����
	 */
	static string format(const char *fmt, ...);

	/**
	 * ��ʽ���ַ���
	 *
	 * @param[in] maxsize	��������ʽ������ַ�������
	 * @param[in] fmt		�������ʽ�����Ʒ����ַ���
	 * @param[in] ...		�������ʽ�����Ʒ���Ӧ�ı��
	 * @return string		���ظ�ʽ������ַ������÷��ص��ַ�����null������maxsize����
	 */
	static string format(size_t maxsize, const char *fmt, ...);

	/**
	 * ��ʽ���ַ���
	 *
	 * @param[out] s		���ڽ��ո�ʽ������ַ���
	 * @param[in] fmt		�������ʽ�����Ʒ����ַ���
	 * @param[in] ...		�������ʽ�����Ʒ���Ӧ�ı��
	 * @return bool			�Ƿ��ʽ���ɹ�
	 */
	static bool format(string& s, const char *fmt, ...);

	/**
	 * ��ʽ���ַ���
	 *
	 * @param[out] s		���ڽ��ո�ʽ������ַ������÷��ص��ַ�����null������maxsize����
	 * @param[in] maxsize	��������ʽ������ַ�������
	 * @param[in] fmt		�������ʽ�����Ʒ����ַ���
	 * @param[in] ...		�������ʽ�����Ʒ���Ӧ�ı��
	 * @return bool			�Ƿ��ʽ���ɹ�
	 */
	static bool format(string& s, size_t maxsize, const char *fmt, ...);

protected:
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_STL_STRING_H__