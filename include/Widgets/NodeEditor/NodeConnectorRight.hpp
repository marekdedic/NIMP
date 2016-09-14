#pragma once

#include "Global.hpp"

class NodeGraphics;

class NodeConnectorRight : public QWidget
{
public:
    NodeConnectorRight(NodeGraphics* parent, NodeOutput* output, int y);
protected:
    NodeOutput* output;
    virtual void paintEvent(QPaintEvent*);
};
