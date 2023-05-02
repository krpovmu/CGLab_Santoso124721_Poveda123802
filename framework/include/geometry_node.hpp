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

	/*float size_;
	float speed_;
	float distance_;*/

 public:
  // constructors
  GeometryNode();
  
  GeometryNode(
	  model const& geometry
  );
  
  GeometryNode(
	  std::shared_ptr<Node> const& parent,
	  std::string const& name
	  //, glm::fmat4 const& localTansform
	  /*,float size,
	  float speed,
	  float distance*/
  );
  
  // getter
  model getGeometry() const;

  // setter
  void setGeometry(model const& geometry);

  // TESTING
  /*float getSize() const;
  float getSpeed() const;
  float getDistance() const;*/

};

#endif
