#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>

// use gl definitions from glbinding
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>
#include <random>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
    :Application{resource_path}
    ,planet_object{}
    ,star_object{}
    ,orbit_object{}
    ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
    ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
    //,cellShading_Mode{false}
    ,skybox_object{}
    ,fullscreen_quad{} // Assignment 5
    ,framebuffer{} // Assignment 5
{
    initializeGeometry();
    initializeShaderPrograms();
    initializeSolarSystem();
    initializeStars();
    initializeSkyBox();
    initializeOrbits();
    // =====================================================
    // Assignment 5
    initializeFrameBuffer(initial_resolution.x, initial_resolution.y);
    initializeFullscreenQuad();
    // =====================================================
}

ApplicationSolar::~ApplicationSolar() {
    glDeleteBuffers(1, &planet_object.vertex_BO);
    glDeleteBuffers(1, &planet_object.element_BO);
    glDeleteVertexArrays(1, &planet_object.vertex_AO);

    /*
    // =====================================================
    // Assignment 5
    glDeleteBuffers(1, &star_object.vertex_BO);
    glDeleteBuffers(1, &star_object.element_BO);
    glDeleteVertexArrays(1, &star_object.vertex_AO);

    glDeleteBuffers(1, &orbit_object.vertex_BO);
    glDeleteBuffers(1, &orbit_object.element_BO);
    glDeleteVertexArrays(1, &orbit_object.vertex_AO);

    glDeleteBuffers(1, &skybox_object.vertex_BO);
    glDeleteBuffers(1, &skybox_object.element_BO);
    glDeleteVertexArrays(1, &skybox_object.vertex_AO);

    glDeleteBuffers(1, &fullscreen_quad.vertex_BO);
    glDeleteBuffers(1, &fullscreen_quad.element_BO);
    glDeleteVertexArrays(1, &fullscreen_quad.vertex_AO);
    // =====================================================
    */

}

void ApplicationSolar::initializeSolarSystem(){
    // root node
    Node root_node = Node();
    std::shared_ptr<Node> root_node_pointer = std::make_shared<Node>(root_node);

    // create scene graph
    solarSystem_ = SceneGraph("Solar System", root_node_pointer);

    // sun
    makeSun("sun", root_node_pointer, 0.5f, 0.0f, 0.0f, glm::fvec3{1.0f, 1.0f, 0.2f}, 1.0f, glm::fvec3{255, 255, 150},"sun.png", 1);

    // planets
    makePlanet("mercury", root_node_pointer, 0.09f, 0.5f, 1.0f, glm::fvec3{0.59f, 0.59f, 0.62f},"mercury.png", 2);
    makePlanet("venus", root_node_pointer, 0.2f, 0.4f, 1.5f, glm::fvec3{1.0f, 1.0f, 0.75f},"venus.png", 3);
    makePlanet("earth", root_node_pointer, 0.2f, 0.3f, 2.5f, glm::fvec3{0.0f, 0.52f, 0.85f},"earth.png", 4);
    makePlanet("mars", root_node_pointer, 0.1f, 0.2f, 3.5f, glm::fvec3{0.63f, 0.24f, 0.18f},"mars.png", 5);
    makePlanet("jupiter", root_node_pointer, 0.4f, 0.09f, 5.0f, glm::fvec3{1.0f, 0.55f, 0.24f},"jupiter.png", 6);
    makePlanet("saturn", root_node_pointer, 0.4f, 0.10f, 7.0f, glm::fvec3{0.9f, 0.75f, 0.54f},"saturn.png", 7);
    makePlanet("uranus", root_node_pointer, 0.3f, 0.05f, 9.0f, glm::fvec3{0.69f, 0.93f, 0.93f},"uranus.png", 8);
    makePlanet("neptune", root_node_pointer, 0.3f, 0.04f, 10.0f, glm::fvec3{0.69f, 0.93f, 0.93f},"neptune.png", 9);
    makePlanet("pluto", root_node_pointer, 0.04f, 0.06f, 10.5f, glm::fvec3{65, 105, 225},"pluto.png", 10);

    // moon
    std::shared_ptr<Node> earthHolderPtr = root_node_pointer->getChild("earth holder");
    makePlanet("moon", earthHolderPtr, 1.0f, 1.3f, 0.6f, glm::fvec3{0.83f, 0.83f, 0.83f},"moon.png", 11);

    // camera
    CameraNode camera = CameraNode("camera", root_node_pointer, glm::fmat4(1));
    std::shared_ptr<CameraNode> camera_pointer = std::make_shared<CameraNode>(camera);
    root_node_pointer->addChild(camera_pointer);

    std::cout << solarSystem_.printGraph() << std::endl;
}

