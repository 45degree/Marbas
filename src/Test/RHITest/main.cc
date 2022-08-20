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
  auto swapChain = rhiFactory->CreateSwapChain();
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

  auto cubeTexture = rhiFactory->CreateTexutre2D("Test/container.jpg", 1);
  auto floorTexture = rhiFactory->CreateTexutre2D("Test/metal.png", 1);

  std::shared_ptr<Marbas::ShaderStage> frameVertexShaderStage =
      rhiFactory->CreateShaderStage(Marbas::ShaderType::VERTEX_SHADER);
  frameVertexShaderStage->ReadSPIR_V("Test/showBox.vert.glsl.spv", "main");
  std::shared_ptr<Marbas::ShaderStage> frameFragmentShaderStage =
      rhiFactory->CreateShaderStage(Marbas::ShaderType::FRAGMENT_SHADER);
  frameFragmentShaderStage->ReadSPIR_V("Test/showBox.frag.glsl.spv", "main");
  std::shared_ptr<Marbas::Shader> frameShader = rhiFactory->CreateShader();
  frameShader->AddShaderStage(frameVertexShaderStage);
  frameShader->AddShaderStage(frameFragmentShaderStage);
  frameShader->Link();

  std::shared_ptr<Marbas::ShaderStage> screenVertexShaderStage =
      rhiFactory->CreateShaderStage(Marbas::ShaderType::VERTEX_SHADER);
  screenVertexShaderStage->ReadSPIR_V("Test/showBoxScreen.vert.glsl.spv", "main");
  std::shared_ptr<Marbas::ShaderStage> screenFragShaderStage =
      rhiFactory->CreateShaderStage(Marbas::ShaderType::FRAGMENT_SHADER);
  screenFragShaderStage->ReadSPIR_V("Test/showBoxScreen.frag.glsl.spv", "main");
  std::shared_ptr<Marbas::Shader> screenShader = rhiFactory->CreateShader();
  screenShader->AddShaderStage(screenVertexShaderStage);
  screenShader->AddShaderStage(screenFragShaderStage);
  screenShader->Link();

  auto commandFactory = rhiFactory->CreateCommandFactory();

  struct Matrix {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
  };
  Matrix mat;
  mat.model = glm::mat4(1.0);
  mat.view = glm::lookAt(glm::vec3(5, 5, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  mat.projection = glm::perspective(glm::radians(45.0), 800.0 / 600.0, 0.1, 100.);

  auto uniformBuffer = rhiFactory->CreateUniformBuffer(sizeof(Matrix));
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
  std::shared_ptr<Marbas::Texture2D> textureColorBuffer =
      rhiFactory->CreateTexutre2D(800, 600, 1, Marbas::TextureFormat::RGB);
  std::shared_ptr<Marbas::Texture2D> textureDepthBuffer =
      rhiFactory->CreateTexutre2D(800, 600, 1, Marbas::TextureFormat::DEPTH);
  Marbas::FrameBufferInfo frameBufferInfo{
      .width = 800,
      .height = 600,
      .renderPass = cubeRenderPass.get(),
      .attachments = {textureColorBuffer, textureDepthBuffer},
  };
  std::shared_ptr<Marbas::FrameBuffer> frameBuffer = rhiFactory->CreateFrameBuffer(frameBufferInfo);
  auto cubeCommandBuffer = commandFactory->CreateCommandBuffer();
  Marbas::DescriptorSetInfo cubeDescriptorSetInfo{
      Marbas::DescriptorInfo{
          .isBuffer = true,
          .type = Marbas::BufferDescriptorType::UNIFORM_BUFFER,
          .bindingPoint = 0,
      },
      Marbas::DescriptorInfo{
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
  cubeDescriptorSet->BindBuffer(0, uniformBuffer->GetIBufferDescriptor());
  cubeDescriptorSet->BindImage(0, cubeTexture->GetDescriptor());

  // create command
  auto cubeBeginRenderPass = commandFactory->CreateBeginRenderPassCMD();
  cubeBeginRenderPass->SetRenderPass(cubeRenderPass);
  cubeBeginRenderPass->SetFrameBuffer(frameBuffer);
  cubeBeginRenderPass->SetClearColor({0.1, 0.1, 0.1, 1.0});
  auto cubeEndRenderPass = commandFactory->CreateEndRenderPassCMD();
  auto cubeBindVertexBuffer = commandFactory->CreateBindVertexBufferCMD();
  cubeBindVertexBuffer->SetVertexBuffer(cubeVertexBuffer);
  auto cubeBindPipeLine = commandFactory->CreateBindPipelineCMD();
  cubeBindPipeLine->SetPipeLine(cubePipeline);
  auto cubeBindDesciptor = commandFactory->CreateBindDescriptorSetCMD();
  cubeBindDesciptor->SetDescriptor(cubeDescriptorSet);
  auto cubeDrawArray = commandFactory->CreateDrawArrayCMD(cubePipeline);
  cubeDrawArray->SetVertexCount(36);

  // add command to command buffer
  cubeCommandBuffer->BeginRecordCmd();
  cubeCommandBuffer->AddCommand(std::move(cubeBeginRenderPass));
  cubeCommandBuffer->AddCommand(std::move(cubeBindPipeLine));
  cubeCommandBuffer->AddCommand(std::move(cubeBindVertexBuffer));
  cubeCommandBuffer->AddCommand(std::move(cubeBindDesciptor));
  cubeCommandBuffer->AddCommand(std::move(cubeDrawArray));
  cubeCommandBuffer->AddCommand(std::move(cubeEndRenderPass));
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

  Marbas::DescriptorSetInfo planeDescriptorSetInfo{
      Marbas::DescriptorInfo{
          .isBuffer = true,
          .type = Marbas::BufferDescriptorType::UNIFORM_BUFFER,
          .bindingPoint = 0,
      },
      Marbas::DescriptorInfo{
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
  auto planeCommandBuffer = commandFactory->CreateCommandBuffer();
  planePipeline->SetVertexBufferLayout(planeVertexBuffer->Getlayout(),
                                       Marbas::VertexInputRate::VERTEX);
  planePipeline->SetShader(frameShader);
  // planePipeline->SetBlendInfo(Marbas::BlendInfo{.blendEnable = false});
  planePipeline->SetViewPort(Marbas::ViewportInfo{
      .x = 0,
      .y = 0,
      .width = 800,
      .height = 600,
  });
  planePipeline->Create();
  planeDescriptorSet->BindBuffer(0, uniformBuffer->GetIBufferDescriptor());
  planeDescriptorSet->BindImage(0, floorTexture->GetDescriptor());

  // create command
  auto planeBeginRenderPass = commandFactory->CreateBeginRenderPassCMD();
  planeBeginRenderPass->SetRenderPass(planeRenderPass);
  planeBeginRenderPass->SetFrameBuffer(frameBuffer);
  auto planeEndRenderPass = commandFactory->CreateEndRenderPassCMD();
  auto planeBindVertexBuffer = commandFactory->CreateBindVertexBufferCMD();
  planeBindVertexBuffer->SetVertexBuffer(planeVertexBuffer);
  auto planeBindPipeLine = commandFactory->CreateBindPipelineCMD();
  planeBindPipeLine->SetPipeLine(planePipeline);
  auto planeBindDescriptorSet = commandFactory->CreateBindDescriptorSetCMD();
  planeBindDescriptorSet->SetDescriptor(planeDescriptorSet);
  auto planeDrawArray = commandFactory->CreateDrawArrayCMD(planePipeline);
  planeDrawArray->SetVertexCount(6);

  // add command to command buffer
  planeCommandBuffer->BeginRecordCmd();
  planeCommandBuffer->AddCommand(std::move(planeBeginRenderPass));
  planeCommandBuffer->AddCommand(std::move(planeBindPipeLine));
  planeCommandBuffer->AddCommand(std::move(planeBindVertexBuffer));
  planeCommandBuffer->AddCommand(std::move(planeBindDescriptorSet));
  planeCommandBuffer->AddCommand(std::move(planeDrawArray));
  planeCommandBuffer->AddCommand(std::move(planeEndRenderPass));
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

  Marbas::DescriptorSetInfo screenDescriptorSetInfo{
      Marbas::DescriptorInfo{
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
  auto screenCommandBuffer = commandFactory->CreateCommandBuffer();
  screenDescriptorSet->BindImage(0, textureColorBuffer->GetDescriptor());
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
  auto screenBeginRenderPass = commandFactory->CreateBeginRenderPassCMD();
  screenBeginRenderPass->SetRenderPass(screenRenderPass);
  screenBeginRenderPass->SetFrameBuffer(defaultFrameBuffer);
  auto screenEndRenderPass = commandFactory->CreateEndRenderPassCMD();
  auto screenBindVertexBuffer = commandFactory->CreateBindVertexBufferCMD();
  screenBindVertexBuffer->SetVertexBuffer(quadVertexBuffer);
  auto screenBindPipeLine = commandFactory->CreateBindPipelineCMD();
  screenBindPipeLine->SetPipeLine(screenPipeLine);
  auto screenBindDescriptor = commandFactory->CreateBindDescriptorSetCMD();
  screenBindDescriptor->SetDescriptor(screenDescriptorSet);
  auto screenDrawArray = commandFactory->CreateDrawArrayCMD(screenPipeLine);
  screenDrawArray->SetVertexCount(6);

  // add command to command buffer
  screenCommandBuffer->BeginRecordCmd();
  screenCommandBuffer->AddCommand(std::move(screenBeginRenderPass));
  screenCommandBuffer->AddCommand(std::move(screenBindPipeLine));
  screenCommandBuffer->AddCommand(std::move(screenBindVertexBuffer));
  screenCommandBuffer->AddCommand(std::move(screenBindDescriptor));
  screenCommandBuffer->AddCommand(std::move(screenDrawArray));
  screenCommandBuffer->AddCommand(std::move(screenEndRenderPass));
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
      .m_openglRHICreateInfo = Marbas::OpenGLRHICreateInfo{.useSPIRV = true},
  });

  ShowBox(glfwWindow, rhiFactory);

  return 0;
}
