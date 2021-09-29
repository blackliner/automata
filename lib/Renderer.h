#ifndef AUTOMATA_RENDERER_H
#define AUTOMATA_RENDERER_H

#include "Vector2D.h"

class IRenderer {
 public:
  enum class Color { GREEN, BLUE, RED, DARK_YELLOW, WHITE };

  virtual void Clear() const = 0;
  virtual void DrawLine(Vector2D<VectorT> start, Vector2D<VectorT> end, Color color) const = 0;
  virtual void DrawCircle(Vector2D<VectorT> position, double radius, Color color) const = 0;
};

#endif  // AUTOMATA_RENDERER_H
