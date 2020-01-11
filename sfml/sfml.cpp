#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

int main() {
  sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML works!");
  sf::CircleShape shape(100.f);
  shape.setFillColor(sf::Color::Green);

  sf::Font font;
  font.loadFromFile("sfml/arial.ttf");

  sf::Text fps;

  fps.setFont(font);
  fps.setPosition({.5, .5});
  fps.setString("");
  fps.setFillColor(sf::Color::Red);
  fps.setCharacterSize(50);

  sf::Clock clock;
  // sf::Time elapsed = clock.restart();
  double summed_time{0.0};
  const double fps_refresh_cycle{1.0};
  int n_frames{0};

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
    }

    summed_time += clock.restart().asMicroseconds() / 1e6;
    n_frames++;

    if (summed_time > fps_refresh_cycle) {
      fps.setString(std::to_string(static_cast<int>(n_frames / summed_time)));
      n_frames = 0;
      summed_time = 0.0;
    }

    const auto p = sf::Mouse::getPosition();

    sf::Vector2f v;
    v.x = p.x;
    v.y = p.y;

    shape.setPosition(v);

    window.clear();
    window.draw(shape);
    window.draw(fps);
    window.display();
  }

  return 0;
}