void ApplicationSolar::makeSun(std::string const& name, std::shared_ptr<Node> const& parent, float size, float speed, float distance, glm::fvec3 color, float light_intensity, glm::fvec3 light_color, std::string texture, int index){
    // set up local transform matrix
    glm::fmat4 localTransform = parent->getWorldTransform();
    localTransform = glm::translate(localTransform, glm::fvec3{0.0f, 0.0f, distance});
    localTransform = glm::rotate(localTransform, float(glfwGetTime()) * speed, glm::fvec3{0.0f, 1.0f, 0.0f});
    localTransform = glm::scale(localTransform, glm::fvec3{size, size, size});

    // create holder node
    PointLightNode sun_light = PointLightNode(name + " light", parent, localTransform, light_intensity, light_color);
    std::shared_ptr<PointLightNode> sun_light_pointer = std::make_shared<PointLightNode>(sun_light);
    parent->addChild(sun_light_pointer);

    // create geometry node
    GeometryNode sun_geometry = GeometryNode(name + " geometry", sun_light_pointer, glm::fmat4(1), size, speed, distance, color, texture, index);
    std::shared_ptr<GeometryNode> sun_geometry_pointer = std::make_shared<GeometryNode>(sun_geometry);
    sun_light_pointer->addChild(sun_geometry_pointer);

    solarSystem_.addPlanet(sun_geometry_pointer);
    solarSystem_.addLightNode(sun_light_pointer);

    makeTexture(sun_geometry_pointer);
}

void ApplicationSolar::makePlanet(std::string const& name, std::shared_ptr<Node> const& parent, float size, float speed, float distance, glm::fvec3 color, std::string texture, int index){
    // set up local transform matrix
    glm::fmat4 localTransform = parent->getWorldTransform();
    localTransform = glm::translate(localTransform, glm::fvec3{0.0f, 0.0f, distance});
    localTransform = glm::rotate(localTransform, float(glfwGetTime()) * speed, glm::fvec3{0.0f, 1.0f, 0.0f});
    localTransform = glm::scale(localTransform, glm::fvec3{size, size, size});

    // create holder node
    Node planet_holder = Node(name + " holder", parent, localTransform);
    std::shared_ptr<Node> planet_holder_pointer = std::make_shared<Node>(planet_holder);
    parent->addChild(planet_holder_pointer);

    // create geometry node
    GeometryNode planet = GeometryNode(name, planet_holder_pointer, glm::fmat4(1), size, speed, distance, color, texture, index);
    std::shared_ptr<GeometryNode> planet_pointer = std::make_shared<GeometryNode>(planet);
    planet_holder_pointer->addChild(planet_pointer);

    solarSystem_.addPlanet(planet_pointer);
    makeTexture(planet_pointer);
}

void ApplicationSolar::makeTexture(std::shared_ptr<GeometryNode> const& object){
    // load texture
    pixel_data planetTexture = texture_loader::file(m_resource_path + "textures/" + object->getTexture());
    // select active texture unit
    glActiveTexture(GL_TEXTURE0 + object->getIndex());

    // create texture object
    texture_object tex;
    tex.target = GL_TEXTURE_2D;
    // generate texture names
    glGenTextures(1, &tex.handle);
    glBindTexture(tex.target, tex.handle);
    object->setTextureObject(tex);

    // define texture sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // define texture data and format
    glTexImage2D(GL_TEXTURE_2D, 0, planetTexture.channels, planetTexture.width, planetTexture.height, 0, planetTexture.channels, planetTexture.channel_type, planetTexture.ptr());
}

