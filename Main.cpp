#define _USE_MATH_DEFINES
#include <cmath>

#include "Main.h"

void Maze::DrawArrow(const Vector2D start, const Vector2D end, const olc::Pixel color) const
{
	DrawLine(start.x, start.y, end.x, end.y, color); //shaft

	auto shaft = end - start;
	shaft.SetMag(25.0);

	shaft.Rotate(M_PI / 4);
	DrawLine(end.x, end.y, end.x - shaft.x, end.y - shaft.y, color); //left thingy

	shaft.Rotate(-M_PI / 2);
	DrawLine(end.x, end.y, end.x - shaft.x, end.y - shaft.y, color); //right thingy
}

void Maze::DrawVehicleVelocity(const Vehicle &vehicle) const
{
	if (vehicle.vel.MagSquared() > 0.0001)
	{
		DrawArrow(vehicle.pos, vehicle.pos + vehicle.vel, olc::GREEN);
	}
}

void Maze::DrawVehicleAcceleration(const Vehicle &vehicle) const
{
	if (vehicle.last_acc.MagSquared() > 0.0001)
	{
		DrawArrow(vehicle.pos, vehicle.pos + vehicle.last_acc, olc::RED);
	}
}

void Maze::DrawSteering(const Vehicle &vehicle, Vector2D steer) const
{
	DrawArrow(vehicle.pos, vehicle.pos + steer * 10, olc::WHITE);
}

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

void Maze::DrawVehicle(const Vehicle &vehicle) const
{
	olc::Pixel color{};
	switch (vehicle.clan)
	{
	case Clan::BLUE:
		color = olc::BLUE;
		break;
	case Clan::RED:
		color = olc::RED;
		break;
	}

	if (vehicle.reproduction_ready)
		color = olc::DARK_YELLOW;

	switch (vehicle.vehicle_type)
	{
	case VehicleType::BIRD:
	case VehicleType::ARROW:
	{
		auto current_diretion = vehicle.last_heading;
		auto p1 = vehicle.pos + current_diretion * vehicle.size;
		auto p3 = vehicle.pos - current_diretion * vehicle.size / 2.0;
		current_diretion.Rotate(M_PI * 3.0 / 4.0);
		auto p2 = vehicle.pos + current_diretion * vehicle.size;
		current_diretion.Rotate(M_PI * 1.0 / 2.0);
		auto p4 = vehicle.pos + current_diretion * vehicle.size;

		DrawLine(p1.x, p1.y, p2.x, p2.y, color);
		DrawLine(p2.x, p2.y, p3.x, p3.y, color);
		DrawLine(p3.x, p3.y, p4.x, p4.y, color);
		DrawLine(p4.x, p4.y, p1.x, p1.y, color);

		//draw gun
		if (vehicle.gun_shot)
		{
			DrawCircle(vehicle.gun_shell_pos.x, vehicle.gun_shell_pos.y, vehicle.gun_shell_radius, color);
		}

		break;
	}
	case VehicleType::CIRCLE:
	case VehicleType::FLY:
	{
		DrawCircle(vehicle.pos.x, vehicle.pos.y, vehicle.size, color);
		break;
	}
	}
}

void Maze::DrawCircleSensor(const Vehicle &vehicle) const
{
	DrawCircle(vehicle.pos.x, vehicle.pos.y, vehicle.sensor_circle_radius);
}

void Maze::DrawAngularSensor(const Vehicle &vehicle) const
{
	auto left_cone_part = vehicle.last_heading;
	left_cone_part.Rotate(vehicle.sensor_angle);
	left_cone_part.SetMag(vehicle.sensor_cone_radius);
	left_cone_part = left_cone_part + vehicle.pos;

	auto right_cone_part = vehicle.last_heading;
	right_cone_part.Rotate(-vehicle.sensor_angle);
	right_cone_part.SetMag(vehicle.sensor_cone_radius);
	right_cone_part = right_cone_part + vehicle.pos;

	DrawLine(vehicle.pos.x, vehicle.pos.y, left_cone_part.x, left_cone_part.y);
	DrawLine(vehicle.pos.x, vehicle.pos.y, right_cone_part.x, right_cone_part.y);
}

