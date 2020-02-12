#include <fstream>
#include <cstdio>

#include "catch.hpp"

#include <SmurffCpp/Types.h>

#include <SmurffCpp/Configs/Config.h>
#include <SmurffCpp/Predict/PredictSession.h>
#include <SmurffCpp/Sessions/SessionFactory.h>
#include <SmurffCpp/Utils/MatrixUtils.h>
#include <SmurffCpp/Utils/RootFile.h>
#include <SmurffCpp/result.h>

#ifdef USE_BOOST_RANDOM
#define TAG_MATRIX_TESTS "[matrix][random]"
#define TAG_TWO_DIMENTIONAL_TENSOR_TESTS "[tensor2d][random]"
#define TAG_THREE_DIMENTIONAL_TENSOR_TESTS "[tensor3d][random]"
#define TAG_VS_TESTS "[versus][random]"
#else
#define TAG_MATRIX_TESTS "[matrix][random][!mayfail]"
#define TAG_TWO_DIMENTIONAL_TENSOR_TESTS "[tensor2d][random][!mayfail]"
#define TAG_THREE_DIMENTIONAL_TENSOR_TESTS "[tensor3d][random][!mayfail]"
#define TAG_VS_TESTS "[versus][random][!mayfail]"
#endif

// Code for printing test results that can then be copy-pasted into tests as
// expected results
void printActualResults(int nr, double actualRmseAvg,
                        const std::vector<smurff::ResultItem> &actualResults) {

  static const char *fname = "TestsSmurff_ExpectedResults.h";
  static bool cleanup = true;

  if (cleanup)
  {
    std::remove(fname);
    cleanup = false;
  }

  std::ofstream os(fname, std::ofstream::app);

  os << "{ " << nr << ",\n"
     << "  { " << std::fixed << std::setprecision(16) << actualRmseAvg << ","
     << std::endl
     << "      {\n";

  for (const auto &actualResultItem : actualResults) {
    os << std::setprecision(16);
    os << "         { { " << actualResultItem.coords << " }, "
       << actualResultItem.val << ", " << std::fixed
       << actualResultItem.pred_1sample << ", " << actualResultItem.pred_avg
       << ", " << actualResultItem.var << ", "
       << " }," << std::endl;
  }

  os << "      }\n"
     << "  }\n"
     << "},\n";
}

#define PRINT_ACTUAL_RESULTS(nr)
//#define PRINT_ACTUAL_RESULTS(nr) printActualResults(nr, actualRmseAvg, actualResults);

using namespace smurff;

struct ExpectedResult {
  double rmseAvg;
  std::vector<ResultItem> resultItems;
};
std::map<int, ExpectedResult> expectedResults = {
#include "TestsSmurff_ExpectedResults.h"
};

static NoiseConfig fixed_ncfg(NoiseTypes::fixed);

// dense train data (matrix/tensor 2d/tensor 3d)


Config genConfig(std::shared_ptr<TensorConfig> train,
                 std::shared_ptr<TensorConfig> test,
                 std::vector<PriorTypes> priors) {
  Config config;
  config.setBurnin(50);
  config.setNSamples(50);
  config.setVerbose(false);
  config.setRandomSeed(1234);
  config.setNumThreads(1);
  config.setNumLatent(4);
  config.setTrain(train);
  config.setTest(test);
  config.setPriorTypes(priors);
  return config;
}

std::shared_ptr<MatrixConfig> trainDenseMatrix() {
  std::vector<double> trainMatrixConfigVals = {1, 5, 9,  2, 6, 10,
                                               3, 7, 11, 4, 8, 12};
  std::shared_ptr<MatrixConfig> trainMatrixConfig =
      std::make_shared<MatrixConfig>(3, 4, trainMatrixConfigVals, fixed_ncfg);
  return trainMatrixConfig;
}

std::shared_ptr<TensorConfig> trainDenseTensor2d() {
  std::vector<double> trainTensorConfigVals = {1, 5, 9,  2, 6, 10,
                                               3, 7, 11, 4, 8, 12};
  std::shared_ptr<TensorConfig> trainTensorConfig =
      std::make_shared<TensorConfig>(std::initializer_list<uint64_t>({3, 4}),
                                     trainTensorConfigVals.data(), fixed_ncfg);
  return trainTensorConfig;
}

std::shared_ptr<TensorConfig> trainDenseTensor3d() {
  std::vector<double> trainTensorConfigVals = {1,  2,  3,  4,  5,  6,  7,  8,
                                               9,  10, 11, 12, 13, 14, 15, 16,
                                               17, 18, 19, 20, 21, 22, 23, 24};
  std::shared_ptr<TensorConfig> trainTensorConfig =
      std::make_shared<TensorConfig>(std::initializer_list<uint64_t>({2, 3, 4}),
                                     trainTensorConfigVals, fixed_ncfg);
  return trainTensorConfig;
}

// sparse train data (matrix/tensor 2d)

std::shared_ptr<MatrixConfig> trainSparseMatrix() {
  std::vector<std::uint32_t> trainMatrixConfigRows = {0, 0, 0, 0, 2, 2, 2, 2};
  std::vector<std::uint32_t> trainMatrixConfigCols = {0, 1, 2, 3, 0, 1, 2, 3};
  std::vector<double> trainMatrixConfigVals = {1, 2, 3, 4, 9, 10, 11, 12};
  std::shared_ptr<MatrixConfig> trainMatrixConfig =
      std::make_shared<MatrixConfig>(3, 4, trainMatrixConfigRows,
                                     trainMatrixConfigCols,
                                     trainMatrixConfigVals, fixed_ncfg, true);
  return trainMatrixConfig;
}

std::shared_ptr<TensorConfig> trainSparseTensor2d() {
  std::vector<std::vector<std::uint32_t>> trainTensorConfigCols = {
      {0, 0, 0, 0, 2, 2, 2, 2}, {0, 1, 2, 3, 0, 1, 2, 3}};
  std::vector<double> trainTensorConfigVals = {1, 2, 3, 4, 9, 10, 11, 12};
  std::shared_ptr<TensorConfig> trainTensorConfig =
      std::make_shared<TensorConfig>(std::initializer_list<uint64_t>({3, 4}),
                                     trainTensorConfigCols,
                                     trainTensorConfigVals, fixed_ncfg, true);
  return trainTensorConfig;
}

// sparse test data (matrix/tensor 2d/tensor 3d)

std::shared_ptr<MatrixConfig> testSparseMatrix() {
  std::vector<std::uint32_t> testMatrixConfigRows = {0, 0, 0, 0, 2, 2, 2, 2};
  std::vector<std::uint32_t> testMatrixConfigCols = {0, 1, 2, 3, 0, 1, 2, 3};
  std::vector<double> testMatrixConfigVals = {1, 2, 3, 4, 9, 10, 11, 12};
  std::shared_ptr<MatrixConfig> testMatrixConfig =
      std::make_shared<MatrixConfig>(3, 4, testMatrixConfigRows,
                                     testMatrixConfigCols, testMatrixConfigVals,
                                     fixed_ncfg, true);
  return testMatrixConfig;
}

