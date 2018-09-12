#include "stdafx.h"
#define REDIS_LBR                       "\r\n"
#define REDIS_STATUS_REPLY_OK           "OK"
#define REDIS_PREFIX_STATUS_REPLY_ERROR "-ERR "
#define REDIS_PREFIX_STATUS_REPLY_ERR_C '-'
#define REDIS_PREFIX_STATUS_REPLY_VALUE '+'
#define REDIS_PREFIX_SINGLE_BULK_REPLY  '$'
#define REDIS_PREFIX_MULTI_BULK_REPLY   '*'
#define REDIS_PREFIX_INT_REPLY          ':'
#define REDIS_WHITESPACE                " \f\n\r\t\v"

struct key
{
	explicit key(const std::string & name)
		: name(name)
	{
	}

	std::string name;
};

class makecmd
{
public:
	explicit makecmd(const std::string & cmd_name)
	{
		append(cmd_name);
		//if (!finalize)
		//  buffer_ << " ";
	}

	const std::string & key_name() const
	{
		if(!key_name_)
			throw std::runtime_error("No key defined!");
		return *key_name_;
	}

	inline makecmd & operator<<(const key & datum)
	{
		if(key_name_)
			throw std::runtime_error("You could not add a second key");
		else
			key_name_ = datum.name;
		append(datum.name);
		return *this;
	}

	inline makecmd & operator<<(const std::string & datum)
	{
		append(datum);
		return *this;
	}

	template <typename T>
	makecmd & operator<<(T const & datum)
	{
		append( boost::lexical_cast<std::string>(datum) );
		return *this;
	}

	makecmd & operator<<(const std::vector<std::string> & data)
	{
		lines_.insert( lines_.end(), data.begin(), data.end() );
		return *this;
	}

	template <typename T>
	makecmd & operator<<(const std::vector<T> & data)
	{
		size_t n = data.size();
		for (size_t i = 0; i < n; ++i)
		{
			append( boost::lexical_cast<std::string>( data[i] ) );
			//if (i < n - 1)
			//  buffer_ << " ";
		}
		return *this;
	}

	operator std::string () const
	{
		std::ostringstream oss;
		size_t n = lines_.size();
		oss << REDIS_PREFIX_MULTI_BULK_REPLY << n << REDIS_LBR;

		for (size_t i = 0; i < n; ++i)
		{
			const std::string & param = lines_[i];
			oss << REDIS_PREFIX_SINGLE_BULK_REPLY << param.size() << REDIS_LBR;
			oss << param << REDIS_LBR;
		}

		return oss.str();
	}

private:
	void append(const std::string & param)
	{
		lines_.push_back(param);
	}

	std::vector<std::string> lines_;
	boost::optional<std::string> key_name_;
};

typedef std::string string_type;
typedef std::vector<string_type> string_vector;
typedef std::pair<string_type, string_type> string_pair;
typedef std::vector<string_pair> string_pair_vector;
typedef std::pair<string_type, double> string_score_pair;
typedef std::vector<string_score_pair> string_score_vector;
typedef std::set<string_type> string_set;

typedef long int_type;

class RedisSender{
public:
	template <class T>
	static string hset( const string_type & key, const string_type & field, const T & value )
	{
		return makecmd("HSET") << key << field << value;
	}

	static string_type hget( const string_type & key, const string_type & field )
	{
		return makecmd("HGET") << key << field;
	}
};