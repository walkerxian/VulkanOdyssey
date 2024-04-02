#pragma once

#include "soc_device.hpp"
#include "soc_swap_chain.hpp"
#include "soc_window.hpp"

//std
#include <cassert>
#include <memory>
#include <vector>

namespace soc{

//三件事情： 1 DrawFrame（驱动每帧绘制）   2 CommandBuffers(提交到GPU队列执行)  3 SwapChain(呈现到窗口模块) 
class SocRenderer
{

public:
    SocRenderer(SocWindow &window,SocDevice &device);
    ~SocRenderer();

    SocRenderer(const SocRenderer &) = delete;
    SocRenderer &operator=(const SocRenderer &) = delete;


  //1 RenderPass: 告知最终输出的Image(或者ImageView, Attachments)
  VkRenderPass getSwapChainRenderPass() const { return socSwapChain->getRenderPass(); }

  bool isFrameInProgress() const { return isFrameStarted; }

  //2 CommandBuffer:得到渲染当帧的CommandBuffer  
  VkCommandBuffer getCurrentCommandBuffer() const {
    assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
  }

  //得到当前帧的索引，这个是根据交换链中的双缓冲还是三缓冲而决定的
  int getFrameIndex() const {
    assert(isFrameStarted && "Cannot get frame index when frame not in progress");
    return currentFrameIndex;
  }

  
  VkCommandBuffer beginFrame();
  void endFrame();

  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private: 

    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    SocWindow &socWindow;
    SocDevice &socDevice;
    std::unique_ptr<SocSwapChain> socSwapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    
    int currentFrameIndex{0};

    bool isFrameStarted{false};

};







}