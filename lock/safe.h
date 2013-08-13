#include <sys/types.h>
#include <pthread.h>

#ifndef __SAFE_H__
#define __SAFE_H__

#define LOCK(Mutex) JLockHelper __lock__(Mutex)

class JLockHelper;

class JAbsXMutex
{
	public:
		virtual ~JAbsXMutex(){}
		virtual void Lock() const = 0;
		virtual void Unlock() const = 0;
};

// X锁(互斥锁)类
class JXMutex : public JAbsXMutex
{
	public:
		JXMutex()
		{
			pthread_mutex_init (&m_Mutex, NULL);
		}

		virtual ~JXMutex()
		{
			pthread_mutex_destroy(&m_Mutex);
		}

	private:
		JXMutex(const JXMutex&);	// 禁止锁拷贝
		const JXMutex& operator =(const JXMutex &);	// 禁止锁赋值
		
	public:
		void Lock() const
		{
			pthread_mutex_lock (&m_Mutex);
		}
		void Unlock() const
		{
			pthread_mutex_unlock(&m_Mutex);
		}

	public:
		mutable pthread_mutex_t m_Mutex;
};

// JSafe 基于各类锁的安全类模版
//
template <class _Tp, class _Mutex = JXMutex>
class JSafe
{
	public:
		typedef _Tp value_type;
		typedef _Mutex lock_type;
		JSafe(void){}
		~JSafe(void){}
		explicit JSafe(const _Tp& other)
		{
			m_Data = other;
		}
		JSafe(const JSafe& other)
		{
			LOCK(other);
			m_Data = other.m_Data;
		}

		// 定义强制类型转换，注意已锁定时不能使用；虽然使用起来方便，但实际上都可以由获取值的Value函数取代
		operator const _Tp() const
		{
			LOCK(m_Mutex);
			return m_Data;
		}

		JSafe& operator =(const JSafe& other)
		{
			LOCK(other);
			{
				LOCK(m_Mutex);
				m_Data = other.m_Data;
			}
			return *this;
		}

		void Value(const _Tp& other)
		{
			LOCK(m_Mutex);
			m_Data = other;
		}

		const _Tp Value(void) const
		{
			LOCK(m_Mutex);
			return m_Data;
		}

		void Lock(void) const
		{
			m_Mutex.Lock();
		}
		void Unlock(void) const
		{
			m_Mutex.Unlock();
		}

		// 加锁后方可安全使用此函数得到的资源引用，如未加锁直接调用可能造成相应的安全问题
		_Tp& Data(void)
		{
			return m_Data;
		}

	protected:
		_Tp m_Data;
		mutable _Mutex m_Mutex;
		friend class JLockHelper;
};

// 适用于有Lock和Unlock成员函数的安全类或锁类的辅助加锁对象，可以做到异常安全的解锁
// 说明：在创建类时加锁，析构时解锁
class JLockHelper
{
	public:
		JLockHelper(const JAbsXMutex& xMutex) : m_xMutex(xMutex)
		{
			m_xMutex.Lock();
		}

		template <class _Tp, class _Mutex>
			JLockHelper(const JSafe<_Tp, _Mutex>& S) : m_xMutex(S.m_Mutex)
			{
				m_xMutex.Lock();
			}
		~JLockHelper()
		{
			m_xMutex.Unlock();
		}

	private:
		const JAbsXMutex& m_xMutex;
};


template <class _Tp>
class JSafeNum : public JSafe<_Tp>
{
	public:
		typedef JSafe<_Tp> safe_type;
		explicit JSafeNum(const _Tp& _Value = 0) : JSafe<_Tp>(_Value) {}
		JSafeNum& operator ++()
		{
			LOCK(safe_type::m_Mutex);
			safe_type::m_Data++;
			return *this;
		}

		JSafeNum operator ++(int)
		{
			LOCK(safe_type::m_Mutex);
			JSafeNum tmp(safe_type::m_Data++);
			return tmp;
		}

		JSafeNum& operator +=(const _Tp& other)
		{
			LOCK(safe_type::m_Mutex);
			safe_type::m_Data += other;
			return *this;
		}

		JSafeNum& operator --(void)
		{
			LOCK(safe_type::m_Mutex);
			safe_type::m_Data--;
			return *this;
		}

		JSafeNum operator --(int)
		{
			LOCK(safe_type::m_Mutex);
			JSafeNum tmp(safe_type::m_Data--);
			return tmp;
		}

		JSafeNum& operator -=(const _Tp& other)
		{
			LOCK(safe_type::m_Mutex);
			safe_type::m_Data -= other;
			return *this;
		}

