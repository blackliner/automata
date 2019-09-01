#pragma once

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "PathSegment.h"
#include "Vector2D.h"
#include "Vehicle.h"

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

	const bool guns_allowed{false};

	std::vector<Vehicle> vehicles = {Vehicle(), Vehicle{nWorldWidth / 2, nWorldHeight / 2}}; //nr1 is our mouse, nr 2 is roby
	std::vector<Vehicle> new_born;
	std::vector<PathSegment> path;

	Vector2D<VectorT> first_node{};
	bool first_node_valid{};
	double t_last_pressed{};
	bool time_freeze{};

public:
	Maze() { sAppName = "Automata"; }

	template <typename T>
	void DrawArrow(const Vector2D<T> start, const Vector2D<T> end, const olc::Pixel color) const;

	void DrawVehicleVelocity(const Vehicle &vehicle) const;

	void DrawVehicleAcceleration(const Vehicle &vehicle) const;

	template <typename T>
	void DrawSteering(const Vehicle &vehicle, const Vector2D<T> steer) const;

	void WrapVehiclePositions(Vehicle &vehicle);

	void DrawVehicle(const Vehicle &vehicle) const;

	void DrawCircleSensor(const Vehicle &vehicle) const;

	void DrawAngularSensor(const Vehicle &vehicle) const;

	void DrawVehicleSensor(const Vehicle &vehicle) const;

	void AddNewVehicle(double x, double y, const VehicleType &type);

	void AddNewPathNode(VectorT x, VectorT y);

	bool OnUserUpdate(float fElapsedTime) override;

	bool OnUserCreate() override { return true; }
};