void ApplicationSolar::initializeSkyBox() {
    std::cout << "Skybox Blue Loading" << std::endl;
    // load the textures for the skybox
    // try with sequence they are looped through
    //  RIGHT
    pixel_data sideright = texture_loader::file(m_resource_path + "textures/skybox/right.png");
    skybox_contain_pixdata_.push_back(sideright);
    // LEFT
    pixel_data sideleft = texture_loader::file(m_resource_path + "textures/skybox/left.png");
    skybox_contain_pixdata_.push_back(sideleft);
    // BOTTOM
    pixel_data up = texture_loader::file(m_resource_path + "textures/skybox/bottom.png");
    skybox_contain_pixdata_.push_back(up);
    // TOP
    pixel_data down = texture_loader::file(m_resource_path + "textures/skybox/top.png");
    skybox_contain_pixdata_.push_back(down);
    // FRONT
    pixel_data back = texture_loader::file(m_resource_path + "textures/skybox/front.png");
    skybox_contain_pixdata_.push_back(back);
    // BACK
    pixel_data front = texture_loader::file(m_resource_path + "textures/skybox/back.png");
    skybox_contain_pixdata_.push_back(front);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &skybox_texture_obj_.handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_obj_.handle);
    // The WARP_S/T/R set the warping methods for the textures s/t/r coordinates
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    for (unsigned int i = 0; i < skybox_contain_pixdata_.size(); ++i) {
        // starting off with GL_TEXTURE_CUBE_MAP_POSITIVE_X and incrementing it with +i so it will loop through the
        // texture targets
        // POSITIVE_X being right, NEGATIVE_X being left etc. etc.
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, skybox_contain_pixdata_[i].channels,
                     (GLsizei) skybox_contain_pixdata_[i].width, (GLsizei) skybox_contain_pixdata_[i].height,
                     0, skybox_contain_pixdata_[i].channels, skybox_contain_pixdata_[i].channel_type,
                     skybox_contain_pixdata_[i].ptr());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
}

void ApplicationSolar::initializeStars(){
    // for loop to create 15000 stars
    for (int i = 0; i < 15000; ++i) {
        // random XYZ-value for position of the star
        GLfloat x = (rand() % 150) - 75.0f;
        GLfloat y = (rand() % 150) - 75.0f;
        GLfloat z = (rand() % 150) - 75.0f;
        stars_.emplace_back(y);
        stars_.emplace_back(x);
        stars_.emplace_back(z);

        // random RGB-value for color of the star
        GLfloat red = (float) rand() / RAND_MAX;
        GLfloat green = (float) rand() / RAND_MAX;
        GLfloat blue = (float) rand() / RAND_MAX;

        stars_.emplace_back(red);
        stars_.emplace_back(green);
        stars_.emplace_back(blue);
    }
    // generation of the vertex array object
    glGenVertexArrays(1, &star_object.vertex_AO); // (number of VAO, array which VAOs are stored)
    // bind the array for attaching buffers
    glBindVertexArray(star_object.vertex_AO); // (name of VA to bind)
    // generate generic buffer
    glGenBuffers(1, &star_object.vertex_BO); // (number of VBO, array)
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO); // (target(purpose: Vertex attributes), buffer)
    // creates and initializes a buffer object's data store
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float) * stars_.size()), stars_.data(), GL_STATIC_DRAW);
    //Specify the attributes
    // atrribute 0 on GPU
    glEnableVertexAttribArray(0);
    // (index of attribute, number of components, type, normalized, stride = components*num_attributes, pointer to first component of the first attribute)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    // attribute 1 on GPU
    glEnableVertexAttribArray(1);
    // (....,start of the 2nd attribute is at index 3, type of this is void pointer)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *) (sizeof(float) * 3));

    // define draw mode to use when render
    star_object.draw_mode = GL_POINTS;
    // define number of elements: = size of the array devided by the number of componentsof attributes
    star_object.num_elements = GLsizei(stars_.size() / 6);
}

void ApplicationSolar::initializeOrbits()
{
    std::vector<float> orbits;
    int points_in_circle = 360;

    for (int i = 0; i < points_in_circle; ++i) {
        GLfloat x = cos(i * 3.14159265358979323846 / 180); //calculating points on a circle in right order
        GLfloat y = 0;
        GLfloat z = sin(i * 3.14159265358979323846 / 180);
        orbits.push_back(x);
        orbits.push_back(y);
        orbits.push_back(z);
    }

    //Same as in draw stars
    glGenVertexArrays(1, &orbit_object.vertex_AO);
    glBindVertexArray(orbit_object.vertex_AO);

    glGenBuffers(1, &orbit_object.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float) * orbits.size()), orbits.data(), GL_STATIC_DRAW);

    //Attributes - index, size(3-dimensional), dtype, normalize data, byte-distance, offsets in bytes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(sizeof(float) * 3), 0);

    orbit_object.draw_mode = GL_LINE_LOOP;
    orbit_object.num_elements = GLsizei(points_in_circle);

}

