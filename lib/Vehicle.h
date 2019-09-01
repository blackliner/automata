#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>

#include "PathSegment.h"
#include "Perlin.h"
#include "Vector2D.h"
#include "Weapons.h"

class Vehicle;

using VehicleStorage = std::vector<std::reference_wrapper<Vehicle>>;

enum class VehicleType : std::uint8_t
{
	ARROW,
	CIRCLE,
	FLY,
	BIRD
};

enum class SensorType : std::uint8_t
{
	CIRCLE,
	ANGULAR,
	BOTH
};

enum class Clan : std::uint8_t
{
	RED,
	BLUE
};

double Map(double value, double from_start, double from_end, double to_start, double to_end);

class Vehicle
{
private:
	static int current_id;

	void InitVehicle();

public:
	Vehicle();
	Vehicle(double x, double y);

	int ID{};

	//  ---------------------state-------------------------

	Vector2D<VectorT> pos{};
	Vector2D<VectorT> vel{};
	Vector2D<VectorT> acc{};
	Vector2D<VectorT> last_acc{};			  //for visu purpose
	Vector2D<VectorT> last_heading{0.0, 1.0}; //to know its orientation if speed = 0; always unit vector

	//  ---------------------config-------------------------

	double max_velocity{200}; // pixel / second
	double force_gain{100.0};
	double max_force{1000}; // velocity / second
	double size{20};
	double sensor_circle_radius{50.0};
	double sensor_angle{0.4 * M_PI * 2.0};
	double sensor_cone_radius{300.0};

	SensorType sensor_type = SensorType::BOTH;

	VehicleType vehicle_type = VehicleType::CIRCLE;

	double health = 10.0;

	Clan clan{};
	// -----------------Equipment----------------

	Rifle weapon{};
	// std::unique_ptr<IWeapon> weapon{std::make_unique<Rifle>()};

	// Vector2D<VectorT> gun_shell_pos{};
	// Vector2D<VectorT> gun_shell_vel{};

	//bool operator==(Clan rhs){return }

	// double gun_radius{400.0};
	// double gun_angle{M_PI / 180.0 * 2.5};
	// double gun_shell_max_velocity{500.0};
	// double gun_shell_max_lifetime{1.0};
	// double gun_shell_lifetime{};
	// double gun_shell_radius{3.0};
	// bool gun_shot{};

	// -----------------reproduction--------------
	double reproduction_radius{2 * size};
	double reproduction_chance{0.01};
	double reproduction_time{};
	double reproduction_waiting_time{5};
	double reproduction_ready{};

	// -----------------operators-----------------

	bool operator==(const Vehicle &v) { return ID == v.ID; };
	bool operator!=(const Vehicle &v) { return !(*this == v); };

	// --------------stuff --------------

	void SetType(const VehicleType &type);

	static VehicleType GetRandomType();

	void update(double delta_t = 1.0);

	Vector2D<VectorT> PredictedPos() const;

	void applyForce(Vector2D<VectorT> force, double delta_t = 1.0);

	VehicleStorage FindClosestVehicle(const VehicleStorage &vehicles);

	VehicleStorage FindClosestMatingPartner(const VehicleStorage &vehicles);

	bool CheckEnemyClan(Vehicle vehicle);

	bool CheckInDistance(Vehicle vehicle, double distance);

	bool CheckInCone(Vehicle vehicle, double angle);

	VehicleStorage CircularSensor(std::vector<Vehicle> &vehicles);

	VehicleStorage AngularSensor(std::vector<Vehicle> &vehicles);

	VehicleStorage BothSensor(std::vector<Vehicle> &vehicles);

	VehicleStorage ScanForVehiclesInRange(std::vector<Vehicle> &vehicles);

	Vector2D<VectorT> Seek(const Vector2D<VectorT> target_pos) const;

	Vector2D<VectorT> Arrive(const Vector2D<VectorT> target_pos) const;

	Vector2D<VectorT> FollowPath(const std::vector<PathSegment> &path) const;

	Vector2D<VectorT> Align(const VehicleStorage &vehicles) const;

	Vector2D<VectorT> Flee(const VehicleStorage &vehicles) const;

	Vector2D<VectorT> Separate(const VehicleStorage &vehicles) const;

	Vector2D<VectorT> Cohesion(const VehicleStorage &vehicles) const;

	Vector2D<VectorT> Wander() const;

	bool IsAlive() const { return health > 0; }

	// void FireGun();

	bool GunSensor(const std::vector<Vehicle> &vehicles);

	void CheckForHits(const std::vector<Vehicle> &vehicles);

	Vehicle Reproduce(const VehicleStorage &vehicles);
};

// move to cpp and remove inline
inline int Vehicle::current_id;
