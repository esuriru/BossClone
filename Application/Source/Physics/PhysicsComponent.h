struct PlayerController2DComponent
{
    float HorizontalForce = 5.f;
    float JumpForce = 5.f; 
};

struct RigidBody2DComponent
{
    float Restitution;
    float Mass;
};