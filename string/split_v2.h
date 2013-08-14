template <class T>
void finish_string(T *str)
{
	*str = '\0';
}

template <>
void finish_string(const char* str)
{
	*const_cast<char*>(str) = '\0';
}


template <class char_ptr>
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
				finish_string(_str);
				_str++;
			}
			return begin;
		}
	private:
		char_ptr _str;
		char _sep;
};

typedef string_split_impl<char*> string_split;
typedef string_split_impl<const char*> const_string_split;
