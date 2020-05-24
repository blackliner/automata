#define _USE_MATH_DEFINES

#include "Main.h"

#include <chrono>
#include <cmath>
#include <future>
#include <thread>

void Maze::WrapVehiclePositions(Vehicle& vehicle) {
  if (vehicle.WriteableState().pos.x < 0) {
    vehicle.WriteableState().pos.x += nWorldWidth;
  } else if (vehicle.WriteableState().pos.x > nWorldWidth) {
    vehicle.WriteableState().pos.x -= nWorldWidth;
  }

  if (vehicle.WriteableState().pos.y < 0) {
    vehicle.WriteableState().pos.y += nWorldHeight;
  } else if (vehicle.WriteableState().pos.y > nWorldHeight) {
    vehicle.WriteableState().pos.y -= nWorldHeight;
  }
}

void Maze::AddNewPathNode(VectorT x, VectorT y) {
  if (!first_node_valid) {
    first_node = Vector2D{x, y};
    first_node_valid = true;
  } else {
    PathSegment segment;
    if (path.empty()) {
      segment.p1 = first_node;
      segment.p2 = Vector2D{x, y};
    } else {
      segment.p1 = path.back().p2;
      segment.p2 = Vector2D{x, y};
    }

    path.push_back(segment);
  }
}

bool Maze::OnUserCreate() {
  AddNewVehicle(ScreenWidth() / 2, ScreenHeight() / 2, VehicleType::TRIANGLE);
  AddNewVehicle(ScreenWidth() / 2, ScreenHeight() / 2, VehicleType::TRIANGLE);

  return true;
}

void UpdateMouseVehicle(Vehicle& vehicle, double x, double y, double delta_t) {
  auto& mouse = vehicle;
  auto tau = delta_t / (delta_t + 0.5);
  mouse.UpdateKinematics(delta_t);

  mouse.WriteableState().pos = {x, y};
  mouse.WriteableState().vel = (1 - tau) * mouse.ReadableState().vel +
                               (mouse.WriteableState().pos - mouse.ReadableState().pos) / (0.5 + delta_t);
}

long DrawScreen(const IRenderer* renderer, const std::vector<Vehicle>& vehicles, const std::vector<PathSegment>& path) {
  auto begin = std::chrono::high_resolution_clock::now();

  renderer->Clear();

  for (const auto& vehicle : vehicles) {
    vehicle.Draw();
  }

  for (auto segment : path) {
    renderer->DrawLine(segment.p1, segment.p2, IRenderer::Color::WHITE);
  }

  auto end = std::chrono::high_resolution_clock::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
}

void SwapStates(std::vector<Vehicle>& vehicles) {
  for (auto& vehicle : vehicles) {
    vehicle.SwapStates();
  }
}

bool Maze::OnUserUpdate(float fElapsedTime) {
  if (GetKey(olc::SPACE).bPressed) time_freeze = !time_freeze;
  if (time_freeze) fElapsedTime = 0.0;

  std::future<long> draw_future = std::async(DrawScreen, &renderer, vehicles, path);

  auto begin = std::chrono::high_resolution_clock::now();

  HandleInput(fElapsedTime);

  RemoveDeadVehicles();

  QuadTree<Vehicle*> quad_tree{};
  quad_tree.SetPoints({0, ScreenHeight()}, {ScreenWidth(), 0});
  for (auto& vehicle : vehicles) {
    Point p{static_cast<int>(vehicle.ReadableState().pos.x), static_cast<int>(vehicle.ReadableState().pos.y)};
    quad_tree.AddNode(&vehicle, p);
  }

  // DrawQuadTree(quad_tree);

  if (vehicles.size() > 1) {
    UpdateMouseVehicle(vehicles[vehicle_mouse], GetMouseX(), GetMouseY(), fElapsedTime);

    // godmode for our loved ones :-)
    vehicles[vehicle_mouse].health = 10.0;
    vehicles[vehicle_roby].health = 10.0;

    // do calculations for all but the first vehicle (which is our mouse)
    for (auto vehicle = std::next(vehicles.begin()); vehicle != vehicles.end(); vehicle = std::next(vehicle)) {
      const int radius = static_cast<int>(std::max(vehicle->sensor_circle_radius, vehicle->sensor_cone_radius));
      const int x = vehicle->ReadableState().pos.x;
      const int y = vehicle->ReadableState().pos.y;
      const Point top_left{x - radius, y + radius};
      const Point bot_right{x + radius, y - radius};

      auto neighbours_tmp = quad_tree.Search(top_left, bot_right);

      VehicleStorage neighbours;
      for (Vehicle* neighbour : neighbours_tmp) {
        neighbours.push_back(std::reference_wrapper(*neighbour));
      }

      auto found_vehicles = vehicle->ScanForVehiclesInRange(neighbours);

      vehicle->UpdateWeapons(found_vehicles, fElapsedTime);

      vehicle->CheckForHits(vehicles);

      vehicle->UpdateBehavior(found_vehicles, fElapsedTime);

      vehicle->UpdatePathFollowing(path, fElapsedTime);

      auto type = vehicle->UpdateReproduction(found_vehicles);
      if (type) {
        AddNewVehicle(vehicle->ReadableState().pos.x, vehicle->ReadableState().pos.y, *type);
      }

      vehicle->UpdateKinematics(fElapsedTime);

      WrapVehiclePositions(*vehicle);
    }
  }

  auto end = std::chrono::high_resolution_clock::now();

  auto duration_game_thread = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

  auto duration_draw_thread = draw_future.get();

  HandleCommands();

  SwapStates(vehicles);

  DrawPerformanceOSD(duration_game_thread, duration_draw_thread);

  return true;
}

