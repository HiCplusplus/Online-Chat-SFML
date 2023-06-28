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

#define DEFAULT_SERVER_PORT 8888
#define DEFAULT_CLIENT_PORT 7778

using boost::asio::ip::udp;


//chat settings
std::string draft_text, main_chat_text;
int main_chat_max_height = 25;
int wrap_text = 50;

//set default ports
int receiver_port=DEFAULT_SERVER_PORT; //server's port
int port=DEFAULT_CLIENT_PORT; //this client's port

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
    udp::endpoint send_endpoint(boost::asio::ip::address::from_string("127.0.0.1"), receiver_port);
    socket.send_to(boost::asio::buffer(send_message), send_endpoint);
}

//setup window and UI elements
sf::Vector2f viewSize(600,600);
sf::VideoMode vm(viewSize.x, viewSize.y);
sf::RenderWindow window(vm, "Chat", sf::Style::Default);
sf::Font font;
sf::Text text,chat;

//init Texts
void init()
{
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
}

void draw()
{
  //set chat values to elements
  text.setString(draft_text);
  chat.setString(main_chat_text);

  window.clear(sf::Color::Black);
  window.draw(text);
  window.draw(chat);
  window.display();
}

//window events
void closeSafe(sf::Event eve)
{
  window.close();
}
void keyboardKeyPressed(sf::Event event,udp::socket* socket)
{
  if(event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return)
  {
      sendData(*socket,draft_text);
      draft_text = "";
  }
  if(event.key.code == sf::Keyboard::BackSpace)
    draft_text = draft_text.substr(0, draft_text.length()-2);
}
void keyboardTextEntered(sf::Event event)
{
  if (event.text.unicode < 128)
  {
    //do wrap text when its too long
    if(draft_text.length() %wrap_text == 0)
      draft_text+="\n";

    draft_text += static_cast<char>(event.text.unicode);
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
        closeSafe(event);
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
    		draw(); //draw and display chat elements
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
