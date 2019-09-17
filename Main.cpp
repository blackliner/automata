#define _USE_MATH_DEFINES

#include <cmath>

#include "Main.h"


void Maze::WrapVehiclePositions(Vehicle &vehicle)
{
  if (vehicle.pos.x < 0)
    vehicle.pos.x = nWorldWidth;
  else if (vehicle.pos.x > nWorldWidth)
    vehicle.pos.x = 0;

  if (vehicle.pos.y < 0)
    vehicle.pos.y = nWorldHeight;
  else if (vehicle.pos.y > nWorldHeight)
    vehicle.pos.y = 0;
}

//void Maze::DrawVehicle(const Vehicle &vehicle) const
//{
//  olc::Pixel color{};
//  switch (vehicle.clan)
//  {
//    case Clan::BLUE:
//      color = olc::BLUE;
//      break;
//    case Clan::RED:
//      color = olc::RED;
//      break;
//  }
//
//  if (vehicle.reproduction_ready)
//    color = olc::DARK_YELLOW;
//
//  switch (vehicle.vehicle_type)
//  {
//    case VehicleType::BIRD:
//    case VehicleType::TRIANGLE:
//    {
//      auto current_diretion = vehicle.last_heading;
//      auto p1 = vehicle.pos + current_diretion * vehicle.size;
//      auto p3 = vehicle.pos - current_diretion * vehicle.size / 2.0;
//      current_diretion.Rotate(M_PI * 3.0 / 4.0);
//      auto p2 = vehicle.pos + current_diretion * vehicle.size;
//      current_diretion.Rotate(M_PI * 1.0 / 2.0);
//      auto p4 = vehicle.pos + current_diretion * vehicle.size;
//
//      DrawLine(p1.x, p1.y, p2.x, p2.y, color);
//      DrawLine(p2.x, p2.y, p3.x, p3.y, color);
//      DrawLine(p3.x, p3.y, p4.x, p4.y, color);
//      DrawLine(p4.x, p4.y, p1.x, p1.y, color);
//
//      //draw gun
//      if (vehicle.weapon.GetGunShot())
//      {
//        DrawCircle(vehicle.weapon.GetPos().x, vehicle.weapon.GetPos().y, vehicle.weapon.GetShellRadius(), color);
//      }
//
//      break;
//    }
//    case VehicleType::CIRCLE:
//    case VehicleType::FLY:
//    {
//      DrawCircle(vehicle.pos.x, vehicle.pos.y, vehicle.size, color);
//
//      //draw gun
//      if (vehicle.weapon.GetGunShot())
//      {
//        DrawCircle(vehicle.weapon.GetPos().x, vehicle.weapon.GetPos().y, vehicle.weapon.GetShellRadius(), color);
//      }
//
//      break;
//    }
//  }
//}

//void Maze::DrawCircleSensor(const Vehicle &vehicle) const
//{
//  DrawCircle(vehicle.pos.x, vehicle.pos.y, vehicle.sensor_circle_radius);
//}
//
//void Maze::DrawAngularSensor(const Vehicle &vehicle) const
//{
//  auto left_cone_part = vehicle.last_heading;
//  left_cone_part.Rotate(vehicle.sensor_angle);
//  left_cone_part.SetMag(vehicle.sensor_cone_radius);
//  left_cone_part = left_cone_part + vehicle.pos;
//
//  auto right_cone_part = vehicle.last_heading;
//  right_cone_part.Rotate(-vehicle.sensor_angle);
//  right_cone_part.SetMag(vehicle.sensor_cone_radius);
//  right_cone_part = right_cone_part + vehicle.pos;
//
//  DrawLine(vehicle.pos.x, vehicle.pos.y, left_cone_part.x, left_cone_part.y);
//  DrawLine(vehicle.pos.x, vehicle.pos.y, right_cone_part.x, right_cone_part.y);
//}
//
//void Maze::DrawVehicleSensor(const Vehicle &vehicle) const
//{
//  switch (vehicle.sensor_type)
//  {
//    case SensorType::CIRCLE:
//      DrawCircleSensor(vehicle);
//      break;
//    case SensorType::ANGULAR:
//      DrawAngularSensor(vehicle);
//      break;
//    case SensorType::BOTH:
//      DrawCircleSensor(vehicle);
//      DrawAngularSensor(vehicle);
//      break;
//  }
//}

