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
  struct Vertex {
    glm::vec2 position;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
  };

  SocModel(SocModel &device, const std::vector<Vertex> &vertices);
  ~SocModel();

  SocModel(const SocModel &) = delete;
  SocModel &operator=(const SocModel &) = delete;

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

 private:
  void createVertexBuffers(const std::vector<Vertex> &vertices);

  SocDevice &socDevice;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  uint32_t vertexCount;
};
}  // namespace lve
