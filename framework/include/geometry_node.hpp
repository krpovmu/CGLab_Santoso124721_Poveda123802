#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include <string>
#include <memory>
#include <model.hpp>
#include "node.hpp"

class GeometryNode : public Node {
private:
	// attributes
	model geometry_;

 public:
  // constructors
  GeometryNode();
  
  GeometryNode(
	  model const& geometry
  );
  
  GeometryNode(
	  std::shared_ptr<Node> const& parent,
	  std::string const& name
  );
  
  // getter
  model getGeometry() const;

  // setter
  void setGeometry(model const& geometry);

};

#endif