void Maze::AddNewVehicle(double x, double y, const VehicleType &type)
{
  Vehicle new_vehicle{&renderer};
  new_vehicle.pos.x = x;
  new_vehicle.pos.y = y;
  new_vehicle.vel.x = 100.0 + (double) rand() / (double) RAND_MAX * 100.0;
  new_vehicle.vel.y = 100.0 + (double) rand() / (double) RAND_MAX * 100.0;
  new_vehicle.SetType(type);
  new_born.push_back(new_vehicle);
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

void UpdateMouseVehicle(Vehicle& vehicle, double x, double y, double delta_t)
{
  auto &mouse = vehicle;
  auto tau = delta_t / (delta_t + 0.5);
  auto last_pos = mouse.pos;
  auto last_vel = mouse.vel;
  mouse.update(delta_t);
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
  Clear(olc::BLACK);

  UpdateMouseVehicle(vehicles[vehicle_mouse], GetMouseX(), GetMouseY(), fElapsedTime);

  // add new vehicles
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
    }
  }

  // add a path node
  if (GetMouse(1).bPressed)
  {
    AddNewPathNode((double) GetMouseX(), (double) GetMouseY());
  }

  //remove all dead vehicles
  auto end = std::remove_if(vehicles.begin(), vehicles.end(), [](const Vehicle &v)
  {
    return !v.IsAlive();
  });
  vehicles.erase(end, vehicles.end());

  //... and add the newborns from before
  if (!new_born.empty())
  {
    vehicles.insert(vehicles.end(), new_born.begin(), new_born.end());
    new_born.clear();
  }

  // do calculations for all but the first vehicle (which is our mouse)
  for (auto vehicle = std::next(vehicles.begin()); vehicle != vehicles.end(); vehicle = std::next(vehicle))
    //for (auto& vehicle : vehicles)
  {

    if (guns_allowed)
    {
      if (vehicle->GunSensor(vehicles))
      {
        vehicle->weapon.FireGun(vehicle->pos, vehicle->last_heading, vehicle->size);
      }
      vehicle->CheckForHits(vehicles);

      vehicle->weapon.Update(fElapsedTime);
    }

    auto found_vehicles = vehicle->ScanForVehiclesInRange(vehicles);
    auto found_target = vehicle->FindClosestVehicle(found_vehicles);

    if (!found_target.empty())
    {
      auto seek = vehicle->Seek(found_target.front().get().pos);
      auto arrive = vehicle->Arrive(found_target.front().get().pos);
      auto arrive_predicted = vehicle->Arrive(found_target.front().get().PredictedPos());

      vehicle->applyForce(seek * 0, fElapsedTime);
      vehicle->applyForce(arrive * 0, fElapsedTime);
      vehicle->applyForce(arrive_predicted * 0, fElapsedTime);
    }

    if (vehicle->reproduction_ready)
    {
      auto found_mate = vehicle->FindClosestMatingPartner(found_vehicles);

      if (!found_mate.empty())
      {
        auto arrive = vehicle->Arrive(found_mate.front().get().pos);

        vehicle->applyForce(arrive * 2.0, fElapsedTime);
      }
    }

    auto align = vehicle->Align(found_vehicles);
    auto flee = vehicle->Flee(found_vehicles);
    auto separate = vehicle->Separate(found_vehicles);
    auto cohesion = vehicle->Cohesion(found_vehicles);
    auto wander = vehicle->Wander();

    vehicle->applyForce(align * 0.25, fElapsedTime);
    vehicle->applyForce(flee * 0.10, fElapsedTime);
    vehicle->applyForce(separate * 2.0, fElapsedTime);
    vehicle->applyForce(cohesion * 0.1, fElapsedTime);
    vehicle->applyForce(wander * 0.01, fElapsedTime);

    auto follow_path = vehicle->FollowPath(path);
    //debug//auto follow_path = FollowPath(*vehicle, path);

    vehicle->applyForce(follow_path, fElapsedTime);

    // reproduction and feasting
    auto parent = vehicle->Reproduce(found_vehicles);
    if (*vehicle != parent)
    {
      auto type = vehicle->vehicle_type;

      if (type != parent.vehicle_type)
      {
        // if parents were different types, roll the new type
        type = Vehicle::GetRandomType();
      }

      AddNewVehicle(vehicle->pos.x, vehicle->pos.y, type);
    }

    vehicle->update(fElapsedTime);

    WrapVehiclePositions(*vehicle);
  }


  for (auto &vehicle : vehicles)
  {
    vehicle.Draw();
  }

  for (auto segment : path)
  {
    DrawLine(segment.p1.x, segment.p1.y, segment.p2.x, segment.p2.y);
  }

  return true;
}

int main()
{
  Maze demo;
  if (demo.Construct(nDisplayWidth, nDisplayHeight, 1, 1))
    demo.Start();
}
