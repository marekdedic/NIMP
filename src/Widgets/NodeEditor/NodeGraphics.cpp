#include "Widgets/NodeEditor/NodeGraphics.hpp"

#include "NodeSystem/Node.hpp"
#include "NodeSystem/NodeInput.hpp"
#include "NodeSystem/NodeOutput.hpp"
#include "NodeSystem/NodeNotifier.hpp"
#include "Registry.hpp"
#include "WidgetActions/States/ActionState.hpp"
#include "Widgets/NodeEditor.hpp"

const int NodeGraphics::headerHeight{Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"] + Registry::getRegistry()->extrinsic->GUI->dimensions["NodeBorderWidth"] + Registry::getRegistry()->extrinsic->GUI->dimensions["NodeHeaderHeight"]};
const int NodeGraphics::footerHeight{Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"] + Registry::getRegistry()->extrinsic->GUI->dimensions["NodeConnectorSpacing"]};

NodeGraphics::NodeGraphics(NodeEditor* parent, Node* node) : Draggable(parent), inputs{}, outputs{}, interfaces{}, node{node}, interfaceHeight{}, connectionHeight{}, insertHighlight{}
{
    setFocusPolicy(Qt::ClickFocus);
    resize(200, height());
    move(node->getPos());
    show();
    (*state->palette)["border"] = std::make_tuple("NodeBorder", "NodeBorderActive");
    QObject::connect(node->notifier, &NodeNotifier::QtMoved, this, &NodeGraphics::reposition);
    QObject::connect(node->notifier, &NodeNotifier::QtDeleted, this, &NodeGraphics::destruct);
}

void NodeGraphics::addInterfaces()
{
    interfaceHeight = 0;
    for(std::vector<NodeInterface*>::iterator it{node->interfaces.begin()}; it != node->interfaces.end(); ++it)
    {
        interfaceHeight += Registry::getRegistry()->extrinsic->GUI->dimensions["NodeInterfaceSpacing"];
        NodeInterfaceGraphics* newItem{NodeInterfaceGraphics::interfaceFactory(this, *it, headerHeight + interfaceHeight)};
        interfaces.push_back(newItem);
        interfaceHeight += newItem->height();
    }
    rebuildConnections();
}

void NodeGraphics::addConnections()
{
    connectionHeight = 0;
    for(std::vector<NodeInput*>::iterator it{node->inputs.begin()}; it != node->inputs.end(); ++it)
    {
        connectionHeight += Registry::getRegistry()->extrinsic->GUI->dimensions["NodeConnectorSpacing"];
        NodeInputGraphics* newItem{new NodeInputGraphics{this, *it, headerHeight + interfaceHeight + connectionHeight}};
        inputs.push_back(newItem);
        newItem->connect();
        connectionHeight += newItem->height();
    }
    for(std::vector<NodeOutput*>::iterator it{node->outputs.begin()}; it != node->outputs.end(); ++it)
    {
        connectionHeight += Registry::getRegistry()->extrinsic->GUI->dimensions["NodeConnectorSpacing"];
        NodeOutputGraphics* newItem{new NodeOutputGraphics{this, *it, headerHeight + interfaceHeight + connectionHeight}};
        outputs.push_back(newItem);
        newItem->connect();
        connectionHeight += newItem->height();
    }
    resize(width(), headerHeight + interfaceHeight + connectionHeight + footerHeight);
    reMask();
}

void NodeGraphics::rebuildInterfaces()
{
    removeInterfaces();
    addInterfaces();
}

void NodeGraphics::rebuildConnections()
{
    removeConnections();
    addConnections();
}

void NodeGraphics::connect(NodeOutputGraphics* left, NodeInputGraphics* right)
{
    Node::connect(left->output, right->input);
}

NodeGraphics::~NodeGraphics()
{
    NodeEditor* editor{dynamic_cast<NodeEditor*>(parentWidget())};
    if(editor != nullptr)
    {
        std::vector<NodeGraphics*>& vec{editor->nodes};
        vec.erase(std::remove(vec.begin(), vec.end(), this), vec.end());
    }
	else
	{
		// TODO: DIE HORRIBLY IN FLAMES
	}
}

void NodeGraphics::reposition()
{
    move(node->getPos());
}

void NodeGraphics::destruct()
{
    delete this;
}

void NodeGraphics::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Registry::getRegistry()->extrinsic->GUI->keys["Delete"] and getState() == ActionWidget::State::SELECTED)
    {
        event->accept();
        deleteNode();
    }
    else
    {
        event->ignore();
    }
}

void NodeGraphics::moveEvent(QMoveEvent*)
{
    reMask();
    updateConnections();
}

void NodeGraphics::mouseMoveEvent(QMouseEvent* event)
{
	Draggable::mouseMoveEvent(event);
	if(inputs.size() > 0 and outputs.size() > 0 and inputs[0]->connection == nullptr and outputs[0]->connections.empty())
	{
		if(insertHighlight != nullptr)
		{
			insertHighlight->changeState(ActionWidget::State::DEFAULT);
		}
		NodeEditor* editor{dynamic_cast<NodeEditor*>(parentWidget())};
		if(editor == nullptr)
		{
			// TODO: DIE HORRIBLY IN FLAMES
			return;
		}
		for(std::unordered_set<NodePath*>::iterator it{editor->paths.begin()}; it != editor->paths.end(); ++it)
		{
			QPainterPath path{};
			path.setFillRule(Qt::WindingFill);
			QRegion* mask{state->getMask()};
			mask->translate(pos());
			path.addRegion(*mask);
			if(((*it)->path->intersects(path)) and ((*it)->left->output->type == inputs[0]->input->type) and ((*it)->right->input->type == outputs[0]->output->type))
			{
				(*it)->changeState(ActionWidget::State::HIGHLIGHTED);
				insertHighlight = *it;
				break;
			}
		}
	}
}

