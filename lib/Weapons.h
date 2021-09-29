
#pragma once

#include "Vector2D.h"

class IWeapon {
 public:
  virtual void FireGun(Vector2D<VectorT> pos, Vector2D<VectorT> heading, double size) = 0;
  virtual void Update(double delta_t) = 0;
  virtual bool CheckHit(Vector2D<VectorT> pos, double size) = 0;

  virtual double GetGunAngle() const = 0;
  virtual double GetGunRange() const = 0;
  virtual bool GetGunShot() const = 0;
  virtual Vector2D<VectorT> GetPos() const = 0;
  virtual double GetShellRadius() const = 0;
};

class Rifle : public IWeapon {
 private:
  Vector2D<VectorT> gun_shell_pos{};
  Vector2D<VectorT> gun_shell_vel{};

  double gun_range{400.0};
  double gun_angle{M_PI / 180.0 * 2.5};
  double gun_shell_max_velocity{500.0};
  double gun_shell_max_lifetime{1.0};
  double gun_shell_lifetime{};
  double gun_shell_radius{3.0};
  bool gun_shot{};

 public:
  void FireGun(Vector2D<VectorT> pos, Vector2D<VectorT> heading, double size) override;
  void Update(double delta_t);
  bool CheckHit(Vector2D<VectorT> pos, double size);

  double GetGunAngle() const {
    return gun_angle;
  }
  double GetGunRange() const {
    return gun_range;
  }
  bool GetGunShot() const {
    return gun_shot;
  }
  Vector2D<VectorT> GetPos() const {
    return gun_shell_pos;
  }
  double GetShellRadius() const {
    return gun_shell_radius;
  }
};
