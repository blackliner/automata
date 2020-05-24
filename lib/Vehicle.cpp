#include "Vehicle.h"

double Map(double value, double from_start, double from_end, double to_start, double to_end) {
  constexpr double eps{0.0001};

  if (value < from_start) return to_start;
  if (value > from_end) return to_end;
  auto to_diff = to_end - to_start;
  if (std::abs(to_diff) < eps) return to_start;
  auto from_diff = from_end - from_start;
  if (std::abs(from_diff) < eps) throw std::invalid_argument("from_* values are within eps: " + std::to_string(eps));

  return to_start + (value - from_start) / from_diff * to_diff;
}

void Vehicle::InitVehicle() {
  ID = current_id++;

  if (rand() < RAND_MAX / 2)
    clan = Clan::BLUE;
  else
    clan = Clan::RED;

  SetType(GetRandomType());
}

void Vehicle::SetType(const VehicleType& type) {
  switch (type) {
    case VehicleType::TRIANGLE:
      vehicle_type = VehicleType::TRIANGLE;
      sensor_type = SensorType::BOTH;
      break;
    case VehicleType::CIRCLE:
      vehicle_type = VehicleType::CIRCLE;
      sensor_type = SensorType::CIRCLE;
      sensor_circle_radius = 200;
      break;
    case VehicleType::FLY:
      vehicle_type = VehicleType::CIRCLE;
      sensor_type = SensorType::CIRCLE;
      sensor_circle_radius = 25;
      size = 5;
      max_force *= 10;
      break;
    case VehicleType::BIRD:
      vehicle_type = VehicleType::TRIANGLE;
      sensor_type = SensorType::BOTH;
      sensor_circle_radius = 100;
      sensor_cone_radius = 500;
      size = 25;
      break;
  }
}

VehicleType Vehicle::GetRandomType() {
  double rand_val{4.0 * rand() / RAND_MAX};

  if (rand_val < 1) return VehicleType::CIRCLE;
  if (rand_val < 2) return VehicleType::TRIANGLE;
  if (rand_val < 3) return VehicleType::FLY;

  return VehicleType::BIRD;
}

void Vehicle::UpdateKinematics(double delta_t) {
  WriteableState().vel = ReadableState().vel + WriteableState().acc * delta_t;
  WriteableState().vel.Limit(max_velocity);
  WriteableState().pos = ReadableState().pos + WriteableState().vel * delta_t;

  // TODO really have to zero here?
  WriteableState().acc = Vector2D<VectorT>::Zero();

  if (WriteableState().vel.MagSquared() > 0.0001) {
    last_heading = WriteableState().vel;
    last_heading.Normalize();
  }

  if (health > 0.0) health -= delta_t;

  if (reproduction_time < reproduction_waiting_time) {
    reproduction_time += delta_t;
  } else {
    m_reproduction_ready = true;
  }
}

Vector2D<VectorT> Vehicle::PredictedPos() const {
  return ReadableState().pos + ReadableState().vel * 0.2;
}

void Vehicle::applyForce(Vector2D<VectorT> force, double delta_t) {
  if (delta_t > 0.0) WriteableState().acc = WriteableState().acc + force;  // / delta_t;
}

VehicleStorage Vehicle::FindClosestVehicle(const VehicleStorage& vehicles) {
  VehicleStorage ret_value;

  if (!vehicles.empty()) {
    auto closest = std::min_element(vehicles.begin(), vehicles.end(), [this](const Vehicle& a, const Vehicle& b) {
      auto dist_a = (a.ReadableState().pos - ReadableState().pos).MagSquared();
      auto dist_b = (b.ReadableState().pos - ReadableState().pos).MagSquared();
      return dist_a < dist_b;
    });

    if (closest != vehicles.end()) {
      ret_value.push_back(*closest);
    }
  }

  return ret_value;
}

std::optional<Vehicle> Vehicle::FindClosestMatingPartner(const VehicleStorage& vehicles) {
  auto min_distance_squared = std::numeric_limits<double>::max();

  std::optional<Vehicle> ret_val{};

  for (const auto& vehicle : vehicles) {
    auto distance_squared = CalculateDistanceSquared(vehicle);

    if (distance_squared < min_distance_squared && vehicle.get().m_reproduction_ready) {
      ret_val = vehicle;
    }
  }

  return ret_val;
}

