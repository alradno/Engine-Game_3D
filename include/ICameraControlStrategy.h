/**
 * @file ICameraControlStrategy.h
 * @brief Interface for camera control strategies.
 */

 #pragma once
 #include "Camera.h"
 #include "SceneNode.h"
 
 class ICameraControlStrategy {
 public:
     virtual void UpdateCamera(Camera* camera, const SceneNode* player, float dt) = 0;
     virtual ~ICameraControlStrategy() = default;
 };
 