#pragma once

#include "Global.hpp"

#include "WidgetActions/Selectable.hpp"

class NodeEditor;
class NodeGraphics;
class NodeOutputGraphics;
class NodeInputGraphics;
class NodePathCutter;

class NodePath : public Selectable
{
    Q_OBJECT
    Q_DISABLE_COPY(NodePath)
public:
    NodeOutputGraphics* left;
    NodeInputGraphics* right;
    NodePath(NodeEditor* parent, NodeOutputGraphics* left, NodeInputGraphics* right);
    void reposition();
    void disconnect();
    ~NodePath();
protected:
	friend NodeGraphics;
    friend NodePathCutter;
    QPainterPath* path;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void paintEvent(QPaintEvent*) override;
};
