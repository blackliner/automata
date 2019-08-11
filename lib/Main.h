#pragma once

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "Vehicle.h"
#include "Vector2D.h"
#include "PathSegment.h"

constexpr int fBlockWidth = 1;
constexpr int nWorldWidth = 160 * 8;
constexpr int nWorldHeight = 120 * 8;
constexpr int nDisplayWidth = nWorldWidth * fBlockWidth;
constexpr int nDisplayHeight = nWorldHeight * fBlockWidth;

class Maze : public olc::PixelGameEngine
{
private:
	const int vehicle_mouse = 0;
	const int vehicle_roby = 1;

	std::vector<Vehicle> vehicles = {Vehicle(), Vehicle{nWorldWidth / 2, nWorldHeight / 2}}; //nr1 is our mouse, nr 2 is roby
	std::vector<Vehicle> new_born;
	std::vector<PathSegment> path;

	Vector2D first_node{};
	bool first_node_valid{};
	double t_last_pressed{};
	bool time_freeze{};

public:
	Maze() { sAppName = "Automata"; }

	void DrawArrow(const Vector2D start, const Vector2D end, const olc::Pixel color) const;

	void DrawVehicleVelocity(const Vehicle &vehicle) const;

	void DrawVehicleAcceleration(const Vehicle &vehicle) const;

	void DrawSteering(const Vehicle &vehicle, Vector2D steer) const;

	void WrapVehiclePositions(Vehicle &vehicle);

	void DrawVehicle(const Vehicle &vehicle) const;

	void DrawCircleSensor(const Vehicle &vehicle) const;

	void DrawAngularSensor(const Vehicle &vehicle) const;

	void DrawVehicleSensor(const Vehicle &vehicle) const;

	void AddNewVehicle(double x, double y, const VehicleType &type);

	void AddNewPathNode(double x, double y);

	bool OnUserUpdate(float fElapsedTime) override;

	bool OnUserCreate() override { return true; }
};