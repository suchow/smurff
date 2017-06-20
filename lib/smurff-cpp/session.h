#ifndef MACAU_H
#define MACAU_H

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <unsupported/Eigen/SparseExtra>
#include <memory>

#include "data.h"
#include "result.h"
#include "model.h"
#include "latentprior.h"


namespace smurff {

class ILatentPrior;

class BaseSession  {
   public:
      //-- data members
      Model                                       model;
      std::unique_ptr<Data>                       data;
      Result                                      pred;
      std::vector< std::unique_ptr<ILatentPrior>> priors;
 
      //-- add priors
      template<class Prior>
      inline Prior& addPrior();

      virtual void step();

      virtual std::ostream &info(std::ostream &, std::string indent);
      void save(std::string prefix, std::string suffix);
      void restore(std::string prefix, std::string suffix);

      std::string name;

   protected:
      bool is_init = false;
};

class Session : public BaseSession {
  public:
      Config      config;
      int         iter = -1;

      // c'tor
      Session() { name = "MacauSession"; }

      //-- set params
      void setFromConfig(const Config &);

      // execution of the sampler
      void init();
      void run();
      void step() override;

      std::ostream &info(std::ostream &, std::string indent) override;

   private:
      void save(int isample);
      void printStatus(double elapsedi);
};

class CmdSession :  public Session {
    public:
        void setFromArgs(int argc, char** argv);
};

class MPISession : public CmdSession {
  public:
    MPISession() { name = "MPISession"; }
      
    void run();
    std::ostream &info(std::ostream &os, std::string indent) override;

    int world_rank;
    int world_size;

};


class PythonSession : public Session {
  public:
    PythonSession() {
        name = "PythonSession"; 
        keepRunning = true;
    }

    void step() override;

  private:
    static void intHandler(int); 
    static bool keepRunning; 

};

template<class Prior>
Prior& BaseSession::addPrior()
{
    auto pos = priors.size();
    Prior *p = new Prior(*this, pos);
    priors.push_back(std::unique_ptr<ILatentPrior>(p));
    return *p;
}

} // end namespace smurff

#endif /* MACAU_H */