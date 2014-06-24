#include "CBstServer.h"

CBstServer::~CBstServer()
{
}

void CBstServer::start()
{
	socket_type socket(new ip::tcp::socket(io_));
	acceptor_.async_accept(*socket, boost::bind(&CBstServer::connect_handler, this, socket, boost::asio::placeholders::error));
}

void CBstServer::connect_handler(socket_type socket, boost::system::error_code error)
{
	//cout<< "connect_handler error message:"<< error.message()<< endl;
	
	if(error) return;

	cout<< "connected ip:"<< socket->remote_endpoint()<< endl;

	buf_type buf(new std::string("asdf"));
	socket->async_write_some(boost::asio::buffer(*buf), boost::bind(&CBstServer::write_handle, this, socket, buf, boost::asio::placeholders::error));

	start();
}

void CBstServer::write_handle(socket_type socket, buf_type buf, boost::system::error_code error)
{
	//cout<< "write_handle error mesage:"<< error.message()<<endl;
    
	if(error) {
		cout<< "Failure."<< endl;
    }
	else {
		cout<< "Success. send message is "<< *(buf)<<endl;
    }
}