void NodeGraphics::mouseReleaseEvent(QMouseEvent* event)
{
    Draggable::mouseReleaseEvent(event);
    node->setPos(pos());
	insertHighlight = nullptr;
	if(inputs.size() > 0 and outputs.size() > 0 and inputs[0]->connection == nullptr and outputs[0]->connections.empty())
	{
		NodeEditor* editor{dynamic_cast<NodeEditor*>(parentWidget())};
		if(editor == nullptr)
		{
			// TODO: DIE HORRIBLY IN FLAMES
			return;
		}
		for(std::unordered_set<NodePath*>::iterator it{editor->paths.begin()}; it != editor->paths.end(); ++it)
		{
			QPainterPath path{};
			path.setFillRule(Qt::WindingFill);
			QRegion* mask{state->getMask()};
			mask->translate(pos());
			path.addRegion(*mask);
			if(((*it)->path->intersects(path)) and ((*it)->left->output->type == inputs[0]->input->type) and ((*it)->right->input->type == outputs[0]->output->type))
			{
				NodeOutput* left{(*it)->left->output};
				NodeInput* right{(*it)->right->input};
				Node::disconnect(left, right);
				Node::connect(left, inputs[0]->input);
				Node::connect(outputs[0]->output, right);
				break;
			}
		}
	}
}

void NodeGraphics::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath border{};
    border.addRoundedRect(QRectF(Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"] - 0.5, Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"] - 0.5, width() - 2 * Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"], height() - 2 * Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"]), Registry::getRegistry()->extrinsic->GUI->dimensions["NodeCornerRadius"], Registry::getRegistry()->extrinsic->GUI->dimensions["NodeCornerRadius"]);
    QPen borderPen{state->getColour("border"), static_cast<double>(Registry::getRegistry()->extrinsic->GUI->dimensions["NodeBorderWidth"])};
    QPen separatorPen{Registry::getRegistry()->extrinsic->GUI->palette["NodeHeaderSeparator"], static_cast<double>(Registry::getRegistry()->extrinsic->GUI->dimensions["NodeHeaderSeparatorHeight"])};
    QPen textPen{Registry::getRegistry()->extrinsic->GUI->palette["NodeHeaderText"]};
    painter.setPen(borderPen);
    painter.fillPath(border, Registry::getRegistry()->extrinsic->GUI->palette["NodeBackground"]);
    painter.drawPath(border);
    painter.setPen(separatorPen);
    painter.drawLine(Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"] + Registry::getRegistry()->extrinsic->GUI->dimensions["NodeBorderWidth"], Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"] + Registry::getRegistry()->extrinsic->GUI->dimensions["NodeBorderWidth"] + Registry::getRegistry()->extrinsic->GUI->dimensions["NodeHeaderHeight"], width() - Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"] - Registry::getRegistry()->extrinsic->GUI->dimensions["NodeBorderWidth"] - 1, Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"] + Registry::getRegistry()->extrinsic->GUI->dimensions["NodeBorderWidth"] + Registry::getRegistry()->extrinsic->GUI->dimensions["NodeHeaderHeight"]);
    painter.setPen(textPen);
    painter.drawText(2 * Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"], Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"] + Registry::getRegistry()->extrinsic->GUI->dimensions["NodeHeaderHeight"] - 8, QString::fromStdString(node->nodeName()));
}

void NodeGraphics::reMask()
{
    QPainterPath border{};
    border.addRoundedRect(QRectF(Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"] - 0.5, Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"] - 0.5, width() - 2 * Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"], height() - 2 * Registry::getRegistry()->extrinsic->GUI->dimensions["NodeMargin"]), Registry::getRegistry()->extrinsic->GUI->dimensions["NodeCornerRadius"], Registry::getRegistry()->extrinsic->GUI->dimensions["NodeCornerRadius"]);
    state->setMask(&border);
}

void NodeGraphics::removeInterfaces()
{
    for(std::vector<NodeInterfaceGraphics*>::iterator it{interfaces.begin()}; it != interfaces.end();)
    {
        delete (*it);
        it = interfaces.erase(it);
    }
    interfaceHeight = 0;
}

void NodeGraphics::removeConnections()
{
    for(std::vector<NodeInputGraphics*>::iterator it{inputs.begin()}; it != inputs.end();)
    {
        (*it)->disconnect();
        delete (*it);
        it = inputs.erase(it);
    }
    for(std::vector<NodeOutputGraphics*>::iterator it{outputs.begin()}; it != outputs.end();)
    {
        (*it)->disconnect();
        delete (*it);
        it = outputs.erase(it);
    }
    connectionHeight = 0;
}

void NodeGraphics::updateConnections()
{
    for(std::vector<NodeInputGraphics*>::iterator it{inputs.begin()}; it != inputs.end(); ++it)
    {
        if((*it)->connection != nullptr)
        {
            (*it)->connection->reposition();
        }
    }
    for(std::vector<NodeOutputGraphics*>::iterator it{outputs.begin()}; it != outputs.end(); ++it)
    {
        for(std::unordered_set<NodePath*>::iterator jt{(*it)->connections.begin()}; jt != (*it)->connections.end(); ++jt)
        {
            (*jt)->reposition();
        }
    }
}

void NodeGraphics::deleteNode()
{
    delete node;
}
