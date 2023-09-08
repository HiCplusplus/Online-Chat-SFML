#include "specific-messages.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
// #include <thread>

//pick random number
#include <cstdlib>
#include <ctime>

//boost socket & thread
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::udp;


bool inChat=false; //to manage draw chat or get-ip-port server
std::string server_ip="127.0.0.1";
unsigned short server_port=8888;
std::string entered_server_info;
std::string connection_status_text = "disconnected.";
enum ConnectionStatus
{
  CONNECTED,
  TRYING,
  DISCONNECTED
};

ConnectionStatus connection_status=DISCONNECTED;

//chat settings
std::string draft_text, main_chat_text;
int main_chat_max_height = 25;
int wrap_text = 50;

//set default ports
// int receiver_port=DEFAULT_SERVER_PORT; //server's port
int port=DEFAULT_CLIENT_PORT; //this client's port


//setup window and UI elements
sf::Vector2f viewSize(600,600);
sf::VideoMode vm(viewSize.x, viewSize.y);
sf::RenderWindow window(vm, "Chat", sf::Style::Default);


sf::Font font;
sf::Text text,chat;

sf::Text tserver_info; //client will enter these. tnickname
sf::Text label_press_to_connect, connection_info; //labels



void receiveData(udp::socket& socket)
{
    while (true)
    {
        char receive_data[1024];
        udp::endpoint receive_endpoint;
        size_t receive_length = socket.receive_from(boost::asio::buffer(receive_data), receive_endpoint);
        // std::cout << "Received: " << std::string(receive_data, receive_length) << std::endl;

        int num_lines = std::count( main_chat_text.begin(), main_chat_text.end(), '\n' ) ;
        if(num_lines>main_chat_max_height)
          main_chat_text="";


        main_chat_text += (std::string(receive_data, receive_length));
    }
}
void sendData(udp::socket& socket,std::string send_message)
{
    udp::endpoint send_endpoint(boost::asio::ip::address::from_string(server_ip), server_port);
    socket.send_to(boost::asio::buffer(send_message), send_endpoint);
}

//init Texts
void init()
{

  //init chat
  if(!font.loadFromFile("arial.ttf"))
    std::cout << "error while loading font. arial.ttf not found..\n";

  text.setFont(font);
  text.setCharacterSize(17);
  text.setString("");
  text.setFillColor(sf::Color::White);
  text.setStyle(sf::Text::Bold);
  text.setPosition(25,viewSize.y-70);

  chat.setFont(font);
  chat.setString("");
  chat.setCharacterSize(14);
  chat.setFillColor(sf::Color::White);
  chat.setStyle(sf::Text::Bold);
  chat.setPosition(25,0);


  //init hud
  connection_info.setFont(font);
  connection_info.setString("");
  connection_info.setCharacterSize(12);
  connection_info.setFillColor(sf::Color::Red);
  connection_info.setStyle(sf::Text::Bold);
  connection_info.setPosition(25,viewSize.y-100);



  //init form
  tserver_info.setFont(font);
  tserver_info.setString("");
  tserver_info.setCharacterSize(20);
  tserver_info.setFillColor(sf::Color::White);
  tserver_info.setStyle(sf::Text::Bold);
  tserver_info.setPosition((viewSize.x/2)-70,viewSize.y/2);

  // tnickname.setFont(font);

  label_press_to_connect.setFont(font);
  label_press_to_connect.setString("Press Enter To Connect");
  label_press_to_connect.setCharacterSize(20);
  label_press_to_connect.setFillColor(sf::Color::Red);
  label_press_to_connect.setStyle(sf::Text::Bold);
  label_press_to_connect.setPosition(viewSize.x/2-130,viewSize.y/4);
}


void draw_hud()
{
  switch(connection_status)
  {
    case CONNECTED:
    {
      connection_status_text = "connected to " + server_ip + std::to_string(server_port)+".";
      connection_info.setFillColor(sf::Color::Green);
    }break;

    case TRYING:
    {
      connection_status_text = "trying to connect...";
      connection_info.setFillColor(sf::Color::Yellow);
    }break;

    case DISCONNECTED:
    {
      connection_status_text = "disconnected.";
      connection_info.setFillColor(sf::Color::Red);
    }break;

  }
  connection_info.setString(connection_status_text);
  window.draw(connection_info);
}

void draw_chat()
{
  text.setString(draft_text);
  chat.setString(main_chat_text);

  window.draw(text);
  window.draw(chat);
}

void draw_form()
{
  tserver_info.setString(entered_server_info);

  window.draw(tserver_info);
  window.draw(label_press_to_connect);
}


void disconnect_connection()
{
  inChat=false;
  connection_status=DISCONNECTED;
  // socket.close();
}

//window events
void closeSafe(udp::socket* socket)
{
  std::cout << "safe close application.. \n";
  sendData(*socket, "~iD`us~");
  window.close();
}

void init_connection(udp::socket& _socket)
{
  sendData(_socket,CLIENT_REQUEST_TO_JOIN_CHAT);
  inChat=true;
  connection_status=CONNECTED;
}

