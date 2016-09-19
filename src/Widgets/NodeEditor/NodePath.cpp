#include "Widgets/NodeEditor.hpp"

#include "Registry.hpp"
#include "WidgetActions/States/ActionState.hpp"

NodeEditor::NodePath::NodePath(NodeEditor* parent, NodeConnectorRight* left, NodeConnectorLeft* right) : Selectable(parent), left{left}, right{right}
{
    (*state->palette)["path"] = std::make_tuple("NodePath", "NodePathActive");
    left->path = this;
    right->path = this;
}

void NodeEditor::NodePath::paintEvent(QPaintEvent*)
{
    resize(parentWidget()->size());
    QPainter painter{this};
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path{};
    double yOffset{0.5 * Registry::getRegistry()->extrinsic->GUI->dimensions["NodeConnectorDiameter"]};
    double lX{static_cast<double>(left->parentWidget()->x() + left->x() + left->width())};
    double lY{left->parentWidget()->y() + left->y() + yOffset};
    double rX{static_cast<double>(right->parentWidget()->x() + right->x())};
    double rY{right->parentWidget()->y() + right->y() + yOffset};
    path.moveTo(lX, lY);
    path.cubicTo(lX + Registry::getRegistry()->extrinsic->GUI->dimensions["NodePathSharpness"], lY, rX - Registry::getRegistry()->extrinsic->GUI->dimensions["NodePathSharpness"], rY, rX, rY);
    QPainterPathStroker stroker;
    stroker.setWidth(Registry::getRegistry()->extrinsic->GUI->dimensions["NodePathClickableWidth"]);
    QPainterPath thick{stroker.createStroke(path)};
    setMask(thick.toFillPolygon().toPolygon());
    state->changeMask(&thick);
    QPen pen{state->getColour("path"), Registry::getRegistry()->extrinsic->GUI->dimensions["NodePathWidth"]};
    painter.setPen(pen);
    painter.drawPath(path);
}
