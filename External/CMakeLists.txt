cmake_minimum_required(VERSION 3.10)

project(DirectXTK VERSION 0.0.1 LANGUAGES CXX)

file(GLOB SOURCES DirectXTK/Src/* DirectXTK/Inc/*)

add_library(DirectXTK 
    DirectXTK/Src/AlignedNew.h
    DirectXTK/Src/AlphaTestEffect.cpp
    DirectXTK/Src/BasicEffect.cpp
    DirectXTK/Src/Bezier.h
    DirectXTK/Src/BinaryReader.cpp
    DirectXTK/Src/BinaryReader.h
    DirectXTK/Src/CommonStates.cpp
    DirectXTK/Src/ConstantBuffer.h
    DirectXTK/Src/dds.h
    DirectXTK/Src/DDSTextureLoader.cpp
    DirectXTK/Src/DemandCreate.h
    DirectXTK/Src/DGSLEffect.cpp
    DirectXTK/Src/DGSLEffectFactory.cpp
    DirectXTK/Src/DualTextureEffect.cpp
    DirectXTK/Src/EffectCommon.cpp
    DirectXTK/Src/EffectCommon.h
    DirectXTK/Src/EffectFactory.cpp
    DirectXTK/Src/EnvironmentMapEffect.cpp
    #DirectXTK/Src/GamePad.cpp
    DirectXTK/Src/GeometricPrimitive.cpp
    DirectXTK/Src/Geometry.cpp
    DirectXTK/Src/Geometry.h
    DirectXTK/Src/GraphicsMemory.cpp
    DirectXTK/Src/Keyboard.cpp
    DirectXTK/Src/LoaderHelpers.h
    DirectXTK/Src/Model.cpp
    DirectXTK/Src/ModelLoadCMO.cpp
    DirectXTK/Src/ModelLoadSDKMESH.cpp
    DirectXTK/Src/ModelLoadVBO.cpp
    DirectXTK/Src/Mouse.cpp
    DirectXTK/Src/NormalMapEffect.cpp
    DirectXTK/Src/pch.cpp
    DirectXTK/Src/pch.h
    DirectXTK/Src/PlatformHelpers.h
    DirectXTK/Src/PrimitiveBatch.cpp
    DirectXTK/Src/ScreenGrab.cpp
    DirectXTK/Src/SDKMesh.h
    DirectXTK/Src/SharedResourcePool.h
    DirectXTK/Src/SimpleMath.cpp
    DirectXTK/Src/SkinnedEffect.cpp
    DirectXTK/Src/SpriteBatch.cpp
    DirectXTK/Src/SpriteFont.cpp
    DirectXTK/Src/TeapotData.inc
    DirectXTK/Src/vbo.h
    DirectXTK/Src/VertexTypes.cpp
    DirectXTK/Src/WICTextureLoader.cpp
    DirectXTK/Inc/Audio.h
    DirectXTK/Inc/CommonStates.h
    DirectXTK/Inc/DDSTextureLoader.h
    DirectXTK/Inc/DirectXHelpers.h
    DirectXTK/Inc/Effects.h
    #DirectXTK/Inc/GamePad.h
    DirectXTK/Inc/GeometricPrimitive.h
    DirectXTK/Inc/GraphicsMemory.h
    DirectXTK/Inc/Keyboard.h
    DirectXTK/Inc/Model.h
    DirectXTK/Inc/Mouse.h
    DirectXTK/Inc/PrimitiveBatch.h
    DirectXTK/Inc/ScreenGrab.h
    DirectXTK/Inc/SimpleMath.h
    DirectXTK/Inc/SimpleMath.inl
    DirectXTK/Inc/SpriteBatch.h
    DirectXTK/Inc/SpriteFont.h
    DirectXTK/Inc/VertexTypes.h
    DirectXTK/Inc/WICTextureLoader.h
)
target_include_directories(DirectXTK PUBLIC DirectXTK/Inc/)