std::shared_ptr<TensorConfig> testSparseTensor2d() {
  std::vector<std::vector<std::uint32_t>> testTensorConfigCols = {
      {0, 0, 0, 0, 2, 2, 2, 2}, {0, 1, 2, 3, 0, 1, 2, 3}};
  std::vector<double> testTensorConfigVals = {1, 2, 3, 4, 9, 10, 11, 12};
  std::shared_ptr<TensorConfig> testTensorConfig =
      std::make_shared<TensorConfig>(std::initializer_list<uint64_t>({3, 4}),
                                     testTensorConfigCols, testTensorConfigVals,
                                     fixed_ncfg, true);
  return testTensorConfig;
}

std::shared_ptr<TensorConfig> testSparseTensor3d() {
  std::vector<std::vector<std::uint32_t>> testTensorConfigCols = {
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 2, 2, 2, 2},
      {0, 1, 2, 3, 0, 1, 2, 3}};
  std::vector<double> testTensorConfigVals = {1, 2, 3, 4, 9, 10, 11, 12};
  std::shared_ptr<TensorConfig> testTensorConfig =
      std::make_shared<TensorConfig>(std::initializer_list<uint64_t>({2, 3, 4}),
                                     testTensorConfigCols, testTensorConfigVals,
                                     fixed_ncfg, true);
  return testTensorConfig;
}

// aux data

std::shared_ptr<MatrixConfig> rowAuxDense() {
  std::vector<double> rowAuxDataDenseMatrixConfigVals = {1, 2, 3};
  std::shared_ptr<MatrixConfig> rowAuxDataDenseMatrixConfig =
      std::make_shared<MatrixConfig>(3, 1, rowAuxDataDenseMatrixConfigVals,
                                     fixed_ncfg);
  rowAuxDataDenseMatrixConfig->setPos(PVec<>({0, 1}));
  return rowAuxDataDenseMatrixConfig;
}

std::shared_ptr<MatrixConfig> colAuxDense() {
  std::vector<double> colAuxDataDenseMatrixConfigVals = {1, 2, 3, 4};
  std::shared_ptr<MatrixConfig> colAuxDataDenseMatrixConfig =
      std::make_shared<MatrixConfig>(1, 4, colAuxDataDenseMatrixConfigVals,
                                     fixed_ncfg);
  colAuxDataDenseMatrixConfig->setPos(PVec<>({1, 0}));
  return colAuxDataDenseMatrixConfig;
}

// side info

std::shared_ptr<MatrixConfig> rowSideDenseMatrix() {
  NoiseConfig nc(NoiseTypes::sampled);
  nc.setPrecision(10.0);

  std::vector<double> rowSideInfoDenseMatrixConfigVals = {1, 2, 3};
  std::shared_ptr<MatrixConfig> rowSideInfoDenseMatrixConfig =
      std::make_shared<MatrixConfig>(3, 1, rowSideInfoDenseMatrixConfigVals,
                                     nc);
  return rowSideInfoDenseMatrixConfig;
}

std::shared_ptr<MatrixConfig> colSideDenseMatrix() {
  NoiseConfig nc(NoiseTypes::sampled);
  nc.setPrecision(10.0);

  std::vector<double> colSideInfoDenseMatrixConfigVals = {1, 2, 3, 4};
  std::shared_ptr<MatrixConfig> colSideInfoDenseMatrixConfig =
      std::make_shared<MatrixConfig>(4, 1, colSideInfoDenseMatrixConfigVals,
                                     nc);
  return colSideInfoDenseMatrixConfig;
}

std::shared_ptr<MatrixConfig> rowSideSparseMatrix() {
  NoiseConfig nc(NoiseTypes::sampled);
  nc.setPrecision(10.0);

  std::vector<std::uint32_t> rowSideInfoSparseMatrixConfigRows = {0, 1, 2};
  std::vector<std::uint32_t> rowSideInfoSparseMatrixConfigCols = {0, 0, 0};
  std::vector<double> rowSideInfoSparseMatrixConfigVals = {1, 2, 3};
  std::shared_ptr<MatrixConfig> rowSideInfoSparseMatrixConfig =
      std::make_shared<MatrixConfig>(3, 1, rowSideInfoSparseMatrixConfigRows,
                                     rowSideInfoSparseMatrixConfigCols,
                                     rowSideInfoSparseMatrixConfigVals, nc,
                                     true);
  return rowSideInfoSparseMatrixConfig;
}

std::shared_ptr<MatrixConfig> colSideSparseMatrix() {
  NoiseConfig nc(NoiseTypes::sampled);
  nc.setPrecision(10.0);

  std::vector<std::uint32_t> colSideInfoSparseMatrixConfigRows = {0, 1, 2, 3};
  std::vector<std::uint32_t> colSideInfoSparseMatrixConfigCols = {0, 0, 0, 0};
  std::vector<double> colSideInfoSparseMatrixConfigVals = {1, 2, 3, 4};
  std::shared_ptr<MatrixConfig> colSideInfoSparseMatrixConfig =
      std::make_shared<MatrixConfig>(4, 1, colSideInfoSparseMatrixConfigRows,
                                     colSideInfoSparseMatrixConfigCols,
                                     colSideInfoSparseMatrixConfigVals, nc,
                                     true);
  return colSideInfoSparseMatrixConfig;
}

std::shared_ptr<MatrixConfig> rowSideDenseMatrix3d() {
  NoiseConfig nc(NoiseTypes::sampled);
  nc.setPrecision(10.0);

  std::vector<double> rowSideInfoDenseMatrixConfigVals = {1, 2, 3, 4, 5, 6};
  std::shared_ptr<MatrixConfig> rowSideInfoDenseMatrixConfig =
      std::make_shared<MatrixConfig>(2, 3, rowSideInfoDenseMatrixConfigVals,
                                     nc);
  return rowSideInfoDenseMatrixConfig;
}

std::shared_ptr<SideInfoConfig> toSide(std::shared_ptr<MatrixConfig> mcfg,
                                       bool direct = true,
                                       double tol = 1e-6) {
  std::shared_ptr<SideInfoConfig> picfg = std::make_shared<SideInfoConfig>();
  picfg->setSideInfo(mcfg);
  picfg->setDirect(direct);
  picfg->setTol(tol);

  return picfg;
}

std::shared_ptr<SideInfoConfig> rowSideDense(bool direct = true,
                                                          double tol = 1e-6) {
  std::shared_ptr<MatrixConfig> mcfg = rowSideDenseMatrix();

  std::shared_ptr<SideInfoConfig> picfg = std::make_shared<SideInfoConfig>();
  picfg->setSideInfo(mcfg);
  picfg->setDirect(direct);
  picfg->setTol(tol);

  return picfg;
}

std::shared_ptr<SideInfoConfig> colSideDense(bool direct = true,
                                                          double tol = 1e-6) {
  std::shared_ptr<MatrixConfig> mcfg = colSideDenseMatrix();

  std::shared_ptr<SideInfoConfig> picfg = std::make_shared<SideInfoConfig>();
  picfg->setSideInfo(mcfg);
  picfg->setDirect(direct);
  picfg->setTol(tol);

  return picfg;
}