bool Vehicle::CheckEnemyClan(const Vehicle& vehicle) const {
  return clan != vehicle.clan;
}

double Vehicle::CalculateDistanceSquared(const Vehicle& vehicle) const {
  return (vehicle.ReadableState().pos - ReadableState().pos).MagSquared();
}

bool Vehicle::CheckInDistance(const Vehicle& vehicle, double distance) const {
  auto distance_squared = CalculateDistanceSquared(vehicle);
  auto is_circle_distance_ok = (distance_squared > 0) && (distance_squared < (distance * distance));
  return is_circle_distance_ok;
}

constexpr double DiamondAngle(double x, double y) {
  if (y >= 0)
    return (x >= 0) ? y / (x + y) : 1 - x / (-x + y);
  else
    return (x < 0) ? 2 - y / (-x - y) : 3 + x / (x - y);
}

constexpr double DiamondAngle(const Vector2D<VectorT>& point) {
  DiamondAngle(point.x, point.y);
}

constexpr double RadiansToDiamondAngle(double rad) {
  return DiamondAngle(std::cos(rad), std::sin(rad));
}

bool Vehicle::CheckInCone(const Vehicle& vehicle, double angle) const {
  if (true) {
    auto angle_a = DiamondAngle(vehicle.ReadableState().pos - ReadableState().pos);
    auto angle_b = DiamondAngle(last_heading);
    return abs(angle_a - angle_b) < RadiansToDiamondAngle(angle);
  }

  auto a = vehicle.ReadableState().pos - ReadableState().pos;
  auto b = last_heading;
  auto angle_to_vehicle = acos(a.Dot(b) / (a.Mag() * b.Mag()));
  return angle_to_vehicle < angle;
}

VehicleStorage Vehicle::CircularSensor(std::vector<Vehicle>& vehicles) const {
  VehicleStorage ret_value;

  for (auto& vehicle : vehicles) {
    if (CheckInDistance(vehicle, sensor_circle_radius)) ret_value.push_back(vehicle);
  }

  return ret_value;
}

VehicleStorage Vehicle::AngularSensor(std::vector<Vehicle>& vehicles) const {
  VehicleStorage ret_value;

  for (auto& vehicle : vehicles) {
    if (CheckInDistance(vehicle, sensor_cone_radius) && CheckInCone(vehicle, sensor_angle))
      ret_value.push_back(std::reference_wrapper(vehicle));
  }

  return ret_value;
}

VehicleStorage Vehicle::BothSensor(std::vector<Vehicle>& vehicles) const {
  VehicleStorage ret_value;

  for (auto& vehicle : vehicles) {
    if (CheckInDistance(vehicle, sensor_circle_radius) ||
        (CheckInDistance(vehicle, sensor_cone_radius) && CheckInCone(vehicle, sensor_angle)))
      ret_value.push_back(std::reference_wrapper(vehicle));
  }

  return ret_value;
}

VehicleStorage Vehicle::ScanForVehiclesInRange(std::vector<Vehicle>& vehicles) const {
  switch (sensor_type) {
    case SensorType::CIRCLE:
      return CircularSensor(vehicles);
    case SensorType::ANGULAR:
      return AngularSensor(vehicles);
    case SensorType::BOTH:
      return BothSensor(vehicles);
  }

  throw std::runtime_error("Unknown enum value.");
}

Vector2D<VectorT> Vehicle::Seek(const Vector2D<VectorT>& target_pos) const {
  auto desired = (target_pos - ReadableState().pos);
  desired.SetMag(max_velocity);

  auto steer = (desired - ReadableState().vel) / max_velocity * max_force;
  steer.Limit(max_force);

  return steer;
}

Vector2D<VectorT> Vehicle::Arrive(const Vector2D<VectorT>& target_pos) const {
  auto desired = (target_pos - ReadableState().pos);
  auto distance = desired.Mag();

  auto desired_mag = Map(distance, size, sensor_circle_radius, 0, max_velocity);
  desired.SetMag(desired_mag);

  // auto steer = desired - vel;
  auto steer = (desired - ReadableState().vel) / max_velocity * max_force;
  steer.Limit(max_force);

  return steer;
}

