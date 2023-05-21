#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "camera_node.hpp"
#include "geometry_node.hpp"
#include "model.hpp"
#include "scene_graph.hpp"
#include "structs.hpp"

// gpu representation of model
class ApplicationSolar : public Application
{
public:
    // allocate and initialize objects
    ApplicationSolar(std::string const &resource_path);
    // free allocated objects
    ~ApplicationSolar();

    // react to key input
    void keyCallback(int key, int action, int mods);
    //handle delta mouse movement input
    void mouseCallback(double pos_x, double pos_y);
    //handle resizing
    void resizeCallback(unsigned width, unsigned height);

    // draw all objects
    void render() const;
    // draw single planet
    void renderPlanet(std::shared_ptr<GeometryNode> planet) const;
    void drawStarts() const;

protected:
    void initializeShaderPrograms();
    void initializeGeometry();
    // update uniform values
    void uploadUniforms();
    // upload projection matrix
    void uploadProjection(std::string shader_name);
    // upload view matrix
    void uploadView(std::string shader_name);

    // create Scene Graph
    void initializeSolarSystem();
    // create single planet
    void createNewPlanet(std::string const &name,
                         std::shared_ptr<Node> const &parent,
                         float distance,
                         float size,
                         float speed);
    // create stars
    void initializeStars();
    // init orbits
    void initializeOrbits();
    // render orbits
    //void renderOrbits(Node *const &child_planet) const;
    void drawOrbits(std::shared_ptr<GeometryNode> childPlanet) const;

    // cpu representation of model
    model_object planet_object;
    model_object star_object;
    model_object orbit_object;

    // camera transform matrix
    glm::fmat4 m_view_transform;
    // camera projection matrix
    glm::fmat4 m_view_projection;

private:
    SceneGraph planetarySystem_;
    std::vector<float> stars_container;
    std::vector<GLfloat> orbit_container;
};

#endif