std::shared_ptr<SideInfoConfig> rowSideSparse(bool direct = true,
                                                           double tol = 1e-6) {
  std::shared_ptr<MatrixConfig> mcfg = rowSideSparseMatrix();

  std::shared_ptr<SideInfoConfig> picfg = std::make_shared<SideInfoConfig>();
  picfg->setSideInfo(mcfg);
  picfg->setDirect(direct);
  picfg->setTol(tol);

  return picfg;
}

std::shared_ptr<SideInfoConfig> colSideSparse(bool direct = true,
                                                           double tol = 1e-6) {
  std::shared_ptr<MatrixConfig> mcfg = colSideSparseMatrix();

  std::shared_ptr<SideInfoConfig> picfg = std::make_shared<SideInfoConfig>();
  picfg->setSideInfo(mcfg);
  picfg->setDirect(direct);
  picfg->setTol(tol);

  return picfg;
}

std::shared_ptr<SideInfoConfig>
rowSideDenseMacauPrior3d(bool direct = true, double tol = 1e-6) {
  std::shared_ptr<MatrixConfig> mcfg = rowSideDenseMatrix3d();

  std::shared_ptr<SideInfoConfig> picfg = std::make_shared<SideInfoConfig>();
  picfg->setSideInfo(mcfg);
  picfg->setDirect(direct);
  picfg->setTol(tol);

  return picfg;
}

// result comparison

void REQUIRE_RESULT_ITEMS(const std::vector<ResultItem> &actualResultItems,
                          const std::vector<ResultItem> &expectedResultItems) {
  REQUIRE(actualResultItems.size() == expectedResultItems.size());
  double single_item_epsilon = APPROX_EPSILON * 10;
  for (std::vector<ResultItem>::size_type i = 0; i < actualResultItems.size();
       i++) {
    const ResultItem &actualResultItem = actualResultItems[i];
    const ResultItem &expectedResultItem = expectedResultItems[i];
    REQUIRE(actualResultItem.coords == expectedResultItem.coords);
    REQUIRE(actualResultItem.val == expectedResultItem.val);
    REQUIRE(
        actualResultItem.pred_1sample ==
        Approx(expectedResultItem.pred_1sample).epsilon(single_item_epsilon));
    REQUIRE(actualResultItem.pred_avg ==
            Approx(expectedResultItem.pred_avg).epsilon(single_item_epsilon));
    REQUIRE(actualResultItem.var ==
            Approx(expectedResultItem.var).epsilon(single_item_epsilon));
  }
}

void runAndCheck(int nr, Config config) {
  std::shared_ptr<ISession> session = SessionFactory::create_session(config);
  session->run();

  double actualRmseAvg = session->getRmseAvg();
  const std::vector<ResultItem> &actualResults = session->getResultItems();

  PRINT_ACTUAL_RESULTS(nr)
  double &expectedRmseAvg = expectedResults[nr].rmseAvg;
  auto &expectedResultItems = expectedResults[nr].resultItems;

  REQUIRE(actualRmseAvg == Approx(expectedRmseAvg).epsilon(APPROX_EPSILON));
  REQUIRE_RESULT_ITEMS(actualResults, expectedResultItems);
}

void compareSessions(Config &matrixSessionConfig, Config &tensorSessionConfig) {
  std::shared_ptr<ISession> matrixSession =
      SessionFactory::create_session(matrixSessionConfig);
  std::shared_ptr<ISession> tensorSession =
      SessionFactory::create_session(tensorSessionConfig);
  matrixSession->run();
  tensorSession->run();

  REQUIRE(matrixSession->getRmseAvg() ==
          Approx(tensorSession->getRmseAvg()).epsilon(APPROX_EPSILON));
  REQUIRE_RESULT_ITEMS(matrixSession->getResultItems(),
                       tensorSession->getResultItems());
}

//
//      train: dense matrix
//       test: sparse matrix
//     priors: normal normal
//   aux-data: none none
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "normal normal --aux-data none none",
          TAG_MATRIX_TESTS) {

  runAndCheck(359, genConfig(trainDenseMatrix(),
                             testSparseMatrix(),
                             {PriorTypes::normal, PriorTypes::normal}));
}

//
//      train: sparse matrix
//       test: sparse matrix
//     priors: normal normal
//   aux-data: none none
//
TEST_CASE("--train <train_sparse_matrix> --test <test_sparse_matrix> --prior "
          "normal normal --aux-data none none",
          TAG_MATRIX_TESTS) {

  runAndCheck(411, genConfig(trainSparseMatrix(),
                             testSparseMatrix(),
                             {PriorTypes::normal, PriorTypes::normal}));
}

//
//      train: dense matrix
//       test: sparse matrix
//     priors: normal normal
//   aux-data: dense_matrix dense_matrix
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "normal normal --aux-data <dense_matrix> <dense_matrix>",
          TAG_MATRIX_TESTS) {

  runAndCheck(467, genConfig(trainDenseMatrix(),
                             testSparseMatrix(),
                             {PriorTypes::normal, PriorTypes::normal})
                       .addAuxData({rowAuxDense()})
                       .addAuxData({colAuxDense()}));
}

//
//      train: sparse matrix
//       test: sparse matrix
//     priors: normal normal
//   aux-data: dense_matrix dense_matrix
//
TEST_CASE("--train <train_sparse_matrix> --test <test_sparse_matrix> --prior "
          "normal normal --aux-data <dense_matrix> <dense_matrix>",
          TAG_MATRIX_TESTS) {

  runAndCheck(523, genConfig(trainSparseMatrix(),
                             testSparseMatrix(),
                             {PriorTypes::normal, PriorTypes::normal})
                       .addAuxData({rowAuxDense()})
                       .addAuxData({colAuxDense()}));
}

//=================================================================

//
//      train: dense matrix
//       test: sparse matrix
//     priors: spikeandslab spikeandslab
//   aux-data: none none
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "spikeandslab spikeandslab --aux-data none none",
          TAG_MATRIX_TESTS) {

  runAndCheck(
      577, genConfig(trainDenseMatrix(), testSparseMatrix(),
                     {PriorTypes::spikeandslab, PriorTypes::spikeandslab}));
}

//
//      train: sparse matrix
//       test: sparse matrix
//     priors: spikeandslab spikeandslab
//   aux-data: none none
//
TEST_CASE("--train <train_sparse_matrix> --test <test_sparse_matrix> --prior "
          "spikeandslab spikeandslab --aux-data none none",
          TAG_MATRIX_TESTS) {

  runAndCheck(
      629, genConfig(trainSparseMatrix(), testSparseMatrix(),
                     {PriorTypes::spikeandslab, PriorTypes::spikeandslab}));
}

//
//      train: dense matrix
//       test: sparse matrix
//     priors: spikeandslab spikeandslab
//   aux-data: dense_matrix dense_matrix
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "spikeandslab spikeandslab --aux-data <dense_matrix> <dense_matrix>",
          TAG_MATRIX_TESTS) {

  runAndCheck(
      685, genConfig(trainDenseMatrix(), testSparseMatrix(),
                     {PriorTypes::spikeandslab, PriorTypes::spikeandslab})
               .addAuxData({rowAuxDense()})
               .addAuxData({colAuxDense()}));
}

