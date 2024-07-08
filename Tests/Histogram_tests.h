//
//  Histogram_tests.h
//  Core Lib
//
//  Created by Rasmus Anthin on 2022-12-01.
//

#pragma once
#include "../Histogram.h"

namespace hist
{

  void unit_tests()
  {
    Histogram<float> hist(100, -4, 10);
    for (int i = 0; i < 100'000; ++i)
      hist += rnd::randn_clamp(0, 3, -4, 10);
    auto sb_hist = hist.to_stringbox(20, 80); //sh.num_cols());
    sb_hist.print();
    int num_outside = hist.sanity_check_bucket_samples();
    if (num_outside > 0)
      std::cerr << num_outside << " samples in the wrong bucket!" << std::endl;
  }

}