// =====================================================
// Assignment 5
void ApplicationSolar::initializeFrameBuffer(int width, int height) {
    // create framebuffer object
    glGenFramebuffers(1, &framebuffer.handle);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.handle);

    // create texture object
    // create texture attachment as color_attachment
    texture_object texture;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture.handle);
    glBindTexture(GL_TEXTURE_2D, texture.handle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.handle, 0);

    framebuffer.color_buffer = texture;
    framebuffer.color_handle = texture.handle;

    //create renderbuffer attachment (depth attachment)
    unsigned int renderbuffer_object;
    glGenRenderbuffers(1, &renderbuffer_object);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_object);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_object);

    framebuffer.depth_handle = renderbuffer_object;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    /*// generate texture names
    glGenTextures(1, &color_buffer.handle);
    glBindTexture(color_buffer.target, color_buffer.handle);

    // define texture sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // define texture data and format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    // set color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer.handle, 0);
    framebuffer.color_buffer = color_buffer;

    // create depth attachment as renderbuffer
    glGenRenderbuffers(1, &framebuffer.depth_handle);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depth_handle);

    // ERROR? - GL_DEBUG_SEVERITY_LOW - GL_DEBUG_TYPE_OTHER: Framebuffer detailed info: The driver allocated storage for renderbuffer 1.
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GLsizei(width), GLsizei(height));
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depth_handle);

    // define which buffers to write
    GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, draw_buffers);*/

    // check whether the framebuffer can be written
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FRAMGEBUFFER DED!" << std::endl;
    }
    else {
        std::cout << "FRAMEBUFFER ON!" << std::endl;
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}

void ApplicationSolar::initializeFullscreenQuad() {
    // triangles
    std::vector<GLfloat> quad = {
        //v1
        -1.0f, 1.0f, 0.0f, 1.0f,
        //v2
        -1.0f, -1.0f, 0.0f, 0.0f,
        //v3
        1.0f, -1.0f, 1.0f, 0.0f,
        //v4
        -1.0f, 1.0f, 0.0f, 1.0f,
        //v5
        1.0f, -1.0f, 1.0f, 0.0f,
        //v6
        1.0f, 1.0f, 1.0f, 1.0f
    };

    //create a new VertexArray
    glGenVertexArrays(1, &fullscreen_quad.vertex_AO);
    glBindVertexArray(fullscreen_quad.vertex_AO);

    //generate a new Buffer and bind it to the new VertexArray
    glGenBuffers(1, &fullscreen_quad.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, fullscreen_quad.vertex_BO);

    //specify the size of the data
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * quad.size(), quad.data(), GL_STATIC_DRAW);

    // first attribute Array for positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, GLsizei(4 * sizeof(float)), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, GLsizei(4 * sizeof(float)), (void*)(2 * sizeof(float)));

    //specify the draw mode and the number of elements
    fullscreen_quad.draw_mode = GL_TRIANGLES;
    fullscreen_quad.num_elements = GLsizei(quad.size() / 4);

    /*// generate vertex array object
    glGenVertexArrays(1, &fullscreen_quad.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(fullscreen_quad.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &fullscreen_quad.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, fullscreen_quad.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * triangles.size(), triangles.data(), GL_STATIC_DRAW);

    // first attribute (position)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(sizeof(float) * 5), 0);

    // second attribute (color)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, GLsizei(sizeof(float) * 5), (void*)(sizeof(float) * 3));

    // store type of primitive to draw
    fullscreen_quad.draw_mode = GL_TRIANGLE_STRIP;
    // transfer number of indices to model object 
    fullscreen_quad.num_elements = GLsizei(triangles.size() / 5);*/
}
// =====================================================

