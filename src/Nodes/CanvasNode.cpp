#include "Nodes/CanvasNode.hpp"

#include "Texture.hpp"
#include "NodeSystem/NodeInputTypes/NodeInputImage.hpp"
#include "NodeSystem/NodeInterfaceTypes/NodeInterfaceUniqueString.hpp"

CanvasNode::CanvasNode(int x, int y) : Node(x, y)
{
    inputs.push_back(new NodeInputImage{this, "Image"});
    interfaces.push_back(new NodeInterfaceUniqueString{this, "Display?"});
    QObject::connect(inputs[0], &NodeInput::reconnected, this, &CanvasNode::inputsReconnected);
}

std::string CanvasNode::nodeName()
{
    return "Canvas";
}

Texture* CanvasNode::getTexture()
{
    NodeDataImage* data{dynamic_cast<NodeDataImage*>(inputs[0]->getData())};
    if(data != nullptr)
    {
        Texture* output{new Texture{}};
        glBindTexture(GL_TEXTURE_2D, output->ID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data->width, data->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data->rawData());
        return output;
    }
    // TODO: DIE HORRIBLY IN FLAMES
    return nullptr;
}

void CanvasNode::inputsReconnected()
{
    emit reconnected();
}
