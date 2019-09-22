#define _USE_MATH_DEFINES

#include <chrono>
#include <cmath>
#include <future>
#include <thread>

#include "Main.h"

void Maze::WrapVehiclePositions(Vehicle &vehicle) {
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

  //  vehicles.emplace_back(Vehicle{&renderer});
  //  vehicles.emplace_back(Vehicle{&renderer});
  //
  //  vehicles[vehicle_roby].WriteableState().pos.x = ScreenWidth() / 2.0;
  //  vehicles[vehicle_roby].WriteableState().pos.y = ScreenHeight() / 2.0;
  //
  //  vehicles[vehicle_roby].SwapStates();

  return true;
}

void UpdateMouseVehicle(Vehicle &vehicle, double x, double y, double delta_t) {

  auto &mouse = vehicle;
  auto tau = delta_t / (delta_t + 0.5);
  mouse.UpdateKinematics(delta_t);

  mouse.WriteableState().pos = {x, y};
  mouse.WriteableState().vel = (1 - tau) * mouse.ReadableState().vel +
                               (mouse.WriteableState().pos - mouse.ReadableState().pos) / (0.5 + delta_t);
}

long DrawScreen(const IRenderer *renderer, const std::vector<Vehicle> &vehicles, const std::vector<PathSegment> &path) {
  auto begin = std::chrono::high_resolution_clock::now();

  renderer->Clear();

  for (const auto &vehicle : vehicles) {
    vehicle.Draw();
  }

  for (auto segment : path) {
    renderer->DrawLine(segment.p1, segment.p2, IRenderer::Color::WHITE);
  }

  auto end = std::chrono::high_resolution_clock::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
}

void SwapStates(std::vector<Vehicle> &vehicles) {
  for (auto &vehicle : vehicles) {
    vehicle.SwapStates();
  }
}

bool Maze::OnUserUpdate(float fElapsedTime) {
  if (GetKey(olc::SPACE).bPressed)
    time_freeze = !time_freeze;
  if (time_freeze)
    fElapsedTime = 0.0;

  std::future<long> draw_future = std::async(DrawScreen, &renderer, vehicles, path);

  // calculate vehicle distances
  squared_distance_lookup.clear();
  for (const auto &veh_a : vehicles) {
    for (const auto &veh_b : vehicles) {
      auto distance_squared = (veh_a.ReadableState().pos - veh_b.ReadableState().pos).MagSquared();

      squared_distance_lookup[veh_a.ID][veh_b.ID] = distance_squared;
    }
  }

  //  std::thread draw_thread(DrawScreen, &renderer, vehicles, path);

  auto begin = std::chrono::high_resolution_clock::now();

  HandleInput(fElapsedTime);

  RemoveDeadVehicles();

  if (vehicles.size() > 1) {
    UpdateMouseVehicle(vehicles[vehicle_mouse], GetMouseX(), GetMouseY(), fElapsedTime);

    // godmode for our loved ones :-)
    vehicles[vehicle_mouse].health = 10.0;
    vehicles[vehicle_roby].health = 10.0;

    // do calculations for all but the first vehicle (which is our mouse)
    for (auto vehicle = std::next(vehicles.begin()); vehicle != vehicles.end(); vehicle = std::next(vehicle)) {

      auto found_vehicles = vehicle->ScanForVehiclesInRange(vehicles);

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

  DrawString(10, 20 * row_osd++, "duration game thread: " + std::to_string(duration_game_thread_ns / 1000) + "mys");
  DrawString(10, 20 * row_osd++, "duration draw thread: " + std::to_string(duration_draw_thread_ns / 1000) + "mys");
}

void Maze::HandleCommands() {
  if (!command_list.empty()) {

    for (const auto &command : command_list) {
      command->Execute();
    }

    command_list.clear();
  }
}

class AddNewVehicleCommand : public ICommand {
private:
  const IRenderer *m_renderer;
  SDLut *m_squared_distance_lookup;
  Vector2D<VectorT> m_position;
  VehicleType m_type;
  std::vector<Vehicle> &m_vehicles;

  int m_ID{};

  void AddNewVehicle(const IRenderer *renderer, SDLut *squared_distance_lookup, Vector2D<VectorT> position,
                     VehicleType type) {
    Vehicle new_vehicle{renderer, squared_distance_lookup};
    new_vehicle.WriteableState().pos = position;
    new_vehicle.WriteableState().vel.x = 100.0 + (double)rand() / (double)RAND_MAX * 100.0;
    new_vehicle.WriteableState().vel.y = 100.0 + (double)rand() / (double)RAND_MAX * 100.0;
    new_vehicle.SetType(type);
    m_vehicles.push_back(new_vehicle);

    m_ID = new_vehicle.ID;
  }

public:
  AddNewVehicleCommand(const IRenderer *renderer, SDLut *squared_distance_lookup, Vector2D<VectorT> position,
                       VehicleType type, std::vector<Vehicle> &m_vehicles)
      : m_renderer(renderer), m_squared_distance_lookup(squared_distance_lookup), m_position(position), m_type(type),
        m_vehicles(m_vehicles) {}

  void Execute() override { AddNewVehicle(m_renderer, m_squared_distance_lookup, m_position, m_type); }

  void Undo() override {
    // remove vehicle with ID == m_ID;
  }
};

void Maze::AddNewVehicle(double x, double y, VehicleType type) {
  command_list.push_back(std::make_unique<AddNewVehicleCommand>(&renderer, &squared_distance_lookup,
                                                                Vector2D<VectorT>{x, y}, type, vehicles));
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
  auto end = std::remove_if(vehicles.begin(), vehicles.end(), [](const Vehicle &v) { return !v.IsAlive(); });

  vehicles.erase(end, vehicles.end());
}

int main() {
  Maze demo;
  if (demo.Construct(nDisplayWidth, nDisplayHeight, 1, 1)) {
    demo.Start();
  }
}
