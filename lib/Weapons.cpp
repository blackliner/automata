#include "Weapons.h"

void Rifle::FireGun(Vector2D<VectorT> pos, Vector2D<VectorT> heading, double size)
{
    if (gun_shot)
        return;
    gun_shot = true;

    gun_shell_lifetime = 0.0;
    gun_shell_pos = pos + heading * 2 * size; //start it a bit away, don't hit yourself :D
    gun_shell_vel = heading;
    gun_shell_vel.SetMag(gun_shell_max_velocity);
}

void Rifle::Update(double delta_t)
{
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

bool Rifle::CheckHit(Vector2D<VectorT> pos, double size)
{
    auto distance_squared = (gun_shell_pos - pos).MagSquared();
    if (distance_squared < (gun_shell_radius + size) * (gun_shell_radius + size))
    {
        // we are hit!
        gun_shot = false;

        return true;
    }

    return false;
}
