#pragma once

#include "soc_device.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
// std
#include <vector>

namespace soc {
class SocModel {
 public:

 //自定义的顶点数据
  struct Vertex {
    glm::vec2 position;
    glm::vec3 color;
    //对于顶点缓冲区需要Binding描述信息和Attribute描述信息
    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
  };

  SocModel(SocDevice &device, const std::vector<Vertex> &vertices);
  ~SocModel();

  SocModel(const SocModel &) = delete;
  SocModel& operator=(const SocModel &) = delete;

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

 private:

  void createVertexBuffers(const std::vector<Vertex> &vertices);

  SocDevice& socDevice;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  
  uint32_t vertexCount;
};
}  // namespace lve
