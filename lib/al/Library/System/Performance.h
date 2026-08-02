#pragma once

#include <nn/oe.h>

namespace al {
class ApplicationMessageReceiver;
class LiveActor;

enum class CpuPerformance {};
enum class GpuPerformance { _2 = 2 };
enum class MemoryPerformance {};

void isPerformanceNormal(const ApplicationMessageReceiver*);
void isPerformanceBoost(const ApplicationMessageReceiver*);
void getPerformanceConfiguration(nn::oe::PerformanceMode);
void getPerformanceConfiguration(const ApplicationMessageReceiver*);
void getCpuPerformance(nn::oe::PerformanceMode);
void getCpuPerformance(const ApplicationMessageReceiver*);
void getCpuPerformance(const LiveActor*);
void getGpuPerformance(nn::oe::PerformanceMode);
void getGpuPerformance(const ApplicationMessageReceiver*);
void getGpuPerformance(const LiveActor*);
void getMemoryPerformance(nn::oe::PerformanceMode);
void getMemoryPerformance(const ApplicationMessageReceiver*);
void getMemoryPerformance(const LiveActor*);
void setCpuPerformance(CpuPerformance, nn::oe::PerformanceMode);
void setCpuPerformance(CpuPerformance, const ApplicationMessageReceiver*);
void setGpuPerformance(GpuPerformance, nn::oe::PerformanceMode);
void setGpuPerformance(GpuPerformance, const ApplicationMessageReceiver*);
void setMemoryPerformance(MemoryPerformance, nn::oe::PerformanceMode);
void setMemoryPerformance(MemoryPerformance, const ApplicationMessageReceiver*);
}  // namespace al
