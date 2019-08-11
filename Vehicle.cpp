#include "Vehicle.h"

double Map(double value, double from_start, double from_end, double to_start, double to_end)
{
	if (value < from_start)
		return to_start;
	if (value > from_end)
		return to_end;
	if (to_start == to_end)
		return to_start; //only if its 0.0!
	if (from_start == from_end)
		return NAN;

	return to_start + (value - from_start) / (from_end - from_start) * (to_end - to_start);
}

void Vehicle::InitVehicle()
{
	ID = current_id++;

	if (rand() < RAND_MAX / 2)
		clan = Clan::BLUE;
	else
		clan = Clan::RED;

	SetType(GetRandomType());
}

Vehicle::Vehicle()
{
	InitVehicle();
};

Vehicle::Vehicle(double x, double y) : pos{x, y}
{
	InitVehicle();
};

void Vehicle::SetType(const VehicleType &type)
{
	switch (type)
	{
	case VehicleType::ARROW:
		vehicle_type = VehicleType::ARROW;
		sensor_type = SensorType::BOTH;
		break;
	case VehicleType::CIRCLE:
		vehicle_type = VehicleType::CIRCLE;
		sensor_type = SensorType::CIRCLE;
		sensor_circle_radius = 200;
		break;
	}
}

VehicleType Vehicle::GetRandomType()
{
	if (rand() < RAND_MAX / 2)
		return VehicleType::ARROW;
	return VehicleType::CIRCLE;
}

void Vehicle::update(double delta_t)
{
	last_acc = acc;

	vel = vel + acc * delta_t;
	vel.Limit(max_velocity);
	pos = pos + vel * delta_t; //  +acc * delta_t * delta_t / 2.0;

	acc = Vector2D::Zero();

	if (vel.MagSquared() > 0.0001)
	{
		last_heading = vel;
		last_heading.Normalize();
	}

	if (health > 0.0)
		health -= delta_t;

	if (reproduction_time < reproduction_waiting_time)
	{
		reproduction_time += delta_t;
	}
	else
	{
		reproduction_ready = true;
	}

	// update gun

	if (gun_shell_lifetime < gun_shell_max_lifetime)
	{
		gun_shell_pos = gun_shell_pos + gun_shell_vel * delta_t;
		gun_shell_lifetime += delta_t;
	}
	else
	{
		gun_shot = false;
	}
}

Vector2D Vehicle::PredictedPos()
{
	return pos + vel * 0.2;
}

void Vehicle::applyForce(Vector2D force, double delta_t)
{
	if (delta_t > 0.0)
		acc = acc + force; // / delta_t;
}

VehicleStorage Vehicle::FindClosestVehicle(const VehicleStorage &vehicles)
{
	VehicleStorage ret_value;

	if (!vehicles.empty())
	{
		auto closest = std::min_element(vehicles.begin(), vehicles.end(), [this](Vehicle a, Vehicle b) {
			auto dist_a = (a.pos - pos).MagSquared();
			auto dist_b = (b.pos - pos).MagSquared();
			return dist_a < dist_b;
		});

		if (closest != vehicles.end())
		{
			ret_value.push_back(*closest);
		}
	}

	return ret_value;
}

VehicleStorage Vehicle::FindClosestMatingPartner(const VehicleStorage &vehicles)
{
	VehicleStorage ret_value;

	if (!vehicles.empty())
	{
		auto closest = std::min_element(vehicles.begin(), vehicles.end(), [this](Vehicle a, Vehicle b) {
			auto dist_a = (a.pos - pos).MagSquared();
			auto dist_b = (b.pos - pos).MagSquared();
			return dist_a < dist_b;
		});

		if (closest != vehicles.end())
		{
			for (auto vehicle : vehicles)
			{
				if (vehicle.get().reproduction_ready)
				{
					ret_value.push_back(vehicle);
					break;
				}
			}
		}
	}

	return ret_value;
}

bool Vehicle::CheckEnemyClan(Vehicle vehicle)
{
	return clan != vehicle.clan;
}

bool Vehicle::CheckInDistance(Vehicle vehicle, double distance)
{
	auto distance_squared = (vehicle.pos - pos).MagSquared();
	auto is_circle_distance_ok = (distance_squared > 0) && (distance_squared < (distance * distance));
	return is_circle_distance_ok;
}

