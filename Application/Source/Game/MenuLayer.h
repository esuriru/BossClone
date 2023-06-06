#include "Core/Layer.h"
#include "Core/Core.h"
#include "Renderer/Texture.h"

class MenuLayer : public Layer
{
public:
    MenuLayer();
    virtual ~MenuLayer() = default;

    virtual auto OnAttach() -> void override;
    virtual auto OnDetach() -> void override;

    auto OnUpdate(Timestep ts) -> void override;
    auto OnEvent(Event& e) -> void override;
    auto OnImGuiRender() -> void override;

private:
    Ref<Texture2D> mapPreview_; 
    bool firstTime_;
};