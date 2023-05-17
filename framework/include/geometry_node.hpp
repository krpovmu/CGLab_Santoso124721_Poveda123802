#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include <string>
#include <memory>
#include "node.hpp"

class GeometryNode : public Node {
 public:
  // constructors
  GeometryNode();
  GeometryNode(model const& geometry);
  GeometryNode(
  std::string const& name,
  std::shared_ptr<Node> const& parent,
  glm::fmat4 const& localTansform,
  float size,
  float speed,
  float distance
  );
  
  // get attribute methods
  model getGeometry() const;
  float getSize() const;
  float getSpeed() const;
  float getDistance() const;

  // set attribute methods
  void setGeometry(model const& geometry);
 
 private:
  // attributes
  model geometry_;
  float size_;
  float speed_;
  float distance_;
};

#endif
