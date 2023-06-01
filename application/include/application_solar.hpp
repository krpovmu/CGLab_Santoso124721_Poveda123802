#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "scene_graph.hpp"
#include "geometry_node.hpp"
#include "camera_node.hpp"
#include "point_light_node.hpp"

// gpu representation of model
class ApplicationSolar : public Application {
public:
    // allocate and initialize objects
    ApplicationSolar(std::string const& resource_path);
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
    void renderPlanet(std::shared_ptr<GeometryNode> planet)const;
    void renderStars()const;
    void renderLightNodes()const;
    void renderOrbits() const;

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
    // create single sun
    void makeSun(
        std::string const& name,
        std::shared_ptr<Node> const& parent,
        float distance,
        float size,
        float speed,
        glm::fvec3 color,
        float light_intensity,
        glm::fvec3 light_color
        );
    // create single planet
    void makePlanet(std::string const& name, std::shared_ptr<Node> const& parent, float distance, float size, float speed, glm::fvec3 color);
    // create stars
    void initializeStars();
    // init orbits
    void initializeOrbits();

    // cpu representation of model
    model_object planet_object;
    model_object star_object;
    model_object orbit_object;

    // camera transform matrix
    glm::fmat4 m_view_transform;
    // camera projection matrix
    glm::fmat4 m_view_projection;
    bool cellShading_Mode;

private:
    SceneGraph solarSystem_;
    std::vector<float> stars_;
};

#endif
