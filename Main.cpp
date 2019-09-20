#define _USE_MATH_DEFINES

#include <cmath>

#include "Main.h"


void Maze::WrapVehiclePositions(Vehicle &vehicle)
{
  if (vehicle.pos.x < 0)
  {
    vehicle.pos.x += nWorldWidth;
  } else if (vehicle.pos.x > nWorldWidth)
  {
    vehicle.pos.x -= nWorldWidth;
  }

  if (vehicle.pos.y < 0)
  {
    vehicle.pos.y += nWorldHeight;
  } else if (vehicle.pos.y > nWorldHeight)
  {
    vehicle.pos.y -= nWorldHeight;
  }
}


void Maze::AddNewPathNode(VectorT x, VectorT y)
{
  if (!first_node_valid)
  {
    first_node = Vector2D{x, y};
    first_node_valid = true;
  } else
  {
    PathSegment segment;
    if (path.empty())
    {
      segment.p1 = first_node;
      segment.p2 = Vector2D{x, y};
    } else
    {
      segment.p1 = path.back().p2;
      segment.p2 = Vector2D{x, y};
    }

    path.push_back(segment);
  }
}

bool Maze::OnUserCreate()
{
  vehicles.emplace_back(Vehicle{&renderer});
  vehicles.emplace_back(Vehicle{&renderer});

  vehicles[vehicle_roby].pos.x = ScreenWidth() / 2.0;
  vehicles[vehicle_roby].pos.y = ScreenHeight() / 2.0;

  return true;
}

void UpdateMouseVehicle(Vehicle &vehicle, double x, double y, double delta_t)
{
  auto &mouse = vehicle;
  auto tau = delta_t / (delta_t + 0.5);
  auto last_pos = mouse.pos;
  auto last_vel = mouse.vel;
  mouse.UpdateKinematics(delta_t);
  mouse.pos = {x, y};
  mouse.vel = (1 - tau) * last_vel + (mouse.pos - last_pos) / (0.2 + delta_t);
}

bool Maze::OnUserUpdate(float fElapsedTime)
{
  if (GetKey(olc::SPACE).bPressed)
    time_freeze = !time_freeze;
  if (time_freeze)
    fElapsedTime = 0.0;

  //godmode for our loved ones :-)
  vehicles[vehicle_mouse].health = 10.0;
  vehicles[vehicle_roby].health = 10.0;

  // DRAW
//  Clear(olc::BLACK);

  UpdateMouseVehicle(vehicles[vehicle_mouse], GetMouseX(), GetMouseY(), fElapsedTime);

  HandleInput(fElapsedTime);

  RemoveDeadVehicles();

  HandleCommands();

  // do calculations for all but the first vehicle (which is our mouse)
  for (auto vehicle = std::next(vehicles.begin()); vehicle != vehicles.end(); vehicle = std::next(vehicle))
  {

    auto found_vehicles = vehicle->ScanForVehiclesInRange(vehicles);

    vehicle->UpdateWeapons(found_vehicles, fElapsedTime);

    vehicle->CheckForHits(vehicles);

    vehicle->UpdateBehavior(found_vehicles, fElapsedTime);

    vehicle->UpdatePathFollowing(path, fElapsedTime);


    auto type = vehicle->UpdateReproduction(found_vehicles);
    if (type)
    {
      AddNewVehicle(vehicle->pos.x, vehicle->pos.y, *type);
    }

    vehicle->UpdateKinematics(fElapsedTime);

    WrapVehiclePositions(*vehicle);

    vehicle->Draw();
  }

  vehicles[vehicle_mouse].Draw();

  for (auto segment : path)
  {
    DrawLine(segment.p1.x, segment.p1.y, segment.p2.x, segment.p2.y);
  }

  return true;
}

void Maze::HandleCommands()
{
  if (!command_list.empty())
  {

    for (const auto &command : command_list)
    {
      command->Execute();
    }

    command_list.clear();
  }
}

class AddNewVehicleCommand : public ICommand
{
private:
  IRenderer *m_renderer;
  Vector2D<VectorT> m_position;
  VehicleType m_type;
  std::vector<Vehicle> &m_vehicles;

  int m_ID{};

  void AddNewVehicle(IRenderer *renderer, Vector2D<VectorT> position, VehicleType type)
  {
    Vehicle new_vehicle{renderer};
    new_vehicle.pos.x = position.x;
    new_vehicle.pos.y = position.y;
    new_vehicle.vel.x = 100.0 + (double) rand() / (double) RAND_MAX * 100.0;
    new_vehicle.vel.y = 100.0 + (double) rand() / (double) RAND_MAX * 100.0;
    new_vehicle.SetType(type);
    m_vehicles.push_back(new_vehicle);

    m_ID = new_vehicle.ID;
  }

public:
  AddNewVehicleCommand(IRenderer *renderer, Vector2D<VectorT> position, VehicleType type,
                       std::vector<Vehicle> &m_vehicles) : m_renderer(renderer), m_position(position), m_type(type),
                                                           m_vehicles(m_vehicles)
  {
  }

  void Execute() override
  {
    AddNewVehicle(m_renderer, m_position, m_type);
  }

  void Undo() override
  {
    // remove vehicle with ID == m_ID;
  }

};


void Maze::AddNewVehicle(double x, double y, const VehicleType &type)
{
  command_list.push_back(std::make_unique<AddNewVehicleCommand>(&renderer, Vector2D<VectorT>{x, y}, type, vehicles));
}

void Maze::HandleInput(float fElapsedTime)
{

  if (GetMouse(0).bPressed)
  {
    t_last_pressed = 0.0;
    AddNewVehicle((double) GetMouseX(), (double) GetMouseY(), Vehicle::GetRandomType());
  }
  if (GetMouse(0).bHeld)
  {
    DrawCircle(GetMouseX(), GetMouseY(), 20);
    FillCircle(GetMouseX(), GetMouseY(), static_cast<int>(Map(t_last_pressed, 0, 1, 0, 20)));

    t_last_pressed += fElapsedTime;
    if (t_last_pressed > 1.0)
    {
      AddNewVehicle((double) GetMouseX(), (double) GetMouseY(), Vehicle::GetRandomType());
      AddNewVehicle((double) GetMouseX(), (double) GetMouseY(), Vehicle::GetRandomType());
      AddNewVehicle((double) GetMouseX(), (double) GetMouseY(), Vehicle::GetRandomType());
      AddNewVehicle((double) GetMouseX(), (double) GetMouseY(), Vehicle::GetRandomType());
      AddNewVehicle((double) GetMouseX(), (double) GetMouseY(), Vehicle::GetRandomType());
    }
  }

  if (GetMouse(1).bPressed)
  {
    AddNewPathNode((double) GetMouseX(), (double) GetMouseY());
  }
}

void Maze::RemoveDeadVehicles()
{
  auto end = std::remove_if(vehicles.begin(), vehicles.end(), [](const Vehicle &v)
  {
    return !v.IsAlive();
  });

  vehicles.erase(end, vehicles.end());
}

int main()
{
  Maze demo;
  if (demo.Construct(nDisplayWidth, nDisplayHeight, 1, 1))
  {
    demo.Start();
  }
}