bool Vehicle::CheckInCone(Vehicle vehicle, double angle)
{
	auto a = vehicle.pos - pos;
	auto b = last_heading;
	auto angle_to_vehicle = acos(a.Dot(b) / (a.Mag() * b.Mag()));
	return angle_to_vehicle < angle;
}

VehicleStorage Vehicle::CircularSensor(std::vector<Vehicle> &vehicles)
{
	VehicleStorage ret_value;

	for (auto &vehicle : vehicles)
	{
		if (CheckInDistance(vehicle, sensor_circle_radius))
			ret_value.push_back(vehicle);
	}

	return ret_value;
}

VehicleStorage Vehicle::AngularSensor(std::vector<Vehicle> &vehicles)
{
	VehicleStorage ret_value;

	for (auto &vehicle : vehicles)
	{
		if (CheckInDistance(vehicle, sensor_cone_radius) && CheckInCone(vehicle, sensor_angle))
			ret_value.push_back(vehicle);
	}

	return ret_value;
}

VehicleStorage Vehicle::BothSensor(std::vector<Vehicle> &vehicles)
{
	VehicleStorage ret_value;

	for (auto &vehicle : vehicles)
	{
		if (CheckInDistance(vehicle, sensor_circle_radius) || (CheckInDistance(vehicle, sensor_cone_radius) && CheckInCone(vehicle, sensor_angle)))
			ret_value.push_back(vehicle);
	}

	return ret_value;
}

VehicleStorage Vehicle::ScanForVehiclesInRange(std::vector<Vehicle> &vehicles)
{
	switch (sensor_type)
	{
	case SensorType::CIRCLE:
		return CircularSensor(vehicles);
	case SensorType::ANGULAR:
		return AngularSensor(vehicles);
	case SensorType::BOTH:
		return BothSensor(vehicles);
	}
	return CircularSensor(vehicles);
}

Vector2D Vehicle::Seek(Vector2D target_pos)
{
	auto desired = (target_pos - pos);
	desired.SetMag(max_velocity);

	auto steer = (desired - vel) / max_velocity * max_force;
	steer.Limit(max_force);

	return steer;
}

Vector2D Vehicle::Arrive(Vector2D target_pos)
{
	auto desired = (target_pos - pos);
	auto distance = desired.Mag();

	auto desired_mag = Map(distance, size, sensor_circle_radius, 0, max_velocity);
	desired.SetMag(desired_mag);

	//auto steer = desired - vel;
	auto steer = (desired - vel) / max_velocity * max_force;
	steer.Limit(max_force);

	return steer;
}

Vector2D Vehicle::FollowPath(std::vector<PathSegment> path)
{
	Vector2D steer{}, target{}, normal_point{}, best_normal_point{};
	auto min_distance_squared = std::numeric_limits<double>().max();

	auto pos_to_check = PredictedPos();

	for (auto segment : path)
	{
		normal_point = NormalPoint(pos_to_check, segment.p1, segment.p2);

		//check if normal_point is on the segment
		if (!segment.IsPointOnSegment(normal_point))
		{
			normal_point = segment.GetClosestPoint(pos_to_check);
		}

		//FillCircle(normal_point.x, normal_point.y, 10, olc::BLUE);

		auto distance_squared = (pos_to_check - normal_point).MagSquared();

		if (distance_squared < min_distance_squared)
		{
			steer = {0, 0};
			target = {0, 0};
			best_normal_point = normal_point;

			min_distance_squared = distance_squared;
			if (distance_squared > (segment.size * segment.size))
			{
				auto path_dir = segment.p2 - segment.p1;
				path_dir.Normalize();
				target = normal_point + path_dir * 50.0;
				//DrawCircle(target.x, target.y, 20, olc::RED);

				steer = Seek(target);
			}
		}
	}

	//DrawLine(pos_to_check.x, pos_to_check.y, best_normal_point.x, best_normal_point.y, olc::GREY);

	return steer;
}

Vector2D Vehicle::Align(VehicleStorage vehicles)
{
	Vector2D desired{}, steer{};

	for (auto vehicle : vehicles)
	{
		desired = desired + vehicle.get().vel;
	}

	if (!vehicles.empty())
	{
		desired.SetMag(max_velocity);

		//steer = desired - vel;
		steer = (desired - vel) / max_velocity * max_force;
		steer.Limit(max_force);
	}

	return steer;
}