void Maze::DrawVehicleSensor(const Vehicle &vehicle) const
{
	switch (vehicle.sensor_type)
	{
	case SensorType::CIRCLE:
		DrawCircleSensor(vehicle);
		break;
	case SensorType::ANGULAR:
		DrawAngularSensor(vehicle);
		break;
	case SensorType::BOTH:
		DrawCircleSensor(vehicle);
		DrawAngularSensor(vehicle);
		break;
	}
}

void Maze::AddNewVehicle(double x, double y, const VehicleType &type)
{
	Vehicle new_vehicle{x, y};
	new_vehicle.vel.x = 100.0 + (double)rand() / (double)RAND_MAX * 100.0;
	new_vehicle.vel.y = 100.0 + (double)rand() / (double)RAND_MAX * 100.0;
	new_vehicle.SetType(type);
	new_born.push_back(new_vehicle);
}

void Maze::AddNewPathNode(double x, double y)
{
	if (!first_node_valid)
	{
		first_node = Vector2D{x, y};
		first_node_valid = true;
	}
	else
	{
		PathSegment segment;
		if (path.empty())
		{
			segment.p1 = first_node;
			segment.p2 = Vector2D{x, y};
		}
		else
		{
			segment.p1 = path.back().p2;
			segment.p2 = Vector2D{x, y};
		}

		path.push_back(segment);
	}
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

	auto &target = vehicles[vehicle_mouse];
	auto tau = fElapsedTime / (0.2 + fElapsedTime);
	auto last_pos = target.pos;
	auto last_vel = target.vel;
	target.update(fElapsedTime);
	target.pos = {(double)GetMouseX(), (double)GetMouseY()};
	target.vel = (1 - tau) * last_vel + (target.pos - last_pos) / (0.2 + fElapsedTime);

	// add new vehicles
	if (GetMouse(0).bPressed)
	{
		t_last_pressed = 0.0;
		AddNewVehicle((double)GetMouseX(), (double)GetMouseY(), Vehicle::GetRandomType());
	}
	if (GetMouse(0).bHeld)
	{
		DrawCircle((double)GetMouseX(), (double)GetMouseY(), 20);
		FillCircle((double)GetMouseX(), (double)GetMouseY(), Map(t_last_pressed, 0, 1, 0, 20));

		t_last_pressed += fElapsedTime;
		if (t_last_pressed > 1.0)
		{
			AddNewVehicle((double)GetMouseX(), (double)GetMouseY(), Vehicle::GetRandomType());
		}
	}

	// add a path node
	if (GetMouse(1).bPressed)
	{
		AddNewPathNode((double)GetMouseX(), (double)GetMouseY());
	}

	//remove all dead vehicles
	auto end = std::remove_if(vehicles.begin(), vehicles.end(), [](Vehicle v) { return !v.IsAlive(); });
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
		if (vehicle->GunSensor(vehicles))
		{
			vehicle->FireGun();
		}
		vehicle->CheckForHits(vehicles);

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

	//draw target
	DrawVehicle(target);
	DrawVehicleVelocity(target);
	//DrawString(target.pos.x, target.pos.y, std::to_string(target.vel.Mag()), olc::WHITE, 10);

	for (auto &vehicle : vehicles)
	{
		DrawVehicle(vehicle);
	}
	// special treatment for our roby :D
	DrawVehicleSensor(vehicles[vehicle_roby]);
	DrawVehicleVelocity(vehicles[vehicle_roby]);
	//DrawVehicleAcceleration(robot);

	for (auto segment : path)
	{
		DrawLine(segment.p1.x, segment.p1.y, segment.p2.x, segment.p2.y);
	}

	//for (int x = 0; x < nWorldWidth; x++)
	//{
	//	for (int y = 0; y < nWorldHeight; y++)
	//	{
	//	}
	//}

	return true;
}

int main()
{
	Maze demo;
	if (demo.Construct(nDisplayWidth, nDisplayHeight, 1, 1))
		demo.Start();
}