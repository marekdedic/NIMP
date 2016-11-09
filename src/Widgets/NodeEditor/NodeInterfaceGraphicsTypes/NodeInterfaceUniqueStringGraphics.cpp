#include "Widgets/NodeEditor/NodeInterfaceGraphicsTypes/NodeInterfaceUniqueStringGraphics.hpp"

#include "NodeSystem/NodeInterfaceTypes/NodeInterfaceUniqueString.hpp"
#include "Registry.hpp"

NodeInterfaceUniqueStringGraphics::NodeInterfaceUniqueStringGraphics(NodeGraphics* parent, NodeInterfaceUniqueString* interface, int y) : NodeInterfaceGraphics{parent, y}, interface{interface}, textbox{new QLineEdit{QString::fromStdString(interface->getValue()), this}}
{
    textbox->setStyleSheet("QLineEdit {border-color: red;}");
    textbox->installEventFilter(this);
    resize(width(), textbox->height());
    show();
    QObject::connect(textbox, &QLineEdit::editingFinished, this, &NodeInterfaceUniqueStringGraphics::triggered);
    QObject::connect(interface, &NodeInterfaceUniqueString::valueChanged, this, &NodeInterfaceUniqueStringGraphics::valueChanged);
}

void NodeInterfaceUniqueStringGraphics::triggered()
{
    interface->setValue(textbox->text().toStdString());
}

void NodeInterfaceUniqueStringGraphics::valueChanged()
{
    textbox->setText(QString::fromStdString(interface->getValue()));
}