//
//      train: sparse matrix
//       test: sparse matrix
//     priors: spikeandslab spikeandslab
//   aux-data: dense_matrix dense_matrix
//
TEST_CASE("--train <train_sparse_matrix> --test <test_sparse_matrix> --prior "
          "spikeandslab spikeandslab --aux-data <dense_matrix> <dense_matrix>",
          TAG_MATRIX_TESTS) {

  runAndCheck(
      741, genConfig(trainSparseMatrix(), testSparseMatrix(),
                     {PriorTypes::spikeandslab, PriorTypes::spikeandslab})
               .addAuxData({rowAuxDense()})
               .addAuxData({colAuxDense()}));
}

//=================================================================

//
//      train: dense matrix
//       test: sparse matrix
//     priors: normalone normalone
//   aux-data: none none
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "normalone normalone --aux-data none none",
          TAG_MATRIX_TESTS) {

  runAndCheck(795, genConfig(trainDenseMatrix(),
                             testSparseMatrix(),
                             {PriorTypes::normalone, PriorTypes::normalone}));
}

//
//      train: sparse matrix
//       test: sparse matrix
//     priors: normalone normalone
//   aux-data: none none
//
TEST_CASE("--train <train_sparse_matrix> --test <test_sparse_matrix> --prior "
          "normalone normalone --aux-data none none",
          TAG_MATRIX_TESTS) {

  runAndCheck(847, genConfig(trainSparseMatrix(),
                             testSparseMatrix(),
                             {PriorTypes::normalone, PriorTypes::normalone}));
}

//
//      train: dense matrix
//       test: sparse matrix
//     priors: normalone normalone
//   aux-data: dense_matrix dense_matrix
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "normalone normalone --aux-data <dense_matrix> <dense_matrix>",
          TAG_MATRIX_TESTS) {

  runAndCheck(903, genConfig(trainDenseMatrix(),
                             testSparseMatrix(),
                             {PriorTypes::normalone, PriorTypes::normalone})
                       .addAuxData({rowAuxDense()})
                       .addAuxData({colAuxDense()}));
}

//
//      train: sparse matrix
//       test: sparse matrix
//     priors: normalone normalone
//   aux-data: dense_matrix dense_matrix
//
TEST_CASE("--train <train_sparse_matrix> --test <test_sparse_matrix> --prior "
          "normalone normalone --aux-data <dense_matrix> <dense_matrix>",
          TAG_MATRIX_TESTS) {

  runAndCheck(959, genConfig(trainSparseMatrix(),
                             testSparseMatrix(),
                             {PriorTypes::normalone, PriorTypes::normalone})
                       .addAuxData({rowAuxDense()})
                       .addAuxData({colAuxDense()}));
}

//=================================================================

//
//      train: dense matrix
//       test: sparse matrix
//     priors: macau macau
//   features: row_side_info_dense_matrix col_side_info_dense_matrix
//     direct: true
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "macau macau --aux-data <row_side_info_dense_matrix> "
          "<col_side_info_dense_matrix> --direct",
          TAG_MATRIX_TESTS) {

  runAndCheck(1018, genConfig(trainDenseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::macau, PriorTypes::macau})
                        .addSideInfoConfig(0, toSide(rowSideDenseMatrix()))
                        .addSideInfoConfig(1, toSide(colSideDenseMatrix())));
}

//
//      train: sparse matrix
//       test: sparse matrix
//     priors: macau macau
//   features: row_side_info_dense_matrix col_side_info_dense_matrix
//     direct: true
//
TEST_CASE("--train <train_sparse_matrix> --test <test_sparse_matrix> --prior "
          "macau macau --aux-data <row_side_info_dense_matrix> "
          "<col_side_info_dense_matrix> --direct",
          TAG_MATRIX_TESTS) {

  runAndCheck(1075, genConfig(trainSparseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::macau, PriorTypes::macau})
                        .addSideInfoConfig(0, toSide(rowSideDenseMatrix()))
                        .addSideInfoConfig(1, toSide(colSideDenseMatrix())));
}

//=================================================================

//
//      train: dense matrix
//       test: sparse matrix
//     priors: macauone macauone
//   features: row_side_info_sparse_matrix col_side_info_sparse_matrix
//     direct: true
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "macauone macauone --aux-data <row_side_info_sparse_matrix> "
          "<col_side_info_sparse_matrix> --direct",
          TAG_MATRIX_TESTS) {

  runAndCheck(1135, genConfig(trainDenseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::macauone, PriorTypes::macauone})
                        .addSideInfoConfig(0, toSide(rowSideSparseMatrix()))
                        .addSideInfoConfig(1, toSide(colSideSparseMatrix())));
}

//
//      train: sparse matrix
//       test: sparse matrix
//     priors: macauone macauone
//   features: row_side_info_sparse_matrix col_side_info_sparse_matrix
//     direct: true
//
TEST_CASE("--train <train_sparse_matrix> --test <test_sparse_matrix> --prior "
          "macauone macauone --aux-data <row_side_info_sparse_matrix> "
          "<col_side_info_sparse_matrix> --direct",
          TAG_MATRIX_TESTS) {

  runAndCheck(1193, genConfig(trainSparseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::macauone, PriorTypes::macauone})
                        .addSideInfoConfig(0, toSide(rowSideSparseMatrix()))
                        .addSideInfoConfig(1, toSide(colSideSparseMatrix())));
}

//=================================================================

//
//      train: dense matrix
//       test: sparse matrix
//     priors: macau normal
//   features: row_side_info_dense_matrix none
//     direct: true
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "macau normal --aux-data <row_side_info_dense_matrix> none --direct",
          TAG_MATRIX_TESTS) {

  runAndCheck(1250, genConfig(trainDenseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::macau, PriorTypes::normal})
                        .addSideInfoConfig(0, toSide(rowSideDenseMatrix())));
}

//
//      train: dense matrix
//       test: sparse matrix
//     priors: normal macau
//   features: none col_side_info_dense_matrix
//     direct: true
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "normal macau --aux-data none <col_side_info_dense_matrix> --direct",
          TAG_MATRIX_TESTS) {

  runAndCheck(1305, genConfig(trainDenseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::normal, PriorTypes::macau})
                        .addSideInfoConfig(1, toSide(colSideDenseMatrix())));
}

// test throw - macau prior should have side info

//
//      train: dense matrix
//       test: sparse matrix
//     priors: macau normal
//   features: none none
//     direct: true
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "macau normal --aux-data none none --direct",
          TAG_MATRIX_TESTS) {

  Config config =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::macau, PriorTypes::normal})
          .addSideInfoConfig(1, toSide(rowSideDenseMatrix()));

  REQUIRE_THROWS(SessionFactory::create_session(config));
}

// test throw - wrong dimentions of side info

//
//      train: dense matrix
//       test: sparse matrix
//     priors: macau normal
//   features: col_side_info_dense_matrix none
//     direct: true
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "macau normal --aux-data <col_side_info_dense_matrix> none --direct",
          TAG_MATRIX_TESTS) {

  Config config =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::macau, PriorTypes::normal})
          .addSideInfoConfig(1, toSide(colSideDenseMatrix()));

  REQUIRE_THROWS(SessionFactory::create_session(config));
}

