/*
   ** 实现的功能和strtok基本一致
   ** 每次返回下一个子字符串的起始位置，对于char类型，直接将分隔符替换为'\0';
   ** 对于const char类型，通过end参数返回结束位置
   **
   ** 提出的问题：
   ** 1. 可使用 strchr()，一些编译器可能提供SIMD加速。 
   ** 2. 考虑性能的话，其实只用strchr()，然后用 const char* begin, send 表示字符串进行处理，可能更快。null-terminated string有时反而不能处理一些情况，例如含有\u0000的字符串。 
   ** 3. 写进 const char* 在一些系统上可能会崩溃。从语意来说，这是in-situ修改，建议只支持char*，使用者有需要可以用复制至自己的buffer或strdup()。
   **
   */
template <class char_ptr, bool _const>
class string_split_impl
{
	public:
		string_split_impl(char_ptr str, char sep) : _str(str), _sep(sep){}
		char_ptr parse(char_ptr* end = NULL)
		{
			if(_str == NULL || *_str == '\0')
				return NULL;
			char_ptr begin = _str;
			while(*_str != _sep && *_str != '\0')
				_str++;
			if(end)
			{
				*end = _str;
			}

			if(*_str == _sep)
			{
				if(!_const)
					//*const_cast<char*>(_str) = '\0';
					*_str = '\0';
				_str++;
			}
			return begin;
		}
	private:
		char_ptr _str;
		char _sep;
};

typedef string_split_impl<char*, false> string_split;
typedef string_split_impl<const char*, true> const_string_split;