		JSafeNum& operator =(const _Tp& other)
		{
			LOCK(safe_type::m_Mutex);
			safe_type::m_Data = other;
			return *this;
		}
};

typedef JSafe<bool> JSafeBool;
typedef JSafeNum<int> JSafeInt;
typedef JSafeNum<long> JSafeLong;
typedef JSafeNum<u_int> JSafeUint;

// 安全容器，适用于stl容器类
template <class _Con>
class JSafeCon : public JSafe<_Con>
{
	public:
		typedef JSafe<_Con> _Base;
		typedef JSafeCon<_Con> _Self;
		typedef _Con con_type;
		typedef typename con_type::iterator iterator;
		explicit JSafeCon(void) : _Base() {}
		bool empty()
		{
			LOCK(_Base::m_Mutex);
			return _Base::m_Data.empty();
		}

		size_t size()
		{
			LOCK(_Base::m_Mutex);
			return _Base::m_Data.size();
		}

		void swap(con_type& Seq)
		{
			LOCK(_Base::m_Mutex);
			_Base::m_Data.swap(Seq);
		}

		// 以下2个函数及其使用均需在同步的保护下
		iterator begin()
		{
			return _Base::m_Data.begin();
		}
		iterator end()
		{
			return _Base::m_Data.end();
		}
};

// 安全序列 （适用于stl的序列容器类如vetor, list,...)
template <class _Sequence>
class JSafeSeq : public JSafeCon<_Sequence>
{
	public:
		typedef JSafeCon<_Sequence> _Base;
		typedef JSafeSeq<_Sequence> _Self;
		typedef _Sequence seq_type;
		typedef typename seq_type::iterator iterator;
		typedef typename seq_type::value_type value_type;

		explicit JSafeSeq(void) : _Base() {}
		void push_back(const value_type& Val)
		{
			LOCK(_Base::m_Mutex);
			_Base::m_Data.push_back(Val);
		}
};

// 适用于stl前向插入序列如deque等
template <class _FSeq>
class JSafeFSeq : public JSafeSeq<_FSeq>
{
	public:
		typedef JSafeSeq<_FSeq> _Base;
		typedef JSafeFSeq<_FSeq> _Self;
		typedef typename _Base::seq_type seq_type;
		typedef typename seq_type::iterator iterator;
		typedef typename seq_type::value_type value_type;

		explicit JSafeFSeq(void) : _Base() {}

		bool pop_front(value_type& Val)
		{
			LOCK(_Base::m_Mutex);
			if(_Base::m_Data.empty())
			{
				return false;
			}
			else
			{
				Val = *(_Base::m_Data.begin());
				_Base::m_Data.pop_front();
				return true;
			}
		}
};

// 安全map类，适用于stl::map
template <class _Map>
class JSafeMap : public JSafeCon<_Map>
{
	public:
		typedef JSafeCon<_Map> _Base;
		typedef JSafeSeq<_Map> _Self;
		typedef _Map map_type;
		typedef typename map_type::iterator  iterator;
		typedef typename map_type::const_iterator const_iterator;
		typedef typename map_type::value_type value_type;
		typedef typename value_type::first_type key_type;
		typedef typename value_type::second_type data_type;

		explicit JSafeMap(void) : _Base(){}

		void insert(const value_type& Val)
		{
			LOCK(_Base::m_Mutex);
			_Base::m_Data.insert(Val);
		}

		bool find(const key_type& Key) const
		{
			LOCK(_Base::m_Mutex);
			return _Base::m_Data.find(Key) != _Base::m_Data.end();
		}

		bool find(const key_type& Key, data_type& Val) const
		{
			LOCK(_Base::m_Mutex);
			const_iterator it = _Base::m_Data.find(Key);
			if(_Base::m_Data.end() != it)
			{
				Val = it->second;
				return true;
			}
			return false;
		}

		void erase(const key_type& Key)
		{
			LOCK(_Base::m_Mutex);
			_Base::m_Data.erase(Key);
		}

		data_type get(const key_type& Key, const data_type& Default) const
		{
			LOCK(_Base::m_Mutex);
			const_iterator it = _Base::m_Data.find(Key);
			if(_Base::m_Data.end() == it)
			{
				return Default;
			}
			else
			{
				return it->second();
			}
		}

		bool find_erase(const key_type& Key, data_type& Val)
		{
			LOCK(_Base::m_Mutex);
			iterator it = _Base::m_Data.find(Key);
			if(_Base::m_Data.end() != it)
			{
				Val = it->second();
				_Base::m_Data.erase(it);
				return true;
			}
			return false;
		}

		void set(const key_type& Key, const data_type& Val)
		{
			LOCK(_Base::m_Mutex);
			_Base::m_Data[Key] = Val;
		}
};

#endif