//=================================================================

//
//      train: dense matrix
//       test: sparse matrix
//     priors: normal spikeandslab
//   aux-data: none none
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "normal spikeandslab --aux-data none none",
          TAG_MATRIX_TESTS) {

  runAndCheck(1466, genConfig(trainDenseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::normal, PriorTypes::spikeandslab}));
}

//
//      train: dense matrix
//       test: sparse matrix
//     priors: spikeandslab normal
//   aux-data: none none
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "spikeandslab normal --aux-data none none",
          TAG_MATRIX_TESTS) {

  runAndCheck(1518, genConfig(trainDenseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::spikeandslab, PriorTypes::normal}));
}

//
//      train: dense matrix
//       test: sparse matrix
//     priors: normal spikeandslab
//   aux-data: none dense_matrix
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "normal spikeandslab --aux-data none <dense_matrix>",
          TAG_MATRIX_TESTS) {

  runAndCheck(1572, genConfig(trainDenseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::spikeandslab, PriorTypes::normal})
                        .addAuxData({colAuxDense()}));
}

//
//      train: dense matrix
//       test: sparse matrix
//     priors: spikeandslab normal
//   aux-data: dense_matrix none
//
TEST_CASE("--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
          "spikeandslab normal --aux-data <dense_matrix> none",
          TAG_MATRIX_TESTS) {

  runAndCheck(1626, genConfig(trainDenseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::spikeandslab, PriorTypes::normal})
                        .addAuxData({rowAuxDense()}));
}

//=================================================================

//
//      train: dense matrix
//       test: sparse matrix
//     priors: macau spikeandslab
//   features: row_side_info_dense_matrix none
//     direct: true
//
TEST_CASE(
    "--train <train_dense_matrix> --test <test_sparse_matrix> --prior macau "
    "spikeandslab --aux-data <row_side_info_dense_matrix> none --direct",
    TAG_MATRIX_TESTS) {

  runAndCheck(1683, genConfig(trainDenseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::macau, PriorTypes::spikeandslab})
                        .addSideInfoConfig(0, toSide(rowSideDenseMatrix())));
}

//
//      train: dense matrix
//       test: sparse matrix
//     priors: spikeandslab macau
//   features: none col_side_info_dense_matrix
//     direct: true
//
TEST_CASE(
    "--train <train_dense_matrix> --test <test_sparse_matrix> --prior "
    "spikeandslab macau --aux-data none <col_side_info_dense_matrix> --direct",
    TAG_MATRIX_TESTS) {

  runAndCheck(1738, genConfig(trainDenseMatrix(),
                              testSparseMatrix(),
                              {PriorTypes::spikeandslab, PriorTypes::macau})
                        .addSideInfoConfig(1, toSide(colSideDenseMatrix())));
}

//=================================================================

//
//      train: dense 2D-tensor (matrix)
//       test: sparse 2D-tensor (matrix)
//     priors: normal normal
//   aux-data: none none
//
TEST_CASE("--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normal normal --aux-data none none",
          TAG_TWO_DIMENTIONAL_TENSOR_TESTS) {

  runAndCheck(1792, genConfig(trainDenseTensor2d(),
                              testSparseTensor2d(),
                              {PriorTypes::normal, PriorTypes::normal}));
}

//
//      train: sparse 2D-tensor (matrix)
//       test: sparse 2D-tensor (matrix)
//     priors: normal normal
//   aux-data: none none
//
TEST_CASE("--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normal normal --aux-data none none",
          TAG_TWO_DIMENTIONAL_TENSOR_TESTS) {
  runAndCheck(1844, genConfig(trainSparseTensor2d(),
                              testSparseTensor2d(),
                              {PriorTypes::normal, PriorTypes::normal}));
}

//=================================================================
//
//      train: dense 2D-tensor (matrix)
//       test: sparse 2D-tensor (matrix)
//     priors: spikeandslab spikeandslab
//   aux-data: none none
//
TEST_CASE("--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior spikeandslab spikeandslab --aux-data none none",
          TAG_TWO_DIMENTIONAL_TENSOR_TESTS) {
  runAndCheck(1898,
              genConfig(trainDenseTensor2d(),
                        testSparseTensor2d(),
                        {PriorTypes::spikeandslab, PriorTypes::spikeandslab}));
}

//
//      train: sparse 2D-tensor (matrix)
//       test: sparse 2D-tensor (matrix)
//     priors: spikeandslab spikeandslab
//   aux-data: none none
//
TEST_CASE("--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior spikeandslab spikeandslab --aux-data none none",
          TAG_TWO_DIMENTIONAL_TENSOR_TESTS) {
  runAndCheck(1950,
              genConfig(trainSparseTensor2d(),
                        testSparseTensor2d(),
                        {PriorTypes::spikeandslab, PriorTypes::spikeandslab}));
}

//=================================================================

//
//      train: dense 2D-tensor (matrix)
//       test: sparse 2D-tensor (matrix)
//     priors: normalone normalone
//   aux-data: none none
//
TEST_CASE("--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normalone normalone --aux-data none none",
          TAG_TWO_DIMENTIONAL_TENSOR_TESTS) {
  runAndCheck(2004, genConfig(trainDenseTensor2d(),
                              testSparseTensor2d(),
                              {PriorTypes::normalone, PriorTypes::normalone}));
}

//
//      train: sparse 2D-tensor (matrix)
//       test: sparse 2D-tensor (matrix)
//     priors: normalone normalone
//   aux-data: none none
//
TEST_CASE("--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normalone normalone --aux-data none none",
          TAG_TWO_DIMENTIONAL_TENSOR_TESTS) {
  runAndCheck(2056, genConfig(trainSparseTensor2d(),
                              testSparseTensor2d(),
                              {PriorTypes::normalone, PriorTypes::normalone}));
}

//=================================================================

//
//      train: dense 3D-tensor (matrix)
//       test: sparse 3D-tensor (matrix)
//     priors: normal normal normal
//   aux-data: none none
//
TEST_CASE("--train <train_dense_3d_tensor> --test <test_sparse_3d_tensor> "
          "--prior normal normal --aux-data none none",
          TAG_THREE_DIMENTIONAL_TENSOR_TESTS) {
  runAndCheck(
      2110,
      genConfig(trainDenseTensor3d(), testSparseTensor3d(),
                {PriorTypes::normal, PriorTypes::normal, PriorTypes::normal}));
}

//=================================================================

//
//      train: dense 3D-tensor (matrix)
//       test: sparse 3D-tensor (matrix)
//     priors: spikeandslab spikeandslab
//   aux-data: none none
//
TEST_CASE("--train <train_dense_3d_tensor> --test <test_sparse_3d_tensor> "
          "--prior spikeandslab spikeandslab --aux-data none none",
          TAG_THREE_DIMENTIONAL_TENSOR_TESTS) {

  runAndCheck(2164,
              genConfig(trainDenseTensor3d(),
                        testSparseTensor3d(),
                        {PriorTypes::spikeandslab, PriorTypes::spikeandslab,
                         PriorTypes::spikeandslab}));
}

