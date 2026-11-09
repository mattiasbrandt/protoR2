// =============================================================================
// include/dome_rotation_task.h
//
// DomeRotationTask  --  turns the dome through the Dome Rotation backend
// (include/dome_rotation_backend.h). Receives commands via
// domeRotationCmdQueue and executes them.
// =============================================================================
#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "robot_state.h"

// Run the Dome Rotation backend's start-up rule before DomeRotationTask's
// first turn; blocks for as long as the rule holds. Call once from setup().
void domeRotationTaskInit();

// The DomeRotationTask function  --  runs on Core 1.
void domeRotationTask(void* pvParameters);