void Maze::DrawPerformanceOSD(long duration_game_thread_ns, long duration_draw_thread_ns) {
  int row_osd{};
  int row_offset{10};

  DrawString(10, row_offset + 20 * row_osd++,
             "duration game thread: " + std::to_string(duration_game_thread_ns / 1000) + "mys");
  DrawString(10, row_offset + 20 * row_osd++,
             "duration draw thread: " + std::to_string(duration_draw_thread_ns / 1000) + "mys");
  DrawString(10, row_offset + 20 * row_osd++, "total number of vehicles: " + std::to_string(vehicles.size()));
}

void Maze::HandleCommands() {
  if (!command_list.empty()) {
    for (const auto& command : command_list) {
      command->Execute();
    }

    command_list.clear();
  }
}

class AddNewVehicleCommand : public ICommand {
 private:
  const IRenderer* m_renderer;
  Vector2D<VectorT> m_position;
  VehicleType m_type;
  std::vector<Vehicle>& m_vehicles;

  int m_ID{};

  void AddNewVehicle(const IRenderer* renderer, Vector2D<VectorT> position, VehicleType type) {
    Vehicle new_vehicle{renderer};
    new_vehicle.WriteableState().pos = position;
    new_vehicle.WriteableState().vel.x = 100.0 + (double)rand() / (double)RAND_MAX * 100.0;
    new_vehicle.WriteableState().vel.y = 100.0 + (double)rand() / (double)RAND_MAX * 100.0;
    new_vehicle.SetType(type);
    m_vehicles.push_back(new_vehicle);

    m_ID = new_vehicle.ID;
  }

 public:
  AddNewVehicleCommand(const IRenderer* renderer,
                       Vector2D<VectorT> position,
                       VehicleType type,
                       std::vector<Vehicle>& vehicles)
      : m_renderer(renderer), m_position(position), m_type(type), m_vehicles(vehicles) {
  }

  void Execute() override {
    AddNewVehicle(m_renderer, m_position, m_type);
  }

  void Undo() override {
    // remove vehicle with ID == m_ID;
  }
};

void Maze::AddNewVehicle(double x, double y, VehicleType type) {
  command_list.push_back(std::make_unique<AddNewVehicleCommand>(&renderer, Vector2D<VectorT>{x, y}, type, vehicles));
}

void Maze::HandleInput(float fElapsedTime) {
  if (GetMouse(0).bPressed) {
    t_last_pressed = 0.0;
    AddNewVehicle((double)GetMouseX(), (double)GetMouseY(), Vehicle::GetRandomType());
  }
  if (GetMouse(0).bHeld) {
    DrawCircle(GetMouseX(), GetMouseY(), 20);
    FillCircle(GetMouseX(), GetMouseY(), static_cast<int>(Map(t_last_pressed, 0, 1, 0, 20)));

    t_last_pressed += fElapsedTime;
    if (t_last_pressed > 1.0) {
      AddNewVehicle((double)GetMouseX(), (double)GetMouseY(), Vehicle::GetRandomType());
    }
  }

  if (GetMouse(1).bPressed) {
    AddNewPathNode((double)GetMouseX(), (double)GetMouseY());
  }
}

void Maze::RemoveDeadVehicles() {
  auto end = std::remove_if(vehicles.begin(), vehicles.end(), [](const Vehicle& v) { return !v.IsAlive(); });

  vehicles.erase(end, vehicles.end());
}

int main() {
  Maze demo;
  if (demo.Construct(nDisplayWidth, nDisplayHeight, 1, 1)) {
    demo.Start();
  }
}
