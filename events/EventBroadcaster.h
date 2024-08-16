//
//  EventBroadcaster.h
//  Core
//
//  Created by Rasmus Anthin on 2024-08-17.
//

#pragma once
#include "../StlUtils.h"
#include <vector>

template<typename ListenerT>
class EventBroadcaster
{
  std::vector<ListenerT*> m_listeners;
  
public:
  void add_listener(ListenerT* listener)
  {
    m_listeners.emplace_back(listener);
  }
  
  void remove_listener(ListenerT* listener)
  {
    stlutils::erase(m_listeners, listener);
  }
  
  template<typename Lambda>
  void broadcast(Lambda pred)
  {
    for (auto* listener : m_listeners)
      pred(listener);
  }

};