void ApplicationSolar::render() const {
    // =====================================================
    // Assignment 5
    // Bind it and render the scene to it and not to the Default one.
    std::cout << "==============NOT DEFAULT==============" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.handle);    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // =====================================================
    
    // render Skybox
    renderSkybox();
    // render stars
    renderStars();
    // render lightnodes
    renderLightNodes();
    // render planets
    auto planets = solarSystem_.getPlanets();
    for (auto planet : planets){
        renderPlanet(planet);
    }
    // render Orbits
    renderOrbits();

    // =====================================================
    // Assignment 5
    // Render changed to default to display screen
    std::cout << "==============SWITCH TO DEFAULT TO RENDER TO SCREEN==============" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderFullscreenquad();
    // =====================================================
}

// =====================================================
// Assignment 5
void ApplicationSolar::renderFullscreenquad()const {
    // full-screen quad
    glUseProgram(m_shaders.at("screen_quad").handle);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffer.color_handle);

    // add sampler
    int samplerLocation = glGetUniformLocation(m_shaders.at("screen_quad").handle, "screen_Texture");
    glUniform1i(samplerLocation, 0);//framebuffer_obj.texture_obj.handle);

    //render quad
    glBindVertexArray(fullscreen_quad.vertex_AO);
    glDrawArrays(fullscreen_quad.draw_mode, 0, fullscreen_quad.num_elements);

    /*
    //std::cout << "================1111111================" << std::endl;
    //// extract texture from framebuffer
    //glActiveTexture(GL_TEXTURE0 + 12);
    //std::cout << "================2222222================" << std::endl;
    //glBindTexture(framebuffer.color_buffer.target, framebuffer.color_buffer.handle);

    //std::cout << "================3333333================" << std::endl;
    //// upload texture to shader
    //auto temp_texture = glGetUniformLocation(m_shaders.at("screen_quad").handle, "fullScreenQuad_texture");
    //std::cout << "================4444444================" << std::endl;
    //glUniform1i(temp_texture, framebuffer.color_buffer.handle);

    //std::cout << "================5555555================" << std::endl;
    //// render quad
    //glBindVertexArray(fullscreen_quad.vertex_AO);
    //std::cout << "================6666666================" << std::endl;
    //glDrawArrays(fullscreen_quad.draw_mode, 0, fullscreen_quad.num_elements);
    //std::cout << "================7777777================" << std::endl;
    */
}
// =====================================================

void ApplicationSolar::renderLightNodes() const {

    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);

    // upload light uniforms
    auto lightNodes = solarSystem_.getLightNodes();
    for(auto lightNode : lightNodes){
        // upload light intensity
        auto temp_intensity = glGetUniformLocation(m_shaders.at("planet").handle, "light_intensity");
        glUniform1f(temp_intensity, lightNode->getIntensity());

        // upload light color
        auto temp_color = glGetUniformLocation(m_shaders.at("planet").handle, "light_color");
        glUniform3f(temp_color, lightNode->getColor()[0], lightNode->getColor()[1], lightNode->getColor()[2]);

        // calculate position
        glm::fvec4 light_position = lightNode->getWorldTransform() * glm::fvec4{0, 0, 0, 1};

        // upload position
        auto temp_position = glGetUniformLocation(m_shaders.at("planet").handle, "light_position");
        glUniform3f(temp_position, light_position[0] / light_position[3], light_position[1] / light_position[3], light_position[2] / light_position[3]);
    }
}

void ApplicationSolar::renderStars()const{
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("star").handle);

    glm::fmat4 matrix = glm::fmat4();
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(matrix));

    // bind the VAO to draw
    glBindVertexArray(star_object.vertex_AO);

    // draw stars
    glDrawArrays(star_object.draw_mode, GLint(0), star_object.num_elements);
}

