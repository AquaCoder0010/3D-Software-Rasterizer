#include <SFML/Graphics.hpp>

struct Triangle{
  
};

int main() {
  sf::RenderWindow window(sf::VideoMode(600, 400), "title");


  sf::Event event;
  while (window.isOpen() == true) {
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    window.clear();
    window.display();
  }
}