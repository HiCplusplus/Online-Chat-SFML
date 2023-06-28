#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <string>

using boost::asio::ip::udp;

int port=8888;


// std::vector<boost::asio::ip::address> clients_ip;
std::vector<unsigned short> clients_port;

void receiveData(udp::socket& socket)
{
    while (true)
    {
        char receive_data[1024];
        udp::endpoint receive_endpoint;
        size_t receive_length = socket.receive_from(boost::asio::buffer(receive_data), receive_endpoint);

        unsigned short c_port = receive_endpoint.port();
        boost::asio::ip::address c_ip = receive_endpoint.address();

        if(std::find(clients_port.begin(), clients_port.end(), c_port)==clients_port.end())
        {
          std::cout << "client port not found lets add him to chat.\n";
          clients_port.push_back(c_port);
        }


        //setup message before forward to clients
        std::string broadcast_message =
        c_ip.to_string() + ":"
        + std::to_string(c_port) + " said:"
        + std::string(receive_data, receive_length) + '\n';




        //broadcast received message to clients.
        /*
            clients on local ip are same,
            so didnt do anything with clients_ip
            have to map them together..
        */
        for (int i = 0; i < clients_port.size(); ++i)
        {
          udp::endpoint send_endpoint(boost::asio::ip::address::from_string("127.0.0.1"), clients_port[i]);
          socket.send_to(boost::asio::buffer(broadcast_message), send_endpoint);
        }

    }
}


int main()
{
    try
    {
      std::cout << "enter server listen port: ";
      std::cin >> port;

        boost::asio::io_context io_context;

        // Create a socket for receiving and sending data
        udp::socket socket(io_context, udp::endpoint(udp::v4(), port));

        // Start receiving thread
        boost::thread receive_thread(boost::bind(receiveData, boost::ref(socket)));

        // Start sending thread
        // boost::thread send_thread(boost::bind(sendData, boost::ref(socket)));

        // Wait for threads to finish
        receive_thread.join();
        // send_thread.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
