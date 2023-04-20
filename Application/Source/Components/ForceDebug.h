#include "Component.h"

class RigidBody;
class ForceDebug : public Component
{
public:
	ForceDebug(GameObject* go);

	void Update(double dt) {}
	void UpdateGravity() override;
	void FixedUpdate() override;
private:
	RigidBody* rigidbody_;
};