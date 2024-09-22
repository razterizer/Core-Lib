//
//  unit_tests.cpp
//  Core
//
//  Created by Rasmus Anthin on 2024-09-21.
//

#include "DateTime_tests.h"
#include "Histogram_tests.h"
#include <iostream>


int main(int argc, char** argv)
{
  std::cout << "### DateTime Tests ###" << std::endl;
  datetime::unit_tests();

  std::cout << "### Histogram Tests ###" << std::endl;
  hist::unit_tests();
  
  return 0;
}
