#include <iostream>

#include <boost\bind.hpp>
#include <boost\asio.hpp>
#include <boost\array.hpp>
 
using namespace boost::asio;
 
int i=0;

void HandleTcpSend(const boost::system::error_code& err, 
    size_t bytes_transferred);

io_service iosev;
ip::tcp::socket socket(iosev);

int main(int argc, char* argv[])
{
    // 所有asio类都需要io_service对象
    
    ip::tcp::acceptor acceptor(iosev, 
        ip::tcp::endpoint(ip::tcp::v4(), 1000));

    //for(;;)
    //{
        // socket对象
        //ip::tcp::socket socket(iosev);
        // 等待直到客户端连接进来
        acceptor.accept(socket);
        // 显示连接进来的客户端
        std::cout << socket.remote_endpoint().address() << std::endl;
        // 向客户端发送hello world!
        boost::system::error_code ec;
        //socket.write_some(buffer("hello world!"), ec);
 

		boost::asio::async_write(
			socket,
			boost::asio::buffer( buffer[i].data_.get(), buffer[i].length_ ),
			boost::asio::transfer_at_least(buffer[i].length_),
			boost::bind(&HandleTcpSend, 
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred         
			)
		);

        // 如果出错，打印出错信息
        if(ec)
        {
            std::cout << 
                boost::system::system_error(ec).what() << std::endl;
            //break;
        }
        // 与当前客户交互完成后循环继续等待下一客户连接
    //}

    return 0;
}

void HandleTcpSend(const boost::system::error_code& err, 
    size_t bytes_transferred)
{
    i++;
    boost::asio::async_write(
        socket,
        boost::asio::buffer( buffer[i].data_.get(), buffer[i].length_ ),
        boost::asio::transfer_at_least(buffer[i].length_),
        boost::bind( 
            &HandleTcpSend, 
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred         
        )
    );
}