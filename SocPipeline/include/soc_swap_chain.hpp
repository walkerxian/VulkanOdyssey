#pragma once

#include "soc_device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>
#include <memory>

namespace soc {

class SocSwapChain {
 public:
  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  SocSwapChain(SocDevice &deviceRef, VkExtent2D windowExtent);
  SocSwapChain(
      SocDevice &deviceRef, VkExtent2D windowExtent, std::shared_ptr<SocSwapChain> previous);

  ~SocSwapChain();

  SocSwapChain(const SocSwapChain &) = delete;
  SocSwapChain& operator=(const SocSwapChain &) = delete;

  VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
  VkRenderPass getRenderPass() { return renderPass; }
  VkImageView getImageView(int index) { return swapChainImageViews[index]; }
  
  size_t imageCount() { return swapChainImages.size(); }
  VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
  VkExtent2D getSwapChainExtent() { return swapChainExtent; }
  uint32_t width() { return swapChainExtent.width; }
  uint32_t height() { return swapChainExtent.height; }

  float extentAspectRatio() {
    return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
  }
  VkFormat findDepthFormat();

  VkResult acquireNextImage(uint32_t *imageIndex);
  VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

bool compareSwapFormats(const SocSwapChain &swapChain) const {
    return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
           swapChain.swapChainImageFormat == swapChainImageFormat;
  }

 private:
  void init();
  void createSwapChain();
  void createImageViews();
  void createDepthResources();
  void createRenderPass();
  void createFramebuffers();
  void createSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkFormat swapChainImageFormat;
  VkFormat swapChainDepthFormat;
  VkExtent2D swapChainExtent;

  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkRenderPass renderPass;

  std::vector<VkImage> depthImages;
  std::vector<VkDeviceMemory> depthImageMemorys;
  std::vector<VkImageView> depthImageViews;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;

  SocDevice &device;
  VkExtent2D windowExtent;

  VkSwapchainKHR swapChain;
  //使用 std::shared_ptr 管理交换链和其图像可以简化资源的重新创建和释放逻辑。
  std::shared_ptr<SocSwapChain> oldSwapChain;//add memory headfile


  //针对GPU队列之间的同步
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  //针对应用程序CPU侧的同步
  std::vector<VkFence> inFlightFences;
  std::vector<VkFence> imagesInFlight;

  size_t currentFrame = 0;
  
};

}  // namespace lve
