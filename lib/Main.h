#pragma once

#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"

#include "PathSegment.h"
#include "Vector2D.h"
#include "Vehicle.h"
#include "quadtree/quad_tree.h"

constexpr int fBlockWidth = 1;
constexpr int nWorldWidth = 210 * 10;
constexpr int nWorldHeight = 90 * 10;
constexpr int nDisplayWidth = nWorldWidth * fBlockWidth;
constexpr int nDisplayHeight = nWorldHeight * fBlockWidth;

olc::Pixel ColorConverter(IRenderer::Color color) {
  switch (color) {
    case IRenderer::Color::GREEN:
      return olc::GREEN;
    case IRenderer::Color::BLUE:
      return olc::BLUE;
    case IRenderer::Color::DARK_YELLOW:
      return olc::DARK_YELLOW;
    case IRenderer::Color::RED:
      return olc::RED;
    case IRenderer::Color::WHITE:
      return olc::WHITE;
  }

  throw std::runtime_error("Unhandled enum type.");
}

class ICommand {
 public:
  virtual ~ICommand() = default;

  virtual void Execute() = 0;

  virtual void Undo() = 0;
};

class Maze : public olc::PixelGameEngine {
 private:
  class OLCRenderer : public IRenderer {
   private:
    olc::PixelGameEngine& m_engine;

   public:
    explicit OLCRenderer(olc::PixelGameEngine& engine) : m_engine(engine){};

    void Clear() const override {
      m_engine.Clear(olc::BLACK);
    }

    void DrawLine(Vector2D<VectorT> start, Vector2D<VectorT> end, IRenderer::Color color) const override {
      m_engine.DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x),
                        static_cast<int>(end.y), ColorConverter(color));
    };

    void DrawCircle(Vector2D<VectorT> position, double radius, Color color) const override {
      m_engine.DrawCircle(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(radius),
                          ColorConverter(color));
    }
  };

  const int vehicle_mouse = 0;
  const int vehicle_roby = 1;

  OLCRenderer renderer;

  SDLut squared_distance_lookup;

  std::vector<Vehicle> vehicles;  // nr1 is our mouse, nr 2 is roby
  //  std::vector<Vehicle> new_born;
  std::vector<std::unique_ptr<ICommand>> command_list{};
  std::vector<PathSegment> path;

  Vector2D<VectorT> first_node{};
  bool first_node_valid{};
  double t_last_pressed{};
  bool time_freeze{};

 public:
  Maze() : renderer(OLCRenderer(*this)) {
    sAppName = "Automata";
  }

  void WrapVehiclePositions(Vehicle& vehicle);

  void AddNewVehicle(double x, double y, VehicleType type);

  void AddNewPathNode(VectorT x, VectorT y);

  bool OnUserUpdate(float fElapsedTime) override;

  bool OnUserCreate() override;

  void RemoveDeadVehicles();

  void HandleInput(float fElapsedTime);

  void HandleCommands();

  void DrawPerformanceOSD(long duration_game_thread_ns, long duration_draw_thread_ns);

  template <typename T>
  void DrawQuadTree(const QuadTree<T>& quad_tree) {
    DrawRect(quad_tree.m_top_left.x, quad_tree.m_top_left.y, quad_tree.m_bot_right.x - quad_tree.m_top_left.x,
             quad_tree.m_bot_right.y - quad_tree.m_top_left.y);

    if (quad_tree.m_child_sw) {
      DrawQuadTree(*quad_tree.m_child_sw);
    }
    if (quad_tree.m_child_se) {
      DrawQuadTree(*quad_tree.m_child_se);
    }
    if (quad_tree.m_child_nw) {
      DrawQuadTree(*quad_tree.m_child_nw);
    }
    if (quad_tree.m_child_ne) {
      DrawQuadTree(*quad_tree.m_child_ne);
    }
  }
};