void ApplicationSolar::renderPlanet(std::shared_ptr<GeometryNode> planet)const{
    glUseProgram(m_shaders.at("planet").handle);

    glm::fmat4 planetWorldTransform = planet->getParent()->getWorldTransform();
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(planetWorldTransform));

    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 planet_normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * planetWorldTransform);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(planet_normal_matrix));

    // set color
    auto temp_color = glGetUniformLocation(m_shaders.at("planet").handle, "planet_color");
    glm::fvec3 color = planet->getColor();
    glUniform3f(temp_color, color[0], color[1], color[2]);

    // set ambient intensity
    auto temp_ambient =  glGetUniformLocation(m_shaders.at("planet").handle, "ambient_intensity");

    if (planet->getParent()->getIsLight() == true) {
        glUniform3f(temp_ambient, 1.0f, 1.0f, 1.0f);
    } else {
        glUniform3f(temp_ambient, 0.3f, 0.3f, 0.3f);
    }

    // access texture
    glActiveTexture(GL_TEXTURE0 + planet->getIndex());
    texture_object textureObject = planet->getTextureObject();
    glBindTexture(textureObject.target, textureObject.handle);

    // upload texture to shader
    auto temp_texture = glGetUniformLocation(m_shaders.at("planet").handle, "planet_texture");
    glUseProgram(m_shaders.at("planet").handle);
    glUniform1i(temp_texture, textureObject.handle);

    //    auto cell_sharing = glGetUniformLocation(m_shaders.at("planet").handle, "CellShadingMode");
    //    glUniform1f(cell_sharing , cellShading_Mode);

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

    // calculate next step
    planetWorldTransform = glm::rotate(planet->getParent()->getParent()->getWorldTransform(), float(glfwGetTime()) * moving_time * planet->getSpeed(), glm::fvec3{0.0f, 1.0f, 0.0f});
    planet->getParent()->setWorldTransform(planetWorldTransform);
}

void ApplicationSolar::renderOrbits() const
{
    //go through planets and get distance and set as radius -> scale of circle. not yet correct
    for (auto const& planet : solarSystem_.getPlanets()) {
        glm::fmat4 orbit_matrix = glm::fmat4(1.0f);
        float radius = planet->getDistanceToOrigin().x;
        orbit_matrix = glm::scale(orbit_matrix, glm::fvec3{radius, radius, radius});

        glUseProgram(m_shaders.at("orbits").handle);
        glUniformMatrix4fv(m_shaders.at("orbits").u_locs.at("OrbitMatrix"), 1, GL_FALSE, glm::value_ptr(orbit_matrix));
        glBindVertexArray(orbit_object.vertex_AO);
        glDrawArrays(orbit_object.draw_mode, GLint(0), orbit_object.num_elements);
    }
}

void ApplicationSolar::renderSkybox() const {
    glDepthMask(GL_FALSE);
    glUseProgram(m_shaders.at("skybox").handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_obj_.handle);
    glBindVertexArray(skybox_object.vertex_AO);
    glDrawElements(skybox_object.draw_mode, skybox_object.num_elements, model::INDEX.type, NULL);
    glDepthMask(GL_TRUE);
}

void ApplicationSolar::uploadView(std::string shader_name) {
    // =====================================================
    // Assignment 5
    if (shader_name == "screen_quad") {
        glUseProgram(m_shaders.at("screen_quad").handle);

        glUniform1i(m_shaders.at("screen_quad").u_locs.at("horizontal_Mirroring"), horizontal_mirroring);
        glUniform1i(m_shaders.at("screen_quad").u_locs.at("vertical_Mirroring"), vertical_mirroring);
        glUniform1i(m_shaders.at("screen_quad").u_locs.at("greyscale_mode"), greyscale_mode);
        glUniform1i(m_shaders.at("screen_quad").u_locs.at("blur_mode"), blur_mode);
        glUniform2f(m_shaders.at("screen_quad").u_locs.at("texture_Size"), img_width, img_height);
    }
    // =====================================================
    else {
        // vertices are transformed in camera space, so camera transform must be inverted
        glm::fmat4 view_matrix = glm::inverse(m_view_transform);
        // upload matrix to gpu
        glUniformMatrix4fv(m_shaders.at(shader_name).u_locs.at("ViewMatrix"),
            1, GL_FALSE, glm::value_ptr(view_matrix));
    }
}

void ApplicationSolar::uploadProjection(std::string shader_name) {
    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at(shader_name).u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("planet").handle);
    // upload uniform values to new locations
    uploadView("planet");
    uploadProjection("planet");

    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("star").handle);
    // upload uniform values to new locations
    uploadView("star");
    uploadProjection("star");

    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("orbits").handle);
    // upload uniform values to new locations
    uploadView("orbits");
    uploadProjection("orbits");

    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("skybox").handle);
    // upload uniform values to new locations
    uploadView("skybox");
    uploadProjection("skybox");

    // =====================================================
    // Assignment 5
    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("screen_quad").handle);
    // upload uniform values to new locations
    uploadView("screen_quad");
    // =====================================================
}