//=================================================================

// not sure if this test produces correct results

//
//      train: dense 3D-tensor
//       test: sparse 3D-tensor
//     priors: macau normal
//   aux-data: row_dense_side_info none
//
TEST_CASE("--train <train_dense_3d_tensor> --test <test_sparse_3d_tensor> "
          "--prior macau normal --side-info row_dense_side_info none",
          TAG_THREE_DIMENTIONAL_TENSOR_TESTS) {
  runAndCheck(
      2222,
      genConfig(trainDenseTensor3d(), testSparseTensor3d(),
                {PriorTypes::macau, PriorTypes::normal, PriorTypes::normal})
          .addSideInfoConfig(0, rowSideDenseMacauPrior3d()));
}

//=================================================================

// not sure if this test produces correct results

//
//      train: dense 3D-tensor
//       test: sparse 3D-tensor
//     priors: macauone normal
//   aux-data: row_dense_side_info none
//
TEST_CASE("--train <train_dense_3d_tensor> --test <test_sparse_3d_tensor> "
          "--prior macauone normal --side-info row_dense_side_info none",
          TAG_THREE_DIMENTIONAL_TENSOR_TESTS "[!mayfail]") {
  runAndCheck(
      2280,
      genConfig(trainDenseTensor3d(), testSparseTensor3d(),
                {PriorTypes::macauone, PriorTypes::normal, PriorTypes::normal})
          .addSideInfoConfig(0, rowSideDenseMacauPrior3d()));
}

//=================================================================

// pairwise tests for 2d matrix vs 2d tensor
// normal normal
// normal spikeandslab
// spikeandslab normal
// spikeandslab spikeandslab

//
//      train: 1. dense 2D-tensor (matrix)
//             2. dense matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: normal normal
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_dense_matrix>    --test <test_sparse_matrix>    "
          "--prior normal normal --aux-data none none"
          "--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normal normal --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::normal, PriorTypes::normal});
  Config tensorSessionConfig =
      genConfig(trainDenseTensor2d(), testSparseTensor2d(),
                {PriorTypes::normal, PriorTypes::normal});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: normal normal
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_sparse_matrix>    --test <test_sparse_matrix>    "
          "--prior normal normal --aux-data none none"
          "--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normal normal --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainSparseMatrix(), testSparseMatrix(),
                {PriorTypes::normal, PriorTypes::normal});
  Config tensorSessionConfig =
      genConfig(trainSparseTensor2d(), testSparseTensor2d(),
                {PriorTypes::normal, PriorTypes::normal});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. dense 2D-tensor (matrix)
//             2. dense matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: normal spikeandslab
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_dense_matrix>    --test <test_sparse_matrix>    "
          "--prior normal spikeandslab --aux-data none none"
          "--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normal spikeandslab --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::normal, PriorTypes::spikeandslab});
  Config tensorSessionConfig =
      genConfig(trainDenseTensor2d(), testSparseTensor2d(),
                {PriorTypes::normal, PriorTypes::spikeandslab});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: normal spikeandslab
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_sparse_matrix>    --test <test_sparse_matrix>    "
          "--prior normal spikeandslab --aux-data none none"
          "--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normal spikeandslab --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainSparseMatrix(), testSparseMatrix(),
                {PriorTypes::normal, PriorTypes::spikeandslab});
  Config tensorSessionConfig =
      genConfig(trainSparseTensor2d(), testSparseTensor2d(),
                {PriorTypes::normal, PriorTypes::spikeandslab});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. dense 2D-tensor (matrix)
//             2. dense matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: spikeandslab normal
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_dense_matrix>    --test <test_sparse_matrix>    "
          "--prior spikeandslab normal --aux-data none none"
          "--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior spikeandslab normal --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::spikeandslab, PriorTypes::normal});
  Config tensorSessionConfig =
      genConfig(trainDenseTensor2d(), testSparseTensor2d(),
                {PriorTypes::spikeandslab, PriorTypes::normal});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: spikeandslab normal
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_sparse_matrix>    --test <test_sparse_matrix>    "
          "--prior spikeandslab normal --aux-data none none"
          "--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior spikeandslab normal --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainSparseMatrix(), testSparseMatrix(),
                {PriorTypes::spikeandslab, PriorTypes::normal});
  Config tensorSessionConfig =
      genConfig(trainSparseTensor2d(), testSparseTensor2d(),
                {PriorTypes::spikeandslab, PriorTypes::normal});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. dense 2D-tensor (matrix)
//             2. dense matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: spikeandslab spikeandslab
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_dense_matrix>    --test <test_sparse_matrix>    "
          "--prior spikeandslab spikeandslab --aux-data none none"
          "--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior spikeandslab spikeandslab --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::spikeandslab, PriorTypes::spikeandslab});
  Config tensorSessionConfig =
      genConfig(trainDenseTensor2d(), testSparseTensor2d(),
                {PriorTypes::spikeandslab, PriorTypes::spikeandslab});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: spikeandslab spikeandslab
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_sparse_matrix>    --test <test_sparse_matrix>    "
          "--prior spikeandslab spikeandslab --aux-data none none"
          "--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior spikeandslab spikeandslab --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainSparseMatrix(), testSparseMatrix(),
                {PriorTypes::spikeandslab, PriorTypes::spikeandslab});
  Config tensorSessionConfig =
      genConfig(trainSparseTensor2d(), testSparseTensor2d(),
                {PriorTypes::spikeandslab, PriorTypes::spikeandslab});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//==========================================================================

//
//      train: 1. dense 2D-tensor (matrix)
//             2. dense matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: normal normalone
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_dense_matrix>    --test <test_sparse_matrix>    "
          "--prior normal normalone --aux-data none none"
          "--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normal normalone --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::normal, PriorTypes::normalone});
  Config tensorSessionConfig =
      genConfig(trainDenseTensor2d(), testSparseTensor2d(),
                {PriorTypes::normal, PriorTypes::normalone});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: normal normalone
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_sparse_matrix>    --test <test_sparse_matrix>    "
          "--prior normal normalone --aux-data none none"
          "--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normal normalone --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainSparseMatrix(), testSparseMatrix(),
                {PriorTypes::normal, PriorTypes::normalone});
  Config tensorSessionConfig =
      genConfig(trainSparseTensor2d(), testSparseTensor2d(),
                {PriorTypes::normal, PriorTypes::normalone});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. dense 2D-tensor (matrix)
//             2. dense matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: normalone normal
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_dense_matrix>    --test <test_sparse_matrix>    "
          "--prior normalone normal --aux-data none none"
          "--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normalone normal --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::normalone, PriorTypes::normal});
  Config tensorSessionConfig =
      genConfig(trainDenseTensor2d(), testSparseTensor2d(),
                {PriorTypes::normalone, PriorTypes::normal});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: normalone normal
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_sparse_matrix>    --test <test_sparse_matrix>    "
          "--prior normalone normal --aux-data none none"
          "--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normalone normal --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainSparseMatrix(), testSparseMatrix(),
                {PriorTypes::normalone, PriorTypes::normal});
  Config tensorSessionConfig =
      genConfig(trainSparseTensor2d(), testSparseTensor2d(),
                {PriorTypes::normalone, PriorTypes::normal});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. dense 2D-tensor (matrix)
