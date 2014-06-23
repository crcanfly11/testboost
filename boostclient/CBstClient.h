#ifndef _CBstClient_h_
#define _CBstClient_h_

#include <ostream>

#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\shared_ptr.hpp>

using namespace std;
using namespace boost::asio;

class CBstClient
{
	typedef boost::shared_ptr<std::vector<char>> strbuf_tyep;

	io_service& io_;
	ip::tcp::socket socket_;
	std::string address_;
	unsigned int port_;
public:
	CBstClient(io_service& io, const char* address, unsigned int port) : io_(io), address_(address), port_(port), socket_(io) 
	{
		start();
	};
	~CBstClient();

	void start();

	void connect_handler(boost::system::error_code error);
	void read_handler(strbuf_tyep buf, boost::system::error_code error);
};

#endif