///////////////////////////// intialisation functions /////////////////////////
// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
    // store shader program objects in container
    m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/simple.vert"},
                                                {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
    // request uniform locations for shader program
    m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
    m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
    m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
    m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;

    // store shader program objects in container
    m_shaders.emplace("star", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/vao.vert"},
                                              {GL_FRAGMENT_SHADER, m_resource_path + "shaders/vao.frag"}}});
    m_shaders.at("star").u_locs["ModelMatrix"] = -1;
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;

    // store shader orbits
    m_shaders.emplace("orbits",
                      shader_program{
                                     {{GL_VERTEX_SHADER, m_resource_path + "shaders/orbits.vert"},
                                      {GL_FRAGMENT_SHADER, m_resource_path + "shaders/orbits.frag"}}});
    m_shaders.at("orbits").u_locs["OrbitMatrix"] = -1;
    m_shaders.at("orbits").u_locs["ViewMatrix"] = -1;
    m_shaders.at("orbits").u_locs["ProjectionMatrix"] = -1;

    // now initialize shaders for skybox
    m_shaders.emplace("skybox", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/skybox.vert"},
                                                {GL_FRAGMENT_SHADER, m_resource_path + "shaders/skybox.frag"}}});

    // request uniform location for shader program
    m_shaders.at("skybox").u_locs["ProjectionMatrix"] = -1;
    m_shaders.at("skybox").u_locs["ViewMatrix"] = -1;

    // =====================================================
    // Assignment 5
    // store quad shader for working with extra framebuffer
    m_shaders.emplace("screen_quad", shader_program{ {{GL_VERTEX_SHADER, m_resource_path + "shaders/screen_quad.vert"},
                                             {GL_FRAGMENT_SHADER, m_resource_path + "shaders/screen_quad.frag"}} });

    m_shaders.at("screen_quad").u_locs["screen_Texture"] = -1;
    m_shaders.at("screen_quad").u_locs["horizontal_Mirroring"] = 0;
    m_shaders.at("screen_quad").u_locs["vertical_Mirroring"] = 0;
    m_shaders.at("screen_quad").u_locs["greyscale_mode"] = 0;
    m_shaders.at("screen_quad").u_locs["blur_mode"] = 0;
    m_shaders.at("screen_quad").u_locs["texture_Size"] = -1;
    // =====================================================
}

