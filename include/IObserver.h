/**
 * @file IObserver.h
 * @brief Observer interface for receiving notifications.
 */

 #pragma once
 #include <string>
 
 class IObserver {
 public:
     virtual void OnNotify(const std::string& event) = 0;
     virtual ~IObserver() = default;
 };
 