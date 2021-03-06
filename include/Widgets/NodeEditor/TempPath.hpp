#pragma once

#include "Global.hpp"

#include "WidgetActions/Selectable.hpp"

class NodeEditor;
class NodeOutputGraphics;
class NodeInputGraphics;

class TempPath : public Selectable
{
    Q_OBJECT
    Q_DISABLE_COPY(TempPath)
public:
    NodeOutputGraphics* left;
    NodeInputGraphics* right;
    TempPath(NodeEditor* parent, NodeInputGraphics* right);
    TempPath(NodeEditor* parent, NodeOutputGraphics* left);
    ~TempPath();
protected:
    QPainterPath* path;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;
};
