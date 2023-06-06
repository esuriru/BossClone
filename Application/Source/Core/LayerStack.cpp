#include "LayerStack.h"
#include "Layer.h"

LayerStack::LayerStack()
	: layerInsertIndex_(0)
{
}

LayerStack::~LayerStack()
{
	for (const Layer* layer : layers_)
		delete layer;
}

void LayerStack::PushLayer(Layer* layer)
{
	layers_.emplace(layers_.begin() + layerInsertIndex_, layer);
    layerInsertIndex_++;
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
        layer->OnDetach();
		layers_.erase(it);
        layerInsertIndex_--;
	}
}

void LayerStack::PopOverlay(Layer* overlay)
{
	auto it = std::find(layers_.begin(), layers_.end(), overlay);
	if (it != layers_.end())
		layers_.erase(it);

	overlay->OnDetach();
}
