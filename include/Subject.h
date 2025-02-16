/**
 * @file Subject.h
 * @brief Subject class that allows observers to register and receive notifications.
 */

 #pragma once
 #include <vector>
 #include <memory>
 #include "IObserver.h"
 
 class Subject {
 public:
     void AddObserver(std::shared_ptr<IObserver> observer) {
         observers.push_back(observer);
     }
     void Notify(const std::string& event) {
         for (auto& obs : observers) {
             obs->OnNotify(event);
         }
     }
 private:
     std::vector<std::shared_ptr<IObserver>> observers;
 };
 