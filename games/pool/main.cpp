#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

int main() {
  sf::RenderWindow window(sf::VideoMode(1024, 1024), "Pool");

  sf::CircleShape circle_shape;
  circle_shape.setRadius(30);
  circle_shape.setFillColor(sf::Color::Green);

  sf::Font font;
  font.loadFromFile("games/pool/arial.ttf");

  sf::Text text_fps;

  text_fps.setFont(font);
  text_fps.setPosition({.5, .5});
  text_fps.setString("");
  text_fps.setFillColor(sf::Color::Red);
  text_fps.setCharacterSize(50);

  sf::View fixed = window.getView();  // Create a fixed view

  std::map<sf::Keyboard::Key, bool> keys_pressed;

  // main loop
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (sf::Event::Closed == event.type) {
        window.close();
      } else if (sf::Event::KeyPressed == event.type) {
        keys_pressed[event.key.code] = true;
      } else if (sf::Event::KeyReleased == event.type) {
        keys_pressed[event.key.code] = false;
      }
    }

    text_fps.setString("static string");

    const auto mouse_position = sf::Mouse::getPosition(window);
    const auto local_pos = window.mapPixelToCoords(mouse_position);

    circle_shape.setPosition(local_pos - sf::Vector2f{circle_shape.getRadius(), circle_shape.getRadius()});

    window.clear();
    window.setView(fixed);
    window.draw(circle_shape);
    window.draw(text_fps);
    window.display();
  }

  return 0;
}