Vector2D<VectorT> Vehicle::FollowPath(const std::vector<PathSegment>& path) const {
  Vector2D<VectorT> steer{}, target{}, normal_point{};
  auto min_distance_squared = std::numeric_limits<double>::max();

  auto pos_to_check = PredictedPos();

  for (auto segment : path) {
    normal_point = NormalPoint(pos_to_check, segment.p1, segment.p2);

    // check if normal_point is on the segment
    if (!segment.IsPointOnSegment(normal_point)) {
      normal_point = segment.GetClosestPoint(pos_to_check);
    }

    auto distance_squared = (pos_to_check - normal_point).MagSquared();

    if (distance_squared < min_distance_squared) {
      steer = {0, 0};

      min_distance_squared = distance_squared;
      if (distance_squared > (segment.size * segment.size)) {
        auto path_dir = segment.p2 - segment.p1;
        path_dir.Normalize();
        target = normal_point + path_dir * 50.0;

        steer = Seek(target);
      }
    }
  }

  return steer;
}

Vector2D<VectorT> Vehicle::Align(const VehicleStorage& vehicles) const {
  Vector2D<VectorT> desired{}, steer{};

  for (auto vehicle : vehicles) {
    desired = desired + vehicle.get().ReadableState().vel;
  }

  if (!vehicles.empty()) {
    desired.SetMag(max_velocity);

    // steer = desired - vel;
    steer = (desired - ReadableState().vel) / max_velocity * max_force;
    steer.Limit(max_force);
  }

  return steer;
}

Vector2D<VectorT> Vehicle::Flee(const VehicleStorage& vehicles) const {
  Vector2D<VectorT> desired{};

  for (auto vehicle : vehicles) {
    desired = desired + ReadableState().pos - vehicle.get().ReadableState().pos;
  }

  desired.SetMag(max_velocity);

  // auto steer = desired - vel;
  auto steer = (desired - ReadableState().vel) / max_velocity * max_force;
  steer.Limit(max_force);

  return steer;
}

Vector2D<VectorT> Vehicle::Separate(const VehicleStorage& vehicles) const {
  Vector2D<VectorT> desired{}, steer{};
  int count{};

  for (auto vehicle : vehicles) {
    auto desired_separation = 1.0 * (size + vehicle.get().size);
    auto diff = ReadableState().pos - vehicle.get().ReadableState().pos;
    auto distance = diff.MagSquared();
    if (distance < (desired_separation * desired_separation)) {
      diff.Normalize();
      diff = diff / distance;
      desired = desired + diff;
      ++count;
    }
  }

  if (count) {
    desired.SetMag(max_velocity);
    // steer = desired - vel;
    steer = (desired - ReadableState().vel) / max_velocity * max_force;
    steer.Limit(max_force);
  }

  return steer;
}

Vector2D<VectorT> Vehicle::Cohesion(const VehicleStorage& vehicles) const {
  Vector2D<VectorT> desired_position{}, steer{};

  for (auto vehicle : vehicles) {
    desired_position = desired_position + vehicle.get().ReadableState().pos;
  }
  if (!vehicles.empty()) {
    desired_position = desired_position / vehicles.size();
    steer = Seek(desired_position);
  }

  return steer;
}

Vector2D<VectorT> Vehicle::Wander() const {
  auto perlin = Noise::noise(ReadableState().pos.x / 1000.0, ReadableState().pos.y / 1000.0, health / 1000.0);
  auto next_direction = perlin * M_PI * 2.0;

  Vector2D<VectorT> desired{sin(next_direction), cos(next_direction)};
  desired.SetMag(0.25 * max_velocity);

  // auto steer = desired - vel;
  auto steer = (desired - ReadableState().vel) / max_velocity * max_force;
  steer.Limit(max_force);

  return steer;
}

bool Vehicle::GunSensor(const VehicleStorage& vehicles) const {
  for (const auto& vehicle : vehicles) {
    auto is_cone_distance_ok = CheckInDistance(vehicle, weapon.GetGunRange());

    // auto a = vehicle.get().ReadableState().pos - ReadableState().pos;
    // auto b = last_heading;
    // auto angle_to_vehicle = acos(a.Dot(b) / (a.Mag() * b.Mag()));
    // auto is_angle_ok = angle_to_vehicle < weapon.GetGunAngle();

    auto is_angle_ok = CheckInCone(vehicle, weapon.GetGunAngle());

    auto is_enemy_clan = CheckEnemyClan(vehicle);

    if (is_cone_distance_ok && is_angle_ok && is_enemy_clan) return true;
  }

  return false;
}

