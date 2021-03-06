#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include <cstdint>
#include <algorithm>

#include <SmurffCpp/Types.h>
#include <SmurffCpp/Utils/PVec.hpp>
#include <SmurffCpp/Configs/NoiseConfig.h>

namespace smurff
{
   class Data;
   class HDF5Group;

   class DataConfig 
   {
   private:
      NoiseConfig m_noiseConfig;

   protected:
      bool m_hasData;
      bool m_isDense;
      bool m_isScarce;
      bool m_isMatrix;

   private:
      PVec<>      m_pos;
      std::string m_filename;

      Matrix       m_dense_matrix_data;
      SparseMatrix m_sparse_matrix_data;
      DenseTensor  m_dense_tensor_data;
      SparseTensor m_sparse_tensor_data;

   public:
      virtual ~DataConfig();

      DataConfig();

      DataConfig(const Matrix &, const NoiseConfig& noiseConfig = NoiseConfig(), PVec<> pos = PVec<>());
      DataConfig(const SparseMatrix &, bool isScarce = true, const NoiseConfig& noiseConfig = NoiseConfig(), PVec<> pos = PVec<>());
      DataConfig(const DenseTensor &, const NoiseConfig& noiseConfig = NoiseConfig(), PVec<> pos = PVec<>());
      DataConfig(const SparseTensor &, bool isScarce = true, const NoiseConfig& noiseConfig = NoiseConfig(), PVec<> pos = PVec<>());

   public:
      void setData(const Matrix &m);
      void setData(const SparseMatrix &m, bool isScarce = true);
      void setData(const DenseTensor &m);
      void setData(const SparseTensor &m, bool isScarce = true);

      const Matrix       &getDenseMatrixData()  const;
      const SparseMatrix &getSparseMatrixData() const;
      const SparseTensor &getSparseTensorData() const;
      const DenseTensor  &getDenseTensorData() const;

      Matrix       &getDenseMatrixData();
      SparseMatrix &getSparseMatrixData();
      SparseTensor &getSparseTensorData();
      DenseTensor  &getDenseTensorData();

      const NoiseConfig& getNoiseConfig() const;
      void setNoiseConfig(const NoiseConfig& value);
     
      bool hasData() const;
      bool isMatrix() const;
      bool isDense() const;
      bool isScarce() const;

      std::uint64_t getNModes() const;
      std::uint64_t getNNZ() const;

      const std::vector<std::uint64_t> getDims() const;
      std::uint64_t getNRow() const { return getDims().at(0); }
      std::uint64_t getNCol() const { return getDims().at(1); }

      void setFilename(const std::string& f);
      const std::string &getFilename() const;

      void setPos(const PVec<>& p);
      void setPos(const std::vector<int> &p) { setPos(PVec<>(p)); }
      bool hasPos() const;
      const PVec<> &getPos() const;

   public:
      virtual std::ostream& info(std::ostream& os) const;
      virtual std::string info() const;

      void save(HDF5Group& writer, const std::string& section_name) const;
      bool restore(const HDF5Group& reader, const std::string& sec_name);

   public:
      void check() const;
   };
}
