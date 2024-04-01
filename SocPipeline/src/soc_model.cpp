#include "soc_model.hpp"

// std
#include <cassert>
#include <cstring>

namespace soc {

//模型需要喂给顶点缓冲区
SocModel::SocModel(SocDevice &device, const std::vector<Vertex> &vertices) : socDevice{device} {
  createVertexBuffers(vertices);
}

SocModel::~SocModel() {
  
  //既要释放顶点缓冲区，也要释放内存区域
  vkDestroyBuffer(socDevice.device(), vertexBuffer, nullptr);

  vkFreeMemory(socDevice.device(), vertexBufferMemory, nullptr);
}

//创建并填充Vulkan顶点缓冲区，顶点数据最终被复制到GPU内存中，可以被后续的图形管线调用来绘制图元。
void SocModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
  
  vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "Vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
  socDevice.createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,//该内存自动对GPU可见
      vertexBuffer,
      vertexBufferMemory);

  void *data;
  //创建VertexBuffer成功之后，填充该内存
  vkMapMemory(socDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(socDevice.device(), vertexBufferMemory);
}

void SocModel::draw(VkCommandBuffer commandBuffer) {
  vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

void SocModel::bind(VkCommandBuffer commandBuffer) {

  VkBuffer buffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  //绑定到顶点缓冲区
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

}
//如何读取顶点缓冲区的数据：间隔，读取频率
std::vector<VkVertexInputBindingDescription> SocModel::Vertex::getBindingDescriptions() {
  
  std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescriptions;
}

//如何解释顶点缓冲区的数据
std::vector<VkVertexInputAttributeDescription> SocModel::Vertex::getAttributeDescriptions() {

  std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, position);

  //Color 在一个绑定中交错使用位置和颜色
  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Vertex, color);
  return attributeDescriptions;

}

}  // namespace lve