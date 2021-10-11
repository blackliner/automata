#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

class FramesCounter {
 public:
  float Update() {
    m_summed_time += m_clock.restart().asMicroseconds() / 1e6;
    m_frames++;

    if (m_summed_time > kFpsRefreshCycle) {
      m_framerate = m_frames / m_summed_time;
      m_frames = 0;
      m_summed_time = 0.0;
    }

    return m_framerate;
  }

 private:
  sf::Clock m_clock{};

  const double kFpsRefreshCycle{1.0};

  double m_framerate{};
  int m_frames{};
  double m_summed_time{};
};

void PrintKeys(const std::map<sf::Keyboard::Key, bool>& key_map) {
  std::cout << "Currently pressed keys:";
  for (const auto value : key_map) {
    if (value.second) {
      std::cout << " " << value.first;
    }
  }
  std::cout << "\n";
}

int main() {
  sf::RenderWindow window(sf::VideoMode(1024, 1024), "SFML works!");
  window.setMouseCursorVisible(false);

  sf::CircleShape circle_shape;
  circle_shape.setRadius(30);
  circle_shape.setFillColor(sf::Color::Green);

  sf::Font font;
  font.loadFromFile("sfml/arial.ttf");

  FramesCounter frames_counter;

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

    text_fps.setString(std::to_string(frames_counter.Update()));

    const auto mouse_position = sf::Mouse::getPosition(window);
    const auto local_pos = window.mapPixelToCoords(mouse_position);

    circle_shape.setPosition(local_pos - sf::Vector2f{circle_shape.getRadius(), circle_shape.getRadius()});

    window.clear();
    window.setView(fixed);
    window.draw(circle_shape);
    window.draw(text_fps);
    window.display();

    PrintKeys(keys_pressed);
  }

  return 0;
}
