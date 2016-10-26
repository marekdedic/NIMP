#include "Widgets/NodeEditor/NodeOutputGraphics.hpp"

#include "NodeSystem/NodeOutput.hpp"
#include "Registry.hpp"
#include "Widgets/NodeEditor.hpp"

NodeOutputGraphics::NodeOutputGraphics(NodeGraphics* parent, NodeOutput* output, int y) : QWidget(parent), connections{}, output{output}, connector{new NodeIOConnector{this, parentWidget()->width() - Registry::getRegistry()->extrinsic->GUI->dimensions["NodeConnectorDiameter"]}}
{
    resize(parent->width(), Registry::getRegistry()->extrinsic->GUI->dimensions["NodeConnectorDiameter"]);
    move(0, y);
}

void NodeOutputGraphics::connect()
{
    if(connections.empty())
    {
        NodeEditor* editor{dynamic_cast<NodeEditor*>(parentWidget()->parentWidget())};
        for(std::vector<NodeInput*>::iterator it{output->connections.begin()}; it != output->connections.end(); it++)
        {
            NodeInputGraphics* inputConnector{nullptr};
            for(std::vector<NodeGraphics*>::iterator jt{editor->nodes.begin()}; jt != editor->nodes.end(); jt++)
            {
                for(std::vector<NodeInputGraphics*>::iterator kt{(*jt)->inputs.begin()}; kt != (*jt)->inputs.end(); kt++)
                {
                    if((*kt)->input == (*it))
                    {
                        inputConnector = *kt;
                    }
                }
            }
            if(inputConnector != nullptr)
            {
                editor->paths.push_back(new NodePath{editor, this, inputConnector});
            }
        }
    }
}

void NodeOutputGraphics::disconnect()
{
    for(std::vector<NodePath*>::iterator it{connections.begin()}; it != connections.end(); it++)
    {
        (*it)->right->connection = nullptr;
        delete *it;
    }
    connections.clear();
}

void NodeOutputGraphics::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setRenderHint(QPainter::Antialiasing);
    QPen textPen{Registry::getRegistry()->extrinsic->GUI->palette["NodeConnectorText"]};
    painter.setPen(textPen);
    int textWidth{painter.fontMetrics().width(QString::fromStdString(output->name))};
    painter.drawText(parentWidget()->width() - Registry::getRegistry()->extrinsic->GUI->dimensions["NodeConnectorDiameter"] - textWidth - 5, Registry::getRegistry()->extrinsic->GUI->dimensions["NodeConnectorDiameter"] - 5, QString::fromStdString(output->name));
}
