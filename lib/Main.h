#pragma once

#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"

#include "PathSegment.h"
#include "Vector2D.h"
#include "Vehicle.h"

constexpr int fBlockWidth = 1;
constexpr int nWorldWidth = 210 * 10;
constexpr int nWorldHeight = 90 * 10;
constexpr int nDisplayWidth = nWorldWidth * fBlockWidth;
constexpr int nDisplayHeight = nWorldHeight * fBlockWidth;

olc::Pixel ColorConverter(IRenderer::Color color)
{
  switch (color)
  {
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

class ICommand
{
public:
  virtual ~ICommand() = default;

  virtual void Execute() = 0;

  virtual void Undo() = 0;
};

class Maze : public olc::PixelGameEngine
{
private:

  class OLCRenderer : public IRenderer
  {
  private:
    olc::PixelGameEngine &m_engine;

  public:
    explicit OLCRenderer(olc::PixelGameEngine &engine) : m_engine(engine)
    {
    };

    void DrawLine(Vector2D<VectorT> start, Vector2D<VectorT> end, IRenderer::Color color) const override
    {
      m_engine.DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x),
                        static_cast<int>(end.y), ColorConverter(color));
    };

    void DrawCircle(Vector2D<VectorT> position, double radius, Color color) const override
    {
      m_engine.DrawCircle(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(radius),
                          ColorConverter(color));
    }
  };


  const int vehicle_mouse = 0;
  const int vehicle_roby = 1;

  OLCRenderer renderer;

  std::vector<Vehicle> vehicles; //nr1 is our mouse, nr 2 is roby
//  std::vector<Vehicle> new_born;
  std::vector<std::unique_ptr<ICommand>> command_list{};
  std::vector<PathSegment> path;

  Vector2D<VectorT> first_node{};
  bool first_node_valid{};
  double t_last_pressed{};
  bool time_freeze{};

public:
  Maze() : renderer(OLCRenderer(*this))
  {
    sAppName = "Automata";
  }

  void WrapVehiclePositions(Vehicle &vehicle);

  void AddNewVehicle(double x, double y, const VehicleType &type);

  void AddNewPathNode(VectorT x, VectorT y);

  bool OnUserUpdate(float fElapsedTime) override;

  bool OnUserCreate() override;

  void RemoveDeadVehicles();

  void HandleInput(float fElapsedTime);

  void HandleCommands();
};