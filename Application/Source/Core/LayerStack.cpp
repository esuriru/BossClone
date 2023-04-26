#include "LayerStack.h"
#include "Layer.h"

LayerStack::LayerStack()
	: layerIterator_(layers_.begin())
{
}

LayerStack::~LayerStack()
{
	for (const Layer* layer : layers_)
		delete layer;
}

void LayerStack::PushLayer(Layer* layer)
{
	layerIterator_ = layers_.emplace(layerIterator_, layer);
	layer->OnAttach();
}

void LayerStack::PushOverlay(Layer* overlay)
{
	layers_.emplace_back(overlay);
	overlay->OnAttach();
}

void LayerStack::PopLayer(Layer* layer)
{
	// TODO: Might want to cache this instead, but it might not be called enough.
	auto it = std::find(layers_.begin(), layers_.end(), layer);
	if (it != layers_.end())
	{
		layers_.erase(it);
		--layerIterator_;
	}
	layer->OnDetach();
}

void LayerStack::PopOverlay(Layer* overlay)
{
	auto it = std::find(layers_.begin(), layers_.end(), overlay);
	if (it != layers_.end())
		layers_.erase(it);

	overlay->OnDetach();
}
