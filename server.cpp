#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <string>
#include "specific-messages.h"

using boost::asio::ip::udp;


std::vector<boost::asio::ip::address> clients_ip;
std::vector<unsigned short> clients_port;


void sendMessage(const boost::asio::ip::address& _ip, const unsigned short& _port, const std::string& _message, udp::socket& _socket)
{
  udp::endpoint send_endpoint(_ip, _port);
  _socket.send_to(boost::asio::buffer(_message), send_endpoint);
}

void broadcastMessage(std::string& message, udp::socket& socket)
{
  //broadcast received message to clients.
  for (int i = 0; i < clients_port.size(); ++i)
    sendMessage(clients_ip[i], clients_port[i], message, socket);
}

template <typename T, typename Y>
int getIndexFrpmVector(const std::vector<T>& v, const Y& data)
{
    auto it = std::find(v.begin(), v.end(), data);

    if (it != v.end()) // If element was found
        return (it - v.begin());  //return index of that data
    else
        return -1; // If the element is not
}

void print_list_of_clients()
{
  //print list of clients connected.
  std::cout << "\n\n-----------------------------------------------------------------\n";
  std::cout << "online clients are:" << std::endl;
  for (int i = 0; i < clients_port.size(); ++i)
    std::cout << clients_ip[i] << ":" << clients_port[i] << std::endl;
  std::cout << "-----------------------------------------------------------------\n\n\n";
}
void receiveData(udp::socket& socket)
{
    while (true)
    {
        char receive_data[1024];
        udp::endpoint receive_endpoint;
        size_t receive_length = socket.receive_from(boost::asio::buffer(receive_data), receive_endpoint);

        const unsigned short c_port = receive_endpoint.port();
        const boost::asio::ip::address c_ip = receive_endpoint.address();

        if(std::string(receive_data, receive_length) == CLIENT_REQUEST_TO_JOIN_CHAT)
        {
          std::cout << "client request to join chat." << std::endl;
          std::string client_connected = "client (" + c_ip.to_string()+ ":"+ std::to_string(c_port) + ") has been connected.\n";
          broadcastMessage(client_connected,socket);

          clients_port.push_back(c_port);
          clients_ip.push_back(c_ip);

          print_list_of_clients();

          //send response to connected client.
          std::string _connected_message = SERVER_CONNECTED_MESSAGE;
          sendMessage(c_ip, c_port, _connected_message , socket);

        }
        else if(std::string(receive_data, receive_length) == CLIENT_REQUEST_TO_CLOSE_CHAT)
        {
          const int disconnected_ip_index= getIndexFrpmVector(clients_ip, c_ip);
          const int disconnected_port_index = getIndexFrpmVector(clients_port, c_port);
          std::cout << "client ip index: " << disconnected_ip_index << " has left.\n";
          std::cout << "client port index: " << disconnected_port_index << " has left.\n";


          //delete disconnected client ip and port from vectors
          if(disconnected_ip_index > -1) clients_ip.erase(clients_ip.begin() + disconnected_ip_index);
          if(disconnected_port_index > -1) clients_port.erase(clients_port.begin() + disconnected_port_index);

          std::string client_disconnected = "client (" + c_ip.to_string()+ ":"+ std::to_string(c_port) + ") has been disconnected.\n";
          broadcastMessage(client_disconnected,socket);
          print_list_of_clients();
        }
        else
        {
          if(std::find(clients_port.begin(), clients_port.end(), c_port)!=clients_port.end()) //CLIENT PORT EXISTS
          {
            //setup message before forward to clients
            std::string broadcast_message =
            c_ip.to_string() + ":"
            + std::to_string(c_port) + " said:"
            + std::string(receive_data, receive_length) + '\n';

            broadcastMessage(broadcast_message,socket);
          }
          else
            std::cout << "[UNKNOWN-CLIENT] trying to tell something.. information: " << c_ip.to_string() << ":" << std::to_string(c_port) << " message-content=" << std::string(receive_data, receive_length) << std::endl;
        }
    }
}

int main()
{
    try
    {
        boost::asio::io_context io_context;

        // Create a socket for receiving and sending data
        udp::socket socket(io_context, udp::endpoint(udp::v4(), DEFAULT_SERVER_PORT));

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