void Vehicle::CheckForHits(std::vector<Vehicle>& vehicles) {
  if (m_guns_allowed) {
    for (auto& vehicle : vehicles) {
      if (CheckEnemyClan(vehicle)) {
        if (vehicle.weapon.CheckHit(ReadableState().pos, size)) {
          health = 0.0;
        }
      }
    }
  }
}

Vehicle& Vehicle::Reproduce(const VehicleStorage& vehicles) {
  auto& parent{*this};
  if (!m_reproduction_ready) return parent;

  for (auto& vehicle : vehicles) {
    if (!vehicle.get().m_reproduction_ready) {
      continue;
    }

    if (CheckInDistance(vehicle.get(), reproduction_radius + vehicle.get().reproduction_radius)) {
      auto reproduction_rand = (double)rand() / RAND_MAX;
      if (reproduction_rand < reproduction_chance) {
        reproduction_time = 0.0;
        m_reproduction_ready = false;

        vehicle.get().reproduction_time = 0.0;
        vehicle.get().m_reproduction_ready = false;
        return vehicle;
      }
    }
  }

  return parent;
}

template <typename T>
void DrawArrow(const Vector2D<T> start,
               const Vector2D<T> end,
               const IRenderer::Color color,
               const IRenderer& renderer) {
  renderer.DrawLine(start, end, color);  // shaft

  auto shaft = end - start;
  shaft.SetMag(25.0);

  shaft.Rotate(M_PI / 4);
  renderer.DrawLine(end, end - shaft, color);  // left thingy

  shaft.Rotate(-M_PI / 2);
  renderer.DrawLine(end, end - shaft, color);  // right thingy
}

void DrawVehicleVelocity(const Vehicle& vehicle, const IRenderer& renderer) {
  if (vehicle.ReadableState().vel.MagSquared() > 0.0001) {
    DrawArrow(vehicle.ReadableState().pos, vehicle.ReadableState().pos + vehicle.ReadableState().vel,
              IRenderer::Color::GREEN, renderer);
  }
}

void DrawCircleSensor(const Vehicle& vehicle, const IRenderer& renderer) {
  renderer.DrawCircle(vehicle.ReadableState().pos, vehicle.sensor_circle_radius, IRenderer::Color::WHITE);
}

void DrawAngularSensor(const Vehicle& vehicle, const IRenderer& renderer) {
  auto left_cone_part = vehicle.last_heading;
  left_cone_part.Rotate(vehicle.sensor_angle);
  left_cone_part.SetMag(vehicle.sensor_cone_radius);
  left_cone_part = left_cone_part + vehicle.ReadableState().pos;

  auto right_cone_part = vehicle.last_heading;
  right_cone_part.Rotate(-vehicle.sensor_angle);
  right_cone_part.SetMag(vehicle.sensor_cone_radius);
  right_cone_part = right_cone_part + vehicle.ReadableState().pos;

  renderer.DrawLine(vehicle.ReadableState().pos, left_cone_part, IRenderer::Color::WHITE);
  renderer.DrawLine(vehicle.ReadableState().pos, right_cone_part, IRenderer::Color::WHITE);
}

void DrawVehicleSensor(const Vehicle& vehicle, const IRenderer& renderer) {
  switch (vehicle.sensor_type) {
    case SensorType::CIRCLE:
      DrawCircleSensor(vehicle, renderer);
      break;
    case SensorType::ANGULAR:
      DrawAngularSensor(vehicle, renderer);
      break;
    case SensorType::BOTH:
      DrawCircleSensor(vehicle, renderer);
      DrawAngularSensor(vehicle, renderer);
      break;
  }
}

void DrawVehicle(const Vehicle& vehicle, const IRenderer& renderer) {
  IRenderer::Color color{};
  switch (vehicle.clan) {
    case Clan::BLUE:
      color = IRenderer::Color::BLUE;
      break;
    case Clan::RED:
      color = IRenderer::Color::RED;
      break;
  }

  if (vehicle.m_reproduction_ready) {
    color = IRenderer::Color::DARK_YELLOW;
  }

  switch (vehicle.vehicle_type) {
    case VehicleType::BIRD:
    case VehicleType::TRIANGLE: {
      auto current_diretion = vehicle.last_heading;
      auto p1 = vehicle.ReadableState().pos + current_diretion * vehicle.size;
      auto p3 = vehicle.ReadableState().pos - current_diretion * vehicle.size / 2.0;
      current_diretion.Rotate(M_PI * 3.0 / 4.0);
      auto p2 = vehicle.ReadableState().pos + current_diretion * vehicle.size;
      current_diretion.Rotate(M_PI * 1.0 / 2.0);
      auto p4 = vehicle.ReadableState().pos + current_diretion * vehicle.size;

      renderer.DrawLine(p1, p2, color);
      renderer.DrawLine(p2, p3, color);
      renderer.DrawLine(p3, p4, color);
      renderer.DrawLine(p4, p1, color);

      // shoot gun
      if (vehicle.weapon.GetGunShot()) {
        renderer.DrawCircle(vehicle.weapon.GetPos(), vehicle.weapon.GetShellRadius(), color);
      }

      break;
    }
    case VehicleType::CIRCLE:
    case VehicleType::FLY: {
      renderer.DrawCircle(vehicle.ReadableState().pos, vehicle.size, color);

      // draw gun
      if (vehicle.weapon.GetGunShot()) {
        renderer.DrawCircle(vehicle.weapon.GetPos(), vehicle.weapon.GetShellRadius(), color);
      }

      break;
    }
  }
}

void Vehicle::Draw() const {
  DrawVehicle(*this, *m_renderer);

  // DrawVehicleVelocity(*this, *m_renderer);

  // DrawVehicleSensor(*this, *m_renderer);
}

void Vehicle::UpdateBehavior(const VehicleStorage& vehicles_in_range, double delta_t) {
  auto found_target = FindClosestVehicle(vehicles_in_range);

  if (!found_target.empty()) {
    auto seek = Seek(found_target.front().get().ReadableState().pos);
    auto arrive = Arrive(found_target.front().get().ReadableState().pos);
    auto arrive_predicted = Arrive(found_target.front().get().PredictedPos());

    applyForce(seek * 0, delta_t);
    applyForce(arrive * 0, delta_t);
    applyForce(arrive_predicted * 0, delta_t);
  }

  if (m_reproduction_ready) {
    auto found_mate = FindClosestMatingPartner(vehicles_in_range);

    if (found_mate) {
      auto arrive = Arrive(found_mate->ReadableState().pos);

      applyForce(arrive * 2.0, delta_t);
    }
  }

  auto align = Align(vehicles_in_range);
  auto flee = Flee(vehicles_in_range);
  auto separate = Separate(vehicles_in_range);
  auto cohesion = Cohesion(vehicles_in_range);
  auto wander = Wander();

  applyForce(align * 0.25, delta_t);
  applyForce(flee * 0.10, delta_t);
  applyForce(separate * 2.0, delta_t);
  applyForce(cohesion * 0.1, delta_t);
  applyForce(wander * 0.01, delta_t);
}

void Vehicle::UpdatePathFollowing(const std::vector<PathSegment>& path, double delta_t) {
  auto follow_path = FollowPath(path);

  applyForce(follow_path, delta_t);
}

std::optional<VehicleType> Vehicle::UpdateReproduction(const VehicleStorage& vehicles_in_range) {
  auto& parent = Reproduce(vehicles_in_range);

  if (*this != parent) {
    auto type = vehicle_type;

    if (type != parent.vehicle_type) {
      // if parents were different types, roll the new type
      type = Vehicle::GetRandomType();
    }

    return type;
  } else {
    return {};
  }
}

void Vehicle::UpdateWeapons(const VehicleStorage& vehicles_in_range, double delta_t) {
  if (m_guns_allowed) {
    if (GunSensor(vehicles_in_range)) {
      weapon.FireGun(ReadableState().pos, last_heading, size);
    }

    weapon.Update(delta_t);
  }
}
