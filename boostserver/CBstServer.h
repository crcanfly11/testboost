#ifndef _CBstServer_h_
#define _CBstServer_h_

#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\shared_ptr.hpp>

#include <ostream>

using namespace std;
using namespace boost::asio;
//asdfjcr
class CBstServer
{
	typedef boost::shared_ptr<ip::tcp::socket> socket_type;
	typedef boost::shared_ptr<std::string> buf_type;

	io_service& io_;
	ip::tcp::acceptor acceptor_;

public:
	CBstServer(io_service& io, unsigned int port) : io_(io), acceptor_(io,ip::tcp::endpoint(ip::address_v4(),port))
	{
		start();
	};

	~CBstServer();

	void start();

	void connect_handler(socket_type socket, boost::system::error_code error);
	void write_handle(socket_type socket, buf_type buf, boost::system::error_code error);
};

#endif
