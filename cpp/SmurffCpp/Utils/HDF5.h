#pragma once

#include <string>
#include <memory>
#include <vector>
#include <set>
#include <cstdint>

#include <highfive/H5File.hpp>

#include <SmurffCpp/Types.h>

namespace h5 = HighFive;

namespace smurff {
   class HDF5 
   {
   protected:
      mutable h5::Group m_group;

   public:
      HDF5(h5::Group group) : m_group(group) {}

      bool hasDataSet(const std::string &section, const std::string& tag) const;

      template <typename T>
      T get(const std::string &section, const std::string& tag) const
      {
         T value;
         m_group.getGroup(section).getAttribute(tag).read(value);
         return value;
      }

      template <typename T>
      void put(const std::string &section, const std::string& tag, const T &value) const
      {
         m_group.getGroup(section).createAttribute(tag, value);
      }

      std::shared_ptr<Matrix>       getMatrix(const std::string &section, const std::string& tag) const;
      std::shared_ptr<Vector>       getVector(const std::string &section, const std::string& tag) const;
      std::shared_ptr<SparseMatrix> getSparseMatrix(const std::string &section, const std::string& tag) const;

      void putMatrix(const std::string &section, const std::string& tag, const Matrix &) const;
      void putSparseMatrix(const std::string &section, const std::string& tag, const SparseMatrix &) const;
   };
}