// load models
void ApplicationSolar::initializeGeometry() {
    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);

    // generate vertex array object
    glGenVertexArrays(1, &planet_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(planet_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &planet_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);
    // activate third attribute on gpu
    glEnableVertexAttribArray(2);
    // second attribute is 2 floats with no offset & stride
    glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);

    // generate generic buffer
    glGenBuffers(1, &planet_object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

    // store type of primitive to draw
    planet_object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object
    planet_object.num_elements = GLsizei(planet_model.indices.size());

    ////////////////

    // add skybox model
    model skybox_model = model_loader::obj(m_resource_path + "models/skybox.obj");
    // starting with VAO
    glGenVertexArrays(1, &skybox_object.vertex_AO);
    // bind that
    glBindVertexArray(skybox_object.vertex_AO);
    // generic buffer
    glGenBuffers(1, &skybox_object.vertex_BO);
    // again : binding as vertex array buffer with all attributes
    glBindBuffer(GL_ARRAY_BUFFER, skybox_object.vertex_BO);
    // configuration
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * skybox_model.data.size(), skybox_model.data.data(), GL_STATIC_DRAW);
    // activation of first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, skybox_model.vertex_bytes,
                          skybox_model.offsets[model::POSITION]);
    // generate generic buffer
    glGenBuffers(1, &skybox_object.element_BO);
    // bind as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * skybox_model.indices.size(), skybox_model.indices.data(),
                 GL_STATIC_DRAW);
    // store type of primitive to draw
    skybox_object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object
    skybox_object.num_elements = GLsizei(skybox_model.indices.size());
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
    if (key == GLFW_KEY_W  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    }
    else if (key == GLFW_KEY_S  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    }
    else if (key == GLFW_KEY_A  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.1f, 0.0f, 0.0f});
    }
    else if (key == GLFW_KEY_D  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.1f, 0.0f, 0.0f});
    }
    else if (key == GLFW_KEY_SPACE  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.1f, 0.0f});
    }
    else if (key == GLFW_KEY_LEFT_SHIFT  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -0.1f, 0.0f});
    }
    /*
    //    else if (key == GLFW_KEY_1 && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    //        if(cellShading_Mode == true) {
    //            cellShading_Mode = false;
    //        }
    //    }
    //    else if (key == GLFW_KEY_2 && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    //        if(cellShading_Mode == false) {
    //            cellShading_Mode = true;
    //        }
    //    }
    */

    // =====================================================
    // Assignment 5
    // to stop the movement and stay in position
    else if (key == GLFW_KEY_3 && (action == GLFW_PRESS)) {
        moving_time = !moving_time;
    }
    // post-processing
    else if (key == GLFW_KEY_7 && (action == GLFW_PRESS)) {
        greyscale_mode = !greyscale_mode;
    }
    else if (key == GLFW_KEY_8 && (action == GLFW_PRESS)) {
        horizontal_mirroring = !horizontal_mirroring;
    }
    else if (key == GLFW_KEY_9 && (action == GLFW_PRESS)) {
        vertical_mirroring = !vertical_mirroring;
    }
    else if (key == GLFW_KEY_0 && (action == GLFW_PRESS)) {
        blur_mode = !blur_mode;
    }
    // =====================================================

    glUseProgram(m_shaders.at("planet").handle);
    uploadView("planet");
    uploadProjection("planet");
    glUseProgram(m_shaders.at("star").handle);
    uploadView("star");
    uploadProjection("star");
    glUseProgram(m_shaders.at("orbits").handle);
    uploadView("orbits");
    uploadProjection("orbits");
    glUseProgram(m_shaders.at("skybox").handle);
    uploadView("skybox");
    uploadProjection("skybox");
    // =====================================================
    // Assignment 5
    glUseProgram(m_shaders.at("screen_quad").handle);
    uploadView("screen_quad");
    // =====================================================
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
    // mouse handling

    float horizontal_rotate = float(pos_x / 20); //divide to slow down
    float vertical_rotate = float(pos_y / 20);

    //glm rotate transforms a matrix 4x4, created from axis of 3 scalars and angle in degree (glm::radians). Scalars define the axis for rotation
    m_view_transform = glm::rotate(m_view_transform,
                                   glm::radians(vertical_rotate),
                                   glm::vec3{1.0f, 0.0f, 0.0f});
    m_view_transform = glm::rotate(m_view_transform,
                                   glm::radians(horizontal_rotate),
                                   glm::vec3{0.0f, 1.0f, 0.0f});

    glUseProgram(m_shaders.at("planet").handle);
    uploadView("planet");
    uploadProjection("planet");
    glUseProgram(m_shaders.at("star").handle);
    uploadView("star");
    uploadProjection("star");
    glUseProgram(m_shaders.at("orbits").handle);
    uploadView("orbits");
    uploadProjection("orbits");
    glUseProgram(m_shaders.at("skybox").handle);
    uploadView("skybox");
    uploadProjection("skybox");
    // =====================================================
    // Assignment 5
    glUseProgram(m_shaders.at("screen_quad").handle);
    uploadView("screen_quad");
    // =====================================================
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
    // recalculate projection matrix for new aspect ration
    m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
    // upload new projection matrices
    glUseProgram(m_shaders.at("planet").handle);
    uploadView("planet");
    uploadProjection("planet");
    glUseProgram(m_shaders.at("star").handle);
    uploadView("star");
    uploadProjection("star");
    glUseProgram(m_shaders.at("orbits").handle);
    uploadView("orbits");
    uploadProjection("orbits");
    glUseProgram(m_shaders.at("skybox").handle);
    uploadView("skybox");
    uploadProjection("skybox");
    // =====================================================
    // Assignment 5
    glUseProgram(m_shaders.at("screen_quad").handle);
    uploadView("screen_quad");
    initializeFrameBuffer(width, height);
    img_width = width;
    img_height = height;
    // =====================================================
}

// exe entry point
int main(int argc, char* argv[]) {
    Application::run<ApplicationSolar>(argc, argv, 3, 2);
}
