#include "RHI/Interface/FrameBuffer.hpp"
#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include <GLFW/glfw3.h>
#include <glog/logging.h>

#include "Common/Common.hpp"
#include "Common/MathCommon.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"
#include "RHI/RHI.hpp"

void
ShowBox(GLFWwindow* glfwWindow, Marbas::RHIFactory* rhiFactory) {
  auto swapChain = rhiFactory->GetSwapChain();
  auto defaultFrameBuffer = swapChain->GetDefaultFrameBuffer();

  // vertices
  Marbas::Vector<float> cubeVertices{
      // positions          // texture Coords
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  //
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f,   //
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,    //
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,    //
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,   //
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  //
                                        //
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   //
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,    //
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,     //
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,     //
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,    //
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   //
                                        //
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,    //
      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   //
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  //
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  //
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   //
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,    //
                                        //
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,     //
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,    //
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,   //
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,   //
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f,    //
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,     //
                                        //
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  //
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f,   //
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,    //
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,    //
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   //
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  //
                                        //
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,   //
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,    //
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,     //
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,     //
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,    //
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f    //
  };

  Marbas::Vector<float> planeVertices{
      // positions          // texture Coords
      5.0f, -0.5f, 5.0f, 2.0f, 0.0f,    //
      -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,   //
      -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,  //
                                        //
      5.0f, -0.5f, 5.0f, 2.0f, 0.0f,    //
      -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,  //
      5.0f, -0.5f, -5.0f, 2.0f, 2.0f    //
  };

  Marbas::Vector<float> quadVertices{
      // positions   // texCoords
      -1.0f, 1.0f, 0.0f, 1.0f,   //
      -1.0f, -1.0f, 0.0f, 0.0f,  //
      1.0f, -1.0f, 1.0f, 0.0f,   //
                                 //
      -1.0f, 1.0f, 0.0f, 1.0f,   //
      1.0f, -1.0f, 1.0f, 0.0f,   //
      1.0f, 1.0f, 1.0f, 1.0f     //
  };

  auto cubeVertexBufferLen = sizeof(float) * cubeVertices.size();
  std::shared_ptr<Marbas::VertexBuffer> cubeVertexBuffer =
      rhiFactory->CreateVertexBuffer(cubeVertices.data(), cubeVertexBufferLen);
  Marbas::Vector<Marbas::ElementLayout> cubeVertexBufferLayout{
      Marbas::ElementLayout{
          .index = 0,
          .mateType = Marbas::ElementType::FLOAT,
          .count = 3,
      },
      Marbas::ElementLayout{
          .index = 1,
          .mateType = Marbas::ElementType::FLOAT,
          .count = 2,
      },
  };
  Marbas::ElementLayout::CalculateLayout(cubeVertexBufferLayout);
  cubeVertexBuffer->SetLayout(cubeVertexBufferLayout);

  auto planeVerticesLen = sizeof(float) * planeVertices.size();
  std::shared_ptr<Marbas::VertexBuffer> planeVertexBuffer =
      rhiFactory->CreateVertexBuffer(planeVertices.data(), planeVerticesLen);
  Marbas::Vector<Marbas::ElementLayout> planeVertexBufferLayout{
      Marbas::ElementLayout{
          .index = 0,
          .mateType = Marbas::ElementType::FLOAT,
          .count = 3,
      },
      Marbas::ElementLayout{
          .index = 1,
          .mateType = Marbas::ElementType::FLOAT,
          .count = 2,
      },
  };
  Marbas::ElementLayout::CalculateLayout(planeVertexBufferLayout);
  planeVertexBuffer->SetLayout(planeVertexBufferLayout);

  auto quadVerticesLen = sizeof(float) * quadVertices.size();
  std::shared_ptr<Marbas::VertexBuffer> quadVertexBuffer =
      rhiFactory->CreateVertexBuffer(quadVertices.data(), quadVerticesLen);
  Marbas::Vector<Marbas::ElementLayout> quadVertexBufferLayout{
      Marbas::ElementLayout{
          .index = 0,
          .mateType = Marbas::ElementType::FLOAT,
          .count = 2,
      },
      Marbas::ElementLayout{
          .index = 1,
          .mateType = Marbas::ElementType::FLOAT,
          .count = 2,
      },
  };
  Marbas::ElementLayout::CalculateLayout(quadVertexBufferLayout);
  quadVertexBuffer->SetLayout(quadVertexBufferLayout);

  std::shared_ptr<Marbas::Texture> cubeTexture =
      rhiFactory->CreateTexture2D("Test/container.jpg", 1, false);
  std::shared_ptr<Marbas::Texture> floorTexture =
      rhiFactory->CreateTexture2D("Test/metal.png", 1, false);

  std::shared_ptr<Marbas::ShaderStage> frameVertexShaderStage =
      rhiFactory->CreateShaderStage(Marbas::ShaderType::VERTEX_SHADER);
  frameVertexShaderStage->ReadFromSource("Test/showBox.vert.glsl");
  std::shared_ptr<Marbas::ShaderStage> frameFragmentShaderStage =
      rhiFactory->CreateShaderStage(Marbas::ShaderType::FRAGMENT_SHADER);
  frameFragmentShaderStage->ReadFromSource("Test/showBox.frag.glsl");
  std::shared_ptr<Marbas::Shader> frameShader = rhiFactory->CreateShader();
  frameShader->AddShaderStage(frameVertexShaderStage);
  frameShader->AddShaderStage(frameFragmentShaderStage);
  frameShader->Link();

  std::shared_ptr<Marbas::ShaderStage> screenVertexShaderStage =
      rhiFactory->CreateShaderStage(Marbas::ShaderType::VERTEX_SHADER);
  screenVertexShaderStage->ReadFromSource("Test/showBoxScreen.vert.glsl");
  std::shared_ptr<Marbas::ShaderStage> screenFragShaderStage =
      rhiFactory->CreateShaderStage(Marbas::ShaderType::FRAGMENT_SHADER);
  screenFragShaderStage->ReadFromSource("Test/showBoxScreen.frag.glsl");
  std::shared_ptr<Marbas::Shader> screenShader = rhiFactory->CreateShader();
  screenShader->AddShaderStage(screenVertexShaderStage);
  screenShader->AddShaderStage(screenFragShaderStage);
  screenShader->Link();

  struct Matrix {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
  };
  Matrix mat;
  mat.model = glm::mat4(1.0);
  mat.view = glm::lookAt(glm::vec3(5, 5, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  mat.projection = glm::perspective(glm::radians(45.0), 800.0 / 600.0, 0.1, 100.);

  std::shared_ptr<Marbas::UniformBuffer> uniformBuffer =
      rhiFactory->CreateUniformBuffer(sizeof(Matrix));
  uniformBuffer->SetData(reinterpret_cast<const void*>(&mat), sizeof(Matrix), 0);

  Marbas::AttachmentDescription colorDescription{
      .format = Marbas::TextureFormat::RGB,
      .type = Marbas::AttachmentType::Color,
      .loadOp = Marbas::AttachmentLoadOp::Clear,
  };
  Marbas::AttachmentDescription depthDescription{
      .format = Marbas::TextureFormat::DEPTH,
      .type = Marbas::AttachmentType::Depth,
      .loadOp = Marbas::AttachmentLoadOp::Clear,
  };

  /**
   * cube render pass
   */

  Marbas::RenderPassCreateInfo cubeRenderPassCreateInfo{
      // clang-format off
      .attachments = Marbas::Vector<Marbas::AttachmentDescription>{
          colorDescription,
          depthDescription,
      },
      // clang-format on
  };

  // create resource
  std::shared_ptr<Marbas::RenderPass> cubeRenderPass =
      rhiFactory->CreateRenderPass(cubeRenderPassCreateInfo);
  std::shared_ptr<Marbas::GraphicsPipeLine> cubePipeline = rhiFactory->CreateGraphicsPipeLine();
  std::shared_ptr<Marbas::Texture> textureColorBuffer = rhiFactory->CreateTexture(Marbas::ImageDesc{
      .textureType = Marbas::TextureType::TEXTURE2D,
      .format = Marbas::TextureFormat::RGB,
      .width = 800,
      .height = 600,
      .mipmapLevel = 1,
  });
  std::shared_ptr<Marbas::ImageView> textureColorBufferView =
      rhiFactory->CreateImageView(Marbas::ImageViewDesc{
          .m_texture = textureColorBuffer,
          .m_format = Marbas::TextureFormat::RGB,
          .m_type = Marbas::TextureType::TEXTURE2D,
          .m_layerBase = 0,
          .m_layerCount = 1,
          .m_levelBase = 0,
          .m_levelCount = 1,
      });

  std::shared_ptr<Marbas::Texture> textureDepthBuffer = rhiFactory->CreateTexture(Marbas::ImageDesc{
      .textureType = Marbas::TextureType::TEXTURE2D,
      .format = Marbas::TextureFormat::DEPTH,
      .width = 800,
      .height = 600,
      .mipmapLevel = 1,
  });
  std::shared_ptr<Marbas::ImageView> textureDepthBufferView =
      rhiFactory->CreateImageView(Marbas::ImageViewDesc{
          .m_texture = textureDepthBuffer,
          .m_format = Marbas::TextureFormat::DEPTH,
          .m_type = Marbas::TextureType::TEXTURE2D,
          .m_layerBase = 0,
          .m_layerCount = 1,
          .m_levelBase = 0,
          .m_levelCount = 1,
      });
  Marbas::FrameBufferInfo frameBufferInfo{
      .width = 800,
      .height = 600,
      .renderPass = cubeRenderPass.get(),
      .attachments = {textureColorBufferView, textureDepthBufferView},
  };
  std::shared_ptr<Marbas::FrameBuffer> frameBuffer = rhiFactory->CreateFrameBuffer(frameBufferInfo);
  Marbas::DescriptorSetLayout cubeDescriptorSetInfo{
      Marbas::DescriptorSetLayoutBinding{
          .isBuffer = true,
          .type = Marbas::BufferDescriptorType::UNIFORM_BUFFER,
          .bindingPoint = 0,
      },
      Marbas::DescriptorSetLayoutBinding{
          .isBuffer = false,
          .bindingPoint = 0,
      },
  };
  std::shared_ptr<Marbas::DescriptorSet> cubeDescriptorSet =
      rhiFactory->CreateDescriptorSet(cubeDescriptorSetInfo);
  cubePipeline->SetVertexBufferLayout(cubeVertexBuffer->Getlayout(),
                                      Marbas::VertexInputRate::VERTEX);
  cubePipeline->SetShader(frameShader);
  // cubePipeline->SetBlendInfo(Marbas::BlendInfo{.logicOpEnable = false});
  cubePipeline->SetViewPort(Marbas::ViewportInfo{
      .x = 0,
      .y = 0,
      .width = 800,
      .height = 600,
  });
  cubePipeline->Create();
  cubeDescriptorSet->BindBuffer(0, uniformBuffer);
  cubeDescriptorSet->BindImage(0, cubeTexture);

  // create command
  auto cubeCommandBuffer = rhiFactory->CreateCommandBuffer();
  cubeCommandBuffer->BeginRecordCmd();
  cubeCommandBuffer->BeginRenderPass(Marbas::BeginRenderPassInfo{
      .renderPass = cubeRenderPass,
      .frameBuffer = frameBuffer,
      .clearColor = {0.1, 0.1, 0.1, 0.1},
  });
  cubeCommandBuffer->BindPipeline(cubePipeline);
  cubeCommandBuffer->BindVertexBuffer(cubeVertexBuffer);
  cubeCommandBuffer->BindDescriptorSet(Marbas::BindDescriptorSetInfo{
      .descriptorSet = cubeDescriptorSet,
      .layouts = cubeDescriptorSetInfo,
      .bufferPiece = {},
  });
  cubeCommandBuffer->DrawArray(32, 0);
  cubeCommandBuffer->EndRenderPass();
  cubeCommandBuffer->EndRecordCmd();

  /**
   * plane render pass
   */

  colorDescription.loadOp = Marbas::AttachmentLoadOp::Ignore;
  depthDescription.loadOp = Marbas::AttachmentLoadOp::Ignore;

  Marbas::RenderPassCreateInfo planeRenderPassCreateInfo{
      // clang-format off
      .attachments = Marbas::Vector<Marbas::AttachmentDescription>{
          colorDescription,
          depthDescription,
      },
      // clang-format on
  };

  Marbas::DescriptorSetLayout planeDescriptorSetInfo{
      Marbas::DescriptorSetLayoutBinding{
          .isBuffer = true,
          .type = Marbas::BufferDescriptorType::UNIFORM_BUFFER,
          .bindingPoint = 0,
      },
      Marbas::DescriptorSetLayoutBinding{
          .isBuffer = false,
          .bindingPoint = 0,
      },
  };

  // create resource
  std::shared_ptr<Marbas::RenderPass> planeRenderPass =
      rhiFactory->CreateRenderPass(planeRenderPassCreateInfo);
  std::shared_ptr<Marbas::GraphicsPipeLine> planePipeline = rhiFactory->CreateGraphicsPipeLine();
  std::shared_ptr<Marbas::DescriptorSet> planeDescriptorSet =
      rhiFactory->CreateDescriptorSet(planeDescriptorSetInfo);
  auto planeCommandBuffer = rhiFactory->CreateCommandBuffer();
  planePipeline->SetVertexBufferLayout(planeVertexBuffer->Getlayout(),
                                       Marbas::VertexInputRate::VERTEX);
  planePipeline->SetShader(frameShader);
  planePipeline->SetViewPort(Marbas::ViewportInfo{
      .x = 0,
      .y = 0,
      .width = 800,
      .height = 600,
  });
  planePipeline->Create();
  planeDescriptorSet->BindBuffer(0, uniformBuffer);
  planeDescriptorSet->BindImage(0, floorTexture);

  // create command
  planeCommandBuffer->BeginRecordCmd();
  planeCommandBuffer->BeginRenderPass(Marbas::BeginRenderPassInfo{
      .renderPass = planeRenderPass,
      .frameBuffer = frameBuffer,
      .clearColor = {0, 0, 0, 0},
  });
  planeCommandBuffer->BindPipeline(planePipeline);
  planeCommandBuffer->BindVertexBuffer(planeVertexBuffer);
  planeCommandBuffer->BindDescriptorSet(Marbas::BindDescriptorSetInfo{
      .descriptorSet = planeDescriptorSet,
      .layouts = planeDescriptorSetInfo,
      .bufferPiece = {},
  });
  planeCommandBuffer->DrawArray(6, 0);
  planeCommandBuffer->EndRenderPass();
  planeCommandBuffer->EndRecordCmd();

  /**
   * screen render pass
   */

  Marbas::AttachmentDescription presentDescription{
      .format = Marbas::TextureFormat::RGBA,
      .type = Marbas::AttachmentType::Present,
  };

  Marbas::RenderPassCreateInfo screenRenderPassCreateInfo{
      // clang-format off
      .attachments = Marbas::Vector<Marbas::AttachmentDescription>{
          presentDescription,
      },
      // clang-format on
  };

  Marbas::DescriptorSetLayout screenDescriptorSetInfo{
      Marbas::DescriptorSetLayoutBinding{
          .isBuffer = false,
          .bindingPoint = 0,
      },
  };

  // create resource
  std::shared_ptr<Marbas::RenderPass> screenRenderPass =
      rhiFactory->CreateRenderPass(screenRenderPassCreateInfo);
  std::shared_ptr<Marbas::GraphicsPipeLine> screenPipeLine = rhiFactory->CreateGraphicsPipeLine();
  std::shared_ptr<Marbas::DescriptorSet> screenDescriptorSet =
      rhiFactory->CreateDescriptorSet(screenDescriptorSetInfo);
  auto screenCommandBuffer = rhiFactory->CreateCommandBuffer();
  screenDescriptorSet->BindImage(0, textureColorBuffer);
  screenPipeLine->SetVertexBufferLayout(quadVertexBuffer->Getlayout(),
                                        Marbas::VertexInputRate::VERTEX);
  // screenPipeLine->SetBlendInfo(Marbas::BlendInfo{.blendEnable = false});
  screenPipeLine->SetViewPort(Marbas::ViewportInfo{
      .x = 0,
      .y = 0,
      .width = 800,
      .height = 600,
  });
  screenPipeLine->SetDepthStencilInfo(Marbas::DepthStencilInfo{
      .depthTestEnable = false,
      .depthBoundsTestEnable = false,
  });
  screenPipeLine->SetShader(screenShader);
  screenPipeLine->Create();

  // create command
  screenCommandBuffer->BeginRecordCmd();
  screenCommandBuffer->BeginRenderPass(Marbas::BeginRenderPassInfo{
      .renderPass = screenRenderPass,
      .frameBuffer = defaultFrameBuffer,
      .clearColor = {0, 0, 0, 0},
  });
  screenCommandBuffer->BindPipeline(screenPipeLine);
  screenCommandBuffer->BindVertexBuffer(quadVertexBuffer);
  screenCommandBuffer->BindDescriptorSet(Marbas::BindDescriptorSetInfo{
      .descriptorSet = screenDescriptorSet,
      .layouts = screenDescriptorSetInfo,
      .bufferPiece = {},
  });
  screenCommandBuffer->DrawArray(6, 0);
  screenCommandBuffer->EndRenderPass();
  screenCommandBuffer->EndRecordCmd();

  while (!glfwWindowShouldClose(glfwWindow)) {
    cubeCommandBuffer->SubmitCommand();
    planeCommandBuffer->SubmitCommand();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);  // disable depth test so screen-space quad isn't discarded due to
                               // depth test.

    screenCommandBuffer->SubmitCommand();

    swapChain->Present();
    glfwPollEvents();
  }
}

int
main(int argc, char* argv[]) {
  FLAGS_alsologtostderr = true;
  google::InitGoogleLogging(argv[0]);

  // init glfw glfwWindow  and rhiFactory
  int width = 800;
  int height = 600;
  glfwInit();
  auto* rhiFactory = Marbas::RHIFactory::GetInstance(Marbas::RendererType::OPENGL);

  auto* glfwWindow = glfwCreateWindow(width, height, "Test Window", nullptr, nullptr);
  rhiFactory->SetGLFWwindow(glfwWindow);
  rhiFactory->Init(Marbas::RHICreateInfo{
      .m_openglRHICreateInfo = Marbas::OpenGLRHICreateInfo{.useSPIRV = false},
  });

  ShowBox(glfwWindow, rhiFactory);

  return 0;
}
