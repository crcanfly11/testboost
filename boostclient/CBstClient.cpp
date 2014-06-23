#include "CBstClient.h"

CBstClient::~CBstClient()
{}

void CBstClient::start()
{
	ip::tcp::endpoint ep(ip::address_v4::from_string(boost::ref(address_)), port_);

	socket_.async_connect(ep, boost::bind(&CBstClient::connect_handler, this, _1));
}

void CBstClient::connect_handler(boost::system::error_code error)
{
	cout<< "connect_handler error message:"<< error.message()<< endl;

	if(error) return;

	strbuf_tyep buf(new std::vector<char>(100,0));
	socket_.async_read_some(boost::asio::buffer(*buf), boost::bind(&CBstClient::read_handler, this, buf, _1));
}

void CBstClient::read_handler(strbuf_tyep buf, boost::system::error_code error)
{
	cout<< "read_handler error message:"<< error.message()<< endl;

	if(error) return;

	cout<< "the buf is:"<< &(*buf)[0]<< endl;
}