//             2. dense matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: normalone normalone
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_dense_matrix>    --test <test_sparse_matrix>    "
          "--prior normalone normalone --aux-data none none"
          "--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normalone normalone --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::normalone, PriorTypes::normalone});
  Config tensorSessionConfig =
      genConfig(trainDenseTensor2d(), testSparseTensor2d(),
                {PriorTypes::normalone, PriorTypes::normalone});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//
//      train: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: normalone normalone
//   aux-data: none none
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_sparse_matrix>    --test <test_sparse_matrix>    "
          "--prior normalone normalone --aux-data none none"
          "--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior normalone normalone --aux-data none none",
          TAG_VS_TESTS) {
  Config matrixSessionConfig =
      genConfig(trainSparseMatrix(), testSparseMatrix(),
                {PriorTypes::normalone, PriorTypes::normalone});
  Config tensorSessionConfig =
      genConfig(trainSparseTensor2d(), testSparseTensor2d(),
                {PriorTypes::normalone, PriorTypes::normalone});
  compareSessions(matrixSessionConfig, tensorSessionConfig);
}

//==========================================================================

//
//      train: 1. dense 2D-tensor (matrix)
//             2. dense matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: macau macau
//  side-info: row_side_info_dense_matrix col_side_info_dense_matrix
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior macau macau --side-info <row_side_info_dense_matrix> "
          "<col_side_info_dense_matrix> --direct"
          "--train <train_dense_matrix>    --test <test_sparse_matrix>    "
          "--prior macau macau --side-info <row_side_info_dense_matrix> "
          "<col_side_info_dense_matrix> --direct",
          TAG_VS_TESTS) {

  Config tensorRunConfig =
      genConfig(trainDenseTensor2d(), testSparseTensor2d(),
                {PriorTypes::macau, PriorTypes::macau})
          .addSideInfoConfig(0, toSide(rowSideDenseMatrix()))
          .addSideInfoConfig(1, toSide(colSideDenseMatrix()));
  Config matrixRunConfig =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::macau, PriorTypes::macau})
          .addSideInfoConfig(0, toSide(rowSideDenseMatrix()))
          .addSideInfoConfig(1, toSide(colSideDenseMatrix()));
  compareSessions(tensorRunConfig, matrixRunConfig);
  ;
}

//
//      train: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: macau macau
//  side-info: row_side_info_dense_matrix col_side_info_dense_matrix
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior macau macau --side-info <row_side_info_dense_matrix> "
          "<col_side_info_dense_matrix> --direct"
          "--train <train_sparse_matrix>    --test <test_sparse_matrix>    "
          "--prior macau macau --side-info <row_side_info_dense_matrix> "
          "<col_side_info_dense_matrix> --direct",
          TAG_VS_TESTS) {

  Config tensorRunConfig =
      genConfig(trainSparseTensor2d(), testSparseTensor2d(),
                {PriorTypes::macau, PriorTypes::macau})
          .addSideInfoConfig(0, toSide(rowSideDenseMatrix()))
          .addSideInfoConfig(1, toSide(colSideDenseMatrix()));
  Config matrixRunConfig =
      genConfig(trainSparseMatrix(), testSparseMatrix(),
                {PriorTypes::macau, PriorTypes::macau})
          .addSideInfoConfig(0, toSide(rowSideDenseMatrix()))
          .addSideInfoConfig(1, toSide(colSideDenseMatrix()));
  compareSessions(tensorRunConfig, matrixRunConfig);
  ;
}

//
//      train: 1. dense 2D-tensor (matrix)
//             2. dense matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: macauone macauone
//  side-info: row_side_info_dense_matrix col_side_info_dense_matrix
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_dense_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior macauone macauone --side-info <row_side_info_dense_matrix> "
          "<col_side_info_dense_matrix> --direct"
          "--train <train_dense_matrix>    --test <test_sparse_matrix>    "
          "--prior macauone macauone --side-info <row_side_info_dense_matrix> "
          "<col_side_info_dense_matrix> --direct",
          TAG_VS_TESTS) {

  Config tensorRunConfig =
      genConfig(trainDenseTensor2d(), testSparseTensor2d(),
                {PriorTypes::macauone, PriorTypes::macauone})
          .addSideInfoConfig(0, toSide(rowSideDenseMatrix()))
          .addSideInfoConfig(1, toSide(colSideDenseMatrix()));
  Config matrixRunConfig =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::macauone, PriorTypes::macauone})
          .addSideInfoConfig(0, toSide(rowSideDenseMatrix()))
          .addSideInfoConfig(1, toSide(colSideDenseMatrix()));
  compareSessions(tensorRunConfig, matrixRunConfig);
  ;
}

//
//      train: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//       test: 1. sparse 2D-tensor (matrix)
//             2. sparse matrix
//     priors: macauone macauone
//  side-info: row_side_info_dense_matrix col_side_info_dense_matrix
//
TEST_CASE("matrix vs 2D-tensor"
          "--train <train_sparse_2d_tensor> --test <test_sparse_2d_tensor> "
          "--prior macauone macauone --side-info <row_side_info_dense_matrix> "
          "<col_side_info_dense_matrix> --direct"
          "--train <train_sparse_matrix>    --test <test_sparse_matrix>    "
          "--prior macauone macauone --side-info <row_side_info_dense_matrix> "
          "<col_side_info_dense_matrix> --direct",
          TAG_VS_TESTS) {

  Config tensorRunConfig =
      genConfig(trainSparseTensor2d(), testSparseTensor2d(),
                {PriorTypes::macauone, PriorTypes::macauone})
          .addSideInfoConfig(0, toSide(rowSideDenseMatrix()))
          .addSideInfoConfig(1, toSide(colSideDenseMatrix()));
  Config matrixRunConfig =
      genConfig(trainSparseMatrix(), testSparseMatrix(),
                {PriorTypes::macauone, PriorTypes::macauone})
          .addSideInfoConfig(0, toSide(rowSideDenseMatrix()))
          .addSideInfoConfig(1, toSide(colSideDenseMatrix()));
  compareSessions(tensorRunConfig, matrixRunConfig);
  ;
}

TEST_CASE("PredictSession/BPMF") {

  Config config =
      genConfig(trainDenseMatrix(), testSparseMatrix(),
                {PriorTypes::normal, PriorTypes::normal});
  config.setSaveFreq(1);

  std::shared_ptr<ISession> session = SessionFactory::create_session(config);
  session->run();

  // std::cout << "Prediction from Session RMSE: " << session->getRmseAvg() <<
  // std::endl;

  std::string root_fname = session->getRootFile()->getFullPath();
  auto rf = std::make_shared<RootFile>(root_fname);

  {
    PredictSession s(rf);

    // test predict from TensorConfig
    auto result = s.predict(config.getTest());

    // std::cout << "Prediction from RootFile RMSE: " << result->rmse_avg <<
    // std::endl;
    REQUIRE(session->getRmseAvg() ==
            Approx(result->rmse_avg).epsilon(APPROX_EPSILON));
  }

  {
    PredictSession s(rf, config);
    s.run();
    auto result = s.getResult();

    // std::cout << "Prediction from RootFile+Config RMSE: " << result->rmse_avg
    // << std::endl;
    REQUIRE(session->getRmseAvg() ==
            Approx(result->rmse_avg).epsilon(APPROX_EPSILON));
  }
}

