#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>

int main() {
  sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
  window.setMouseCursorVisible(false);
  // window.setVerticalSyncEnabled(true);
  // window.setFramerateLimit(200);

  sf::CircleShape circle_shape;
  circle_shape.setRadius(30);
  circle_shape.setFillColor(sf::Color::Green);

  sf::Font font;
  font.loadFromFile("sfml/arial.ttf");

  sf::Text text_fps;

  text_fps.setFont(font);
  text_fps.setPosition({.5, .5});
  text_fps.setString("");
  text_fps.setFillColor(sf::Color::Red);
  text_fps.setCharacterSize(50);

  sf::Clock clock;
  // sf::Time elapsed = clock.restart();
  double summed_time{};
  const double fps_refresh_cycle{1.0};
  int n_frames{0};

  std::map<sf::Keyboard::Key, bool> keys_pressed;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
        case sf::Event::KeyPressed:
          keys_pressed[event.key.code] = true;
          break;
        case sf::Event::KeyReleased:
          keys_pressed[event.key.code] = false;
          break;
      }
    }

    sf::View fixed = window.getView();  // Create a fixed view

    summed_time += clock.restart().asMicroseconds() / 1e6;
    n_frames++;

    if (summed_time > fps_refresh_cycle) {
      text_fps.setString(std::to_string(static_cast<int>(n_frames / summed_time)));
      n_frames = 0;
      summed_time = 0.0;
    }

    const auto mouse_position = sf::Mouse::getPosition(window);
    // std::cout << "mouse_position: x: " << mouse_position.x << "; y: " << mouse_position.y << "\n";
    const auto local_pos = window.mapPixelToCoords(mouse_position);
    // std::cout << "local_pos       x: " << local_pos.x << "; y: " << local_pos.y << "\n";

    std::cout << "Currently pressed keys:";
    for (const auto value : keys_pressed) {
      if (value.second) {
        std::cout << " " << value.first;
      }
    }
    std::cout << "\n";

    circle_shape.setPosition(local_pos - sf::Vector2f{circle_shape.getRadius(), circle_shape.getRadius()});

    window.clear();
    window.setView(fixed);
    window.draw(circle_shape);
    window.draw(text_fps);
    window.display();
  }

  return 0;
}