Vector2D Vehicle::Flee(VehicleStorage vehicles)
{
	Vector2D desired{};

	for (auto vehicle : vehicles)
	{
		desired = desired + pos - vehicle.get().pos;
	}

	desired.SetMag(max_velocity);

	//auto steer = desired - vel;
	auto steer = (desired - vel) / max_velocity * max_force;
	steer.Limit(max_force);

	return steer;
}

Vector2D Vehicle::Separate(VehicleStorage vehicles)
{
	Vector2D desired{}, steer{};
	int count{};

	for (auto vehicle : vehicles)
	{
		auto desired_separation = 1.0 * (size + vehicle.get().size);
		auto diff = pos - vehicle.get().pos;
		auto distance = diff.MagSquared();
		if (distance < (desired_separation * desired_separation))
		{
			diff.Normalize();
			diff = diff / distance;
			desired = desired + diff;
			count++;
		}
	}

	if (count)
	{
		desired.SetMag(max_velocity);
		//steer = desired - vel;
		steer = (desired - vel) / max_velocity * max_force;
		steer.Limit(max_force);
	}

	return steer;
}

Vector2D Vehicle::Cohesion(VehicleStorage vehicles)
{
	Vector2D desired_position{}, steer{};

	for (auto vehicle : vehicles)
	{
		desired_position = desired_position + vehicle.get().pos;
	}
	if (!vehicles.empty())
	{
		desired_position = desired_position / vehicles.size();
		steer = Seek(desired_position);
	}

	return steer;
}

Vector2D Vehicle::Wander()
{
	auto perlin = Noise::noise(pos.x / 1000.0, pos.y / 1000.0, health / 1000.0);
	auto next_direction = perlin * M_PI * 2.0;

	Vector2D desired{sin(next_direction), cos(next_direction)};
	desired.SetMag(0.25 * max_velocity);

	//auto steer = desired - vel;
	auto steer = (desired - vel) / max_velocity * max_force;
	steer.Limit(max_force);

	return steer;
}

void Vehicle::FireGun()
{
	if (!guns_allowed || gun_shot)
		return;
	gun_shot = true;

	gun_shell_lifetime = 0.0;
	gun_shell_pos = pos + last_heading * 2 * size; //start it a bit away, don't hit yourself :D
	gun_shell_vel = last_heading;
	gun_shell_vel.SetMag(gun_shell_max_velocity);
}

bool Vehicle::GunSensor(const std::vector<Vehicle> &vehicles)
{
	if (!guns_allowed)
		return false;

	for (auto vehicle : vehicles)
	{
		auto distance_squared = (vehicle.pos - pos).MagSquared();
		auto is_cone_distance_ok = (distance_squared > 0) && (distance_squared < (gun_radius * gun_radius));

		auto a = vehicle.pos - pos;
		auto b = last_heading;
		auto angle_to_vehicle = acos(a.Dot(b) / (a.Mag() * b.Mag()));
		auto is_angle_ok = angle_to_vehicle < gun_angle;

		auto is_enemy_clan = CheckEnemyClan(vehicle);

		if (is_cone_distance_ok && is_angle_ok && is_enemy_clan)
			return true;
	}

	return false;
}

void Vehicle::CheckForHits(const std::vector<Vehicle> &vehicles)
{
	for (auto vehicle : vehicles)
	{
		if (CheckEnemyClan(vehicle) && vehicle.gun_shot)
		{
			auto distance_squared = (vehicle.gun_shell_pos - pos).MagSquared();
			if (distance_squared < (vehicle.gun_shell_radius + size) * (vehicle.gun_shell_radius + size))
			{
				// we are hit!
				vehicle.gun_shot = false;
				health = 0.0;
			}
		}
	}
}

Vehicle Vehicle::Reproduce(const VehicleStorage &vehicles)
{
	auto parent{*this};
	if (!reproduction_ready)
		return parent;

	for (auto &vehicle : vehicles)
	{
		if (!vehicle.get().reproduction_ready)
			continue;

		if (CheckInDistance(vehicle.get(), reproduction_radius + vehicle.get().reproduction_radius))
		{
			auto reproduction_rand = (double)rand() / RAND_MAX;
			if (reproduction_rand < reproduction_chance)
			{
				reproduction_time = 0.0;
				reproduction_ready = false;

				vehicle.get().reproduction_time = 0.0;
				vehicle.get().reproduction_ready = false;
				return vehicle.get();
			}
		}
	}

	return parent;
}
