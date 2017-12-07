#pragma once

#include <string>
#include <iostream>

#include <SmurffCpp/Noises/INoiseModel.h>

#include <SmurffCpp/DataMatrices/Data.h>

namespace smurff {

   class NoiseFactory;

   // Gaussian noise that adapts to the data
   class AdaptiveGaussianNoise : public INoiseModel
   {
      friend class NoiseFactory;
      
   public:
      double var_total = NAN;
      double alpha = NAN;
      double alpha_max = NAN;
      double sn_max;
      double sn_init;

   protected:
      AdaptiveGaussianNoise(double sinit = 1., double smax = 10.);

   public:
      void init(const Data* data) override;
      void update(const SubModel& model) override;
      double getAlpha(const SubModel& model, const PVec<> &pos, double val) override;

      std::ostream &info(std::ostream &os, std::string indent) override;
      std::string getStatus() override;

      void setSNInit(double a);
      void setSNMax(double a);
   };

}