unsigned short convertStringToUshort(std::string& str)
{
  std::istringstream iss(str);
  unsigned short result;
  iss >> result;
  return result;
}

int findIndexOfChar(const std::string& str, const char& c)
{
  int index = str.find(c);
  if (index != std::string::npos)
    return index;
  else
    return -1;
}

void validateServerInfo()
{
    //will seperate ip and port and remvoe invalid charecters
    int index = findIndexOfChar(entered_server_info,':');
    if(index != -1)
    {
      std::string sip = entered_server_info.substr(0,index);
      std::string sport = entered_server_info.substr(index+1, entered_server_info.length());
      std::cout << "ip=" << sip << " port=" <<sport <<std::endl;
      server_ip = sip;
      server_port = convertStringToUshort(sport);
      entered_server_info = "";
    }
    else
      std::cout << "[invalid port] enter ip and port like 0.0.0.0:1234" << std::endl;
}



void keyboardKeyPressed(sf::Event event,udp::socket* socket)
{
  if(event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return)
  {
    if(inChat)
    {
      if(findIndexOfChar(draft_text,'/') == -1) //normal text
      {
        sendData(*socket,draft_text);
        draft_text = "";
      }
      else //command text
      {
        std::cout << "draft=" << draft_text.length() <<"?\n";
        if(draft_text == "/quit")
        {
            closeSafe(socket);
            std::cout << "exit..";
        }
        else if (draft_text == "/disconnect")
        {
            disconnect_connection();
        }
        else
        {
            std::cout <<"unknwon command." << std::endl;
        }
        draft_text = "";
      }
    }
    else //is in form
    {
      connection_status=TRYING;
      validateServerInfo();
      init_connection(*socket);
    }
  }
  if(event.key.code == sf::Keyboard::BackSpace)
  {
    if(inChat)
    {
      draft_text = draft_text.substr(0, draft_text.length()-2);
    }
    else //is in form
    {
      entered_server_info = entered_server_info.substr(0, entered_server_info.length()-2);
    }
  }
}
void keyboardTextEntered(sf::Event event)
{
  if (event.text.unicode < 128)
  {

    if(inChat)
    {
      //do wrap text when its too long
      if(draft_text.length() %wrap_text == 0)
      draft_text+="\n";

      draft_text += static_cast<char>(event.text.unicode);
    }
    else //is in form
    {
      entered_server_info += static_cast<char>(event.text.unicode);
    }
  }
}


void inputs(udp::socket* socket)
{
	sf::Event event;
	while(window.pollEvent(event))
	{
    switch(event.type)
    {
      //window
      case sf::Event::Closed:
        closeSafe(socket);
        break;
      //keyboard
      case sf::Event::KeyPressed:
        keyboardKeyPressed(event,socket);
        break;
      case sf::Event::TextEntered:
        keyboardTextEntered(event);
        break;
    }
  }
}

bool isPortBusy(int _port=DEFAULT_CLIENT_PORT)
{
  //lets check port is in use or not
  boost::asio::io_service ioService;
  boost::asio::ip::tcp::socket socket(ioService);
  try
  {
      boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), _port);
      socket.open(endpoint.protocol());
      socket.bind(endpoint);
      std::cout << "Port is available port = " << _port << std::endl;
      port=_port; //set current available port to client's port
      return false;
  }
  catch (std::exception& e)
  {
      std::string errorMsg = e.what();
      if (errorMsg.find("bind: Address already in use") != std::string::npos)
      {
          std::cerr << "Port is already in use port = " << _port << std::endl;
          return true;
      }
      else
      {
          std::cerr << "Exception: " << errorMsg << std::endl;
          return true;
      }
  }
}


int genrate_random_number(int min=2500, int max=64000)
{
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  int randomNumber = std::rand() % (max - min + 1) + min;
  return randomNumber;
}

void pickPort()
{
  bool result=false; //status port busy false=not true=is busy
  do
  {
    result = isPortBusy(genrate_random_number(3000,64000));
  } while(result);
}


int main()
{
  try
  {
      pickPort(); //will pick a random port number for client's port

      //do socket bind
      boost::asio::io_context io_context;
      udp::socket socket(io_context, udp::endpoint(udp::v4(), port));
      boost::thread receive_thread(boost::bind(receiveData, boost::ref(socket)));
      // boost::thread send_thread(boost::bind(sendData, boost::ref(socket)));

      //window settings
      window.setFramerateLimit(60);
      window.setKeyRepeatEnabled(true);


    	init();
    	while(window.isOpen())
    	{
        inputs(&socket);



        //render things
        window.clear(sf::Color::Black);

        if(inChat)
          draw_chat(); //draw_chat and display chat elements
        else
          draw_form();

        draw_hud();
        window.display();
    	}

      receive_thread.join();
      // send_thread.join();
  }
  catch (std::exception& e)
  {
    std::string errorMsg = e.what();
    if (errorMsg.find("bind: Address already in use") != std::string::npos)
        std::cerr << "Socket bind failed: Port Address already in." << std::endl;
    else
        std::cerr << "Exception: " << errorMsg << std::endl;
  }
  return 0;
}
