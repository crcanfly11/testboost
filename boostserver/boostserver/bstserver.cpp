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
    // ����asio�඼��Ҫio_service����
    
    ip::tcp::acceptor acceptor(iosev, 
        ip::tcp::endpoint(ip::tcp::v4(), 1000));

    //for(;;)
    //{
        // socket����
        //ip::tcp::socket socket(iosev);
        // �ȴ�ֱ���ͻ������ӽ���
        acceptor.accept(socket);
        // ��ʾ���ӽ����Ŀͻ���
        std::cout << socket.remote_endpoint().address() << std::endl;
        // ��ͻ��˷���hello world!
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

        // ���������ӡ������Ϣ
        if(ec)
        {
            std::cout << 
                boost::system::system_error(ec).what() << std::endl;
            //break;
        }
        // �뵱ǰ�ͻ�������ɺ�ѭ�������ȴ���һ�ͻ�����
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