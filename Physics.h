//
//  Physics.h
//  Core
//
//  Created by Rasmus Anthin.
//

#pragma once

namespace physics
{
  namespace UnitConversion
  {
    float deg_celcius_to_kelvin(float degC)
    {
      return degC + 273.15f;
    }
    
    float kelvin_to_deg_celcius(float K)
    {
      return K - 273.15f;
    }
  }

  

}