//=================================================================

//
//      train: dense matrix
//       test: sparse matrix
//     priors: macau normal
//   features: row_side_info_dense_matrix none
//     direct: true
//
TEST_CASE("PredictSession/Features/1", TAG_MATRIX_TESTS) {
  std::shared_ptr<MatrixConfig> trainDenseMatrixConfig =
      trainDenseMatrix();
  std::shared_ptr<MatrixConfig> testSparseMatrixConfig =
      testSparseMatrix();
  std::shared_ptr<SideInfoConfig> rowSideInfoDenseMatrixConfig =
      toSide(rowSideDenseMatrix());

  Config config = genConfig(trainDenseMatrixConfig, testSparseMatrixConfig,
                            {PriorTypes::macau, PriorTypes::normal})
                      .addSideInfoConfig(0, rowSideInfoDenseMatrixConfig);
  config.setSaveFreq(1);

  std::shared_ptr<ISession> session = SessionFactory::create_session(config);
  session->run();

  PredictSession predict_session(session->getRootFile());

  auto sideInfoMatrix = matrix_utils::dense_to_eigen(
      *rowSideInfoDenseMatrixConfig->getSideInfo());
  auto trainMatrix =
      smurff::matrix_utils::dense_to_eigen(*trainDenseMatrixConfig);

#if 0
    std::cout << "sideInfo =\n" << sideInfoMatrix << std::endl;
    std::cout << "train    =\n" << trainMatrix << std::endl;
#endif

  for (int r = 0; r < sideInfoMatrix.rows(); r++) {
#if 0
        std::cout << "=== row " << r << " ===\n";
#endif

    auto predictions = predict_session.predict(0, sideInfoMatrix.row(r));
#if 0
        int i = 0;
        for (auto P : predictions)
        {
            std::cout << "p[" << i++ << "] = " << P->transpose() << std::endl;
        }
#endif
  }
}

TEST_CASE("PredictSession/Features/2", TAG_MATRIX_TESTS) {
  /*
       BetaPrecision: 1.00
  U = np.array([ [ 1, 2, -1, -2  ] ])
  V = np.array([ [ 2, 2, 1, 2 ] ])
  U*V =
    [[ 2,  2,  1,  2],
     [ 4,  4,  2,  4],
     [-2, -2, -1, -2],
     [-4, -4, -2, -4]])
  */

  std::shared_ptr<MatrixConfig> trainMatrixConfig;
  {
    std::vector<std::uint32_t> trainMatrixConfigRows = {0, 0, 1, 1, 2, 2};
    std::vector<std::uint32_t> trainMatrixConfigCols = {0, 1, 2, 3, 0, 1};
    std::vector<double> trainMatrixConfigVals = {2, 2, 2, 4, -2, -2};
    // std::vector<std::uint32_t> trainMatrixConfigRows = {0, 0, 1, 1, 2, 2, 3,
    // 3}; std::vector<std::uint32_t> trainMatrixConfigCols = {0, 1, 2, 3, 0, 1,
    // 2, 3}; std::vector<double> trainMatrixConfigVals = {2, 2, 2, 4, -2, -2,
    // -2, -4};
    fixed_ncfg.setPrecision(1.);
    trainMatrixConfig = std::make_shared<MatrixConfig>(
        4, 4, trainMatrixConfigRows, trainMatrixConfigCols,
        trainMatrixConfigVals, fixed_ncfg, true);
  }

  std::shared_ptr<MatrixConfig> testMatrixConfig;
  {
    std::vector<std::uint32_t> testMatrixConfigRows = {0, 0, 0, 0, 1, 1, 1, 1,
                                                       2, 2, 2, 2, 3, 3, 3, 3};
    std::vector<std::uint32_t> testMatrixConfigCols = {0, 1, 2, 3, 0, 1, 2, 3,
                                                       0, 1, 2, 3, 0, 1, 2, 3};
    std::vector<double> testMatrixConfigVals = {2,  2,  1,  2,  4,  4,  2,  4,
                                                -2, -2, -1, -2, -4, -4, -2, -4};
    testMatrixConfig = std::make_shared<MatrixConfig>(
        4, 4, testMatrixConfigRows, testMatrixConfigCols, testMatrixConfigVals,
        fixed_ncfg, true);
  }

  std::shared_ptr<SideInfoConfig> rowSideInfoConfig;
  {
    NoiseConfig nc(NoiseTypes::sampled);
    nc.setPrecision(10.0);

    std::vector<std::uint32_t> rowSideInfoSparseMatrixConfigRows = {0, 1, 2, 3};
    std::vector<std::uint32_t> rowSideInfoSparseMatrixConfigCols = {0, 0, 0, 0};
    std::vector<double> rowSideInfoSparseMatrixConfigVals = {2, 4, -2, -4};

    auto mcfg = std::make_shared<MatrixConfig>(
        4, 1, rowSideInfoSparseMatrixConfigRows,
        rowSideInfoSparseMatrixConfigCols, rowSideInfoSparseMatrixConfigVals,
        nc, true);

    rowSideInfoConfig = std::make_shared<SideInfoConfig>();
    rowSideInfoConfig->setSideInfo(mcfg);
    rowSideInfoConfig->setDirect(true);
  }
  Config config = genConfig(trainMatrixConfig, testMatrixConfig,
                            {PriorTypes::macau, PriorTypes::normal})
                      .addSideInfoConfig(0, rowSideInfoConfig);
  config.setSaveFreq(1);

  std::shared_ptr<ISession> session = SessionFactory::create_session(config);
  session->run();

  PredictSession predict_session_in(session->getRootFile());
  auto in_matrix_predictions =
      predict_session_in.predict(config.getTest())->m_predictions;

  PredictSession predict_session_out(session->getRootFile());
  auto sideInfoMatrix =
      matrix_utils::sparse_to_eigen(*rowSideInfoConfig->getSideInfo());
  int d = config.getTrain()->getDims()[0];
  for (int r = 0; r < d; r++) {
    auto feat = sideInfoMatrix.row(r).transpose();
    auto out_of_matrix_predictions = predict_session_out.predict(0, feat);
    // Vector out_of_matrix_averages =
    // out_of_matrix_predictions->colwise().mean();

#undef DEBUG_OOM_PREDICT
#ifdef DEBUG_OOM_PREDICT
    for (auto p : in_matrix_predictions) {
      if (p.coords[0] == r) {
        std::cout << "in: " << p << std::endl;
        std::cout << "  out: " << out_of_matrix_averages.row(p.coords[1])
                  << std::endl;
      }
    }
#endif
  }
}
