// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_NODE_ND4J_GRAPH_H_
#define FLATBUFFERS_GENERATED_NODE_ND4J_GRAPH_H_

#include "flatbuffers/flatbuffers.h"

namespace nd4j {
namespace graph {

struct LongPair;

struct IntPair;

struct IntTriple;

struct FlatVariable;

struct FlatNode;

enum OpType {
  OpType_TRANSFORM = 0,
  OpType_ACCUMULATION = 1,
  OpType_INDEX_ACCUMULATION = 2,
  OpType_SCALAR = 3,
  OpType_BROADCAST = 4,
  OpType_SUMMARYSTATS = 7,
  OpType_SHAPE = 8,
  OpType_AGGREGATION = 9,
  OpType_CUSTOM = 10,
  OpType_GRAPH = 11,
  OpType_VARIABLE = 119,
  OpType_MIN = OpType_TRANSFORM,
  OpType_MAX = OpType_VARIABLE
};

inline OpType (&EnumValuesOpType())[11] {
  static OpType values[] = {
    OpType_TRANSFORM,
    OpType_ACCUMULATION,
    OpType_INDEX_ACCUMULATION,
    OpType_SCALAR,
    OpType_BROADCAST,
    OpType_SUMMARYSTATS,
    OpType_SHAPE,
    OpType_AGGREGATION,
    OpType_CUSTOM,
    OpType_GRAPH,
    OpType_VARIABLE
  };
  return values;
}

enum OpClass {
  OpClass_TRANSFORM = 0,
  OpClass_REDUCTION = 1,
  OpClass_MULTIPLICATOR = 2,
  OpClass_GRAPH = 3,
  OpClass_MIN = OpClass_TRANSFORM,
  OpClass_MAX = OpClass_GRAPH
};

inline OpClass (&EnumValuesOpClass())[4] {
  static OpClass values[] = {
    OpClass_TRANSFORM,
    OpClass_REDUCTION,
    OpClass_MULTIPLICATOR,
    OpClass_GRAPH
  };
  return values;
}

inline const char **EnumNamesOpClass() {
  static const char *names[] = {
    "TRANSFORM",
    "REDUCTION",
    "MULTIPLICATOR",
    "GRAPH",
    nullptr
  };
  return names;
}

inline const char *EnumNameOpClass(OpClass e) {
  const size_t index = static_cast<int>(e);
  return EnumNamesOpClass()[index];
}

enum DataType {
  DataType_INHERIT = 0,
  DataType_HALF = 1,
  DataType_FLOAT = 2,
  DataType_DOUBLE = 3,
  DataType_MIN = DataType_INHERIT,
  DataType_MAX = DataType_DOUBLE
};

inline DataType (&EnumValuesDataType())[4] {
  static DataType values[] = {
    DataType_INHERIT,
    DataType_HALF,
    DataType_FLOAT,
    DataType_DOUBLE
  };
  return values;
}

inline const char **EnumNamesDataType() {
  static const char *names[] = {
    "INHERIT",
    "HALF",
    "FLOAT",
    "DOUBLE",
    nullptr
  };
  return names;
}

inline const char *EnumNameDataType(DataType e) {
  const size_t index = static_cast<int>(e);
  return EnumNamesDataType()[index];
}

struct LongPair FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_FIRST = 4,
    VT_SECOND = 6
  };
  int64_t first() const {
    return GetField<int64_t>(VT_FIRST, 0);
  }
  int64_t second() const {
    return GetField<int64_t>(VT_SECOND, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int64_t>(verifier, VT_FIRST) &&
           VerifyField<int64_t>(verifier, VT_SECOND) &&
           verifier.EndTable();
  }
};

struct LongPairBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_first(int64_t first) {
    fbb_.AddElement<int64_t>(LongPair::VT_FIRST, first, 0);
  }
  void add_second(int64_t second) {
    fbb_.AddElement<int64_t>(LongPair::VT_SECOND, second, 0);
  }
  LongPairBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  LongPairBuilder &operator=(const LongPairBuilder &);
  flatbuffers::Offset<LongPair> Finish() {
    const auto end = fbb_.EndTable(start_, 2);
    auto o = flatbuffers::Offset<LongPair>(end);
    return o;
  }
};

inline flatbuffers::Offset<LongPair> CreateLongPair(
    flatbuffers::FlatBufferBuilder &_fbb,
    int64_t first = 0,
    int64_t second = 0) {
  LongPairBuilder builder_(_fbb);
  builder_.add_second(second);
  builder_.add_first(first);
  return builder_.Finish();
}

struct IntPair FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_FIRST = 4,
    VT_SECOND = 6
  };
  int32_t first() const {
    return GetField<int32_t>(VT_FIRST, 0);
  }
  int32_t second() const {
    return GetField<int32_t>(VT_SECOND, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_FIRST) &&
           VerifyField<int32_t>(verifier, VT_SECOND) &&
           verifier.EndTable();
  }
};

struct IntPairBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_first(int32_t first) {
    fbb_.AddElement<int32_t>(IntPair::VT_FIRST, first, 0);
  }
  void add_second(int32_t second) {
    fbb_.AddElement<int32_t>(IntPair::VT_SECOND, second, 0);
  }
  IntPairBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  IntPairBuilder &operator=(const IntPairBuilder &);
  flatbuffers::Offset<IntPair> Finish() {
    const auto end = fbb_.EndTable(start_, 2);
    auto o = flatbuffers::Offset<IntPair>(end);
    return o;
  }
};

inline flatbuffers::Offset<IntPair> CreateIntPair(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t first = 0,
    int32_t second = 0) {
  IntPairBuilder builder_(_fbb);
  builder_.add_second(second);
  builder_.add_first(first);
  return builder_.Finish();
}

struct IntTriple FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_FIRST = 4,
    VT_SECOND = 6,
    VT_THIRD = 8
  };
  int32_t first() const {
    return GetField<int32_t>(VT_FIRST, 0);
  }
  int32_t second() const {
    return GetField<int32_t>(VT_SECOND, 0);
  }
  int32_t third() const {
    return GetField<int32_t>(VT_THIRD, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_FIRST) &&
           VerifyField<int32_t>(verifier, VT_SECOND) &&
           VerifyField<int32_t>(verifier, VT_THIRD) &&
           verifier.EndTable();
  }
};

struct IntTripleBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_first(int32_t first) {
    fbb_.AddElement<int32_t>(IntTriple::VT_FIRST, first, 0);
  }
  void add_second(int32_t second) {
    fbb_.AddElement<int32_t>(IntTriple::VT_SECOND, second, 0);
  }
  void add_third(int32_t third) {
    fbb_.AddElement<int32_t>(IntTriple::VT_THIRD, third, 0);
  }
  IntTripleBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  IntTripleBuilder &operator=(const IntTripleBuilder &);
  flatbuffers::Offset<IntTriple> Finish() {
    const auto end = fbb_.EndTable(start_, 3);
    auto o = flatbuffers::Offset<IntTriple>(end);
    return o;
  }
};

inline flatbuffers::Offset<IntTriple> CreateIntTriple(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t first = 0,
    int32_t second = 0,
    int32_t third = 0) {
  IntTripleBuilder builder_(_fbb);
  builder_.add_third(third);
  builder_.add_second(second);
  builder_.add_first(first);
  return builder_.Finish();
}

struct FlatVariable FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_ID = 4,
    VT_NAME = 6,
    VT_SHAPE = 8,
    VT_VALUES = 10,
    VT_DEVICE = 12
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  const flatbuffers::Vector<int32_t> *shape() const {
    return GetPointer<const flatbuffers::Vector<int32_t> *>(VT_SHAPE);
  }
  const flatbuffers::Vector<float> *values() const {
    return GetPointer<const flatbuffers::Vector<float> *>(VT_VALUES);
  }
  int32_t device() const {
    return GetField<int32_t>(VT_DEVICE, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.Verify(name()) &&
           VerifyOffset(verifier, VT_SHAPE) &&
           verifier.Verify(shape()) &&
           VerifyOffset(verifier, VT_VALUES) &&
           verifier.Verify(values()) &&
           VerifyField<int32_t>(verifier, VT_DEVICE) &&
           verifier.EndTable();
  }
};

struct FlatVariableBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(FlatVariable::VT_ID, id, 0);
  }
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(FlatVariable::VT_NAME, name);
  }
  void add_shape(flatbuffers::Offset<flatbuffers::Vector<int32_t>> shape) {
    fbb_.AddOffset(FlatVariable::VT_SHAPE, shape);
  }
  void add_values(flatbuffers::Offset<flatbuffers::Vector<float>> values) {
    fbb_.AddOffset(FlatVariable::VT_VALUES, values);
  }
  void add_device(int32_t device) {
    fbb_.AddElement<int32_t>(FlatVariable::VT_DEVICE, device, 0);
  }
  FlatVariableBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  FlatVariableBuilder &operator=(const FlatVariableBuilder &);
  flatbuffers::Offset<FlatVariable> Finish() {
    const auto end = fbb_.EndTable(start_, 5);
    auto o = flatbuffers::Offset<FlatVariable>(end);
    return o;
  }
};

inline flatbuffers::Offset<FlatVariable> CreateFlatVariable(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    flatbuffers::Offset<flatbuffers::Vector<int32_t>> shape = 0,
    flatbuffers::Offset<flatbuffers::Vector<float>> values = 0,
    int32_t device = 0) {
  FlatVariableBuilder builder_(_fbb);
  builder_.add_device(device);
  builder_.add_values(values);
  builder_.add_shape(shape);
  builder_.add_name(name);
  builder_.add_id(id);
  return builder_.Finish();
}

inline flatbuffers::Offset<FlatVariable> CreateFlatVariableDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    const char *name = nullptr,
    const std::vector<int32_t> *shape = nullptr,
    const std::vector<float> *values = nullptr,
    int32_t device = 0) {
  return nd4j::graph::CreateFlatVariable(
      _fbb,
      id,
      name ? _fbb.CreateString(name) : 0,
      shape ? _fbb.CreateVector<int32_t>(*shape) : 0,
      values ? _fbb.CreateVector<float>(*values) : 0,
      device);
}

struct FlatNode FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_ID = 4,
    VT_NAME = 6,
    VT_OPTYPE = 8,
    VT_OPNUM = 10,
    VT_INPUT = 12,
    VT_INPUTPAIRED = 14,
    VT_DATATYPE = 16,
    VT_OUTPUT = 18,
    VT_EXTRAPARAMS = 20,
    VT_EXTRAINTEGER = 22,
    VT_DIMENSIONS = 24,
    VT_DEVICE = 26,
    VT_SCALAR = 28
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  OpType opType() const {
    return static_cast<OpType>(GetField<int8_t>(VT_OPTYPE, 0));
  }
  int64_t opNum() const {
    return GetField<int64_t>(VT_OPNUM, 0);
  }
  const flatbuffers::Vector<int32_t> *input() const {
    return GetPointer<const flatbuffers::Vector<int32_t> *>(VT_INPUT);
  }
  const flatbuffers::Vector<flatbuffers::Offset<IntPair>> *inputPaired() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<IntPair>> *>(VT_INPUTPAIRED);
  }
  DataType dataType() const {
    return static_cast<DataType>(GetField<int8_t>(VT_DATATYPE, 0));
  }
  const flatbuffers::Vector<int32_t> *output() const {
    return GetPointer<const flatbuffers::Vector<int32_t> *>(VT_OUTPUT);
  }
  const flatbuffers::Vector<float> *extraParams() const {
    return GetPointer<const flatbuffers::Vector<float> *>(VT_EXTRAPARAMS);
  }
  const flatbuffers::Vector<int32_t> *extraInteger() const {
    return GetPointer<const flatbuffers::Vector<int32_t> *>(VT_EXTRAINTEGER);
  }
  const flatbuffers::Vector<int32_t> *dimensions() const {
    return GetPointer<const flatbuffers::Vector<int32_t> *>(VT_DIMENSIONS);
  }
  int32_t device() const {
    return GetField<int32_t>(VT_DEVICE, 0);
  }
  float scalar() const {
    return GetField<float>(VT_SCALAR, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.Verify(name()) &&
           VerifyField<int8_t>(verifier, VT_OPTYPE) &&
           VerifyField<int64_t>(verifier, VT_OPNUM) &&
           VerifyOffset(verifier, VT_INPUT) &&
           verifier.Verify(input()) &&
           VerifyOffset(verifier, VT_INPUTPAIRED) &&
           verifier.Verify(inputPaired()) &&
           verifier.VerifyVectorOfTables(inputPaired()) &&
           VerifyField<int8_t>(verifier, VT_DATATYPE) &&
           VerifyOffset(verifier, VT_OUTPUT) &&
           verifier.Verify(output()) &&
           VerifyOffset(verifier, VT_EXTRAPARAMS) &&
           verifier.Verify(extraParams()) &&
           VerifyOffset(verifier, VT_EXTRAINTEGER) &&
           verifier.Verify(extraInteger()) &&
           VerifyOffset(verifier, VT_DIMENSIONS) &&
           verifier.Verify(dimensions()) &&
           VerifyField<int32_t>(verifier, VT_DEVICE) &&
           VerifyField<float>(verifier, VT_SCALAR) &&
           verifier.EndTable();
  }
};

struct FlatNodeBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(FlatNode::VT_ID, id, 0);
  }
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(FlatNode::VT_NAME, name);
  }
  void add_opType(OpType opType) {
    fbb_.AddElement<int8_t>(FlatNode::VT_OPTYPE, static_cast<int8_t>(opType), 0);
  }
  void add_opNum(int64_t opNum) {
    fbb_.AddElement<int64_t>(FlatNode::VT_OPNUM, opNum, 0);
  }
  void add_input(flatbuffers::Offset<flatbuffers::Vector<int32_t>> input) {
    fbb_.AddOffset(FlatNode::VT_INPUT, input);
  }
  void add_inputPaired(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<IntPair>>> inputPaired) {
    fbb_.AddOffset(FlatNode::VT_INPUTPAIRED, inputPaired);
  }
  void add_dataType(DataType dataType) {
    fbb_.AddElement<int8_t>(FlatNode::VT_DATATYPE, static_cast<int8_t>(dataType), 0);
  }
  void add_output(flatbuffers::Offset<flatbuffers::Vector<int32_t>> output) {
    fbb_.AddOffset(FlatNode::VT_OUTPUT, output);
  }
  void add_extraParams(flatbuffers::Offset<flatbuffers::Vector<float>> extraParams) {
    fbb_.AddOffset(FlatNode::VT_EXTRAPARAMS, extraParams);
  }
  void add_extraInteger(flatbuffers::Offset<flatbuffers::Vector<int32_t>> extraInteger) {
    fbb_.AddOffset(FlatNode::VT_EXTRAINTEGER, extraInteger);
  }
  void add_dimensions(flatbuffers::Offset<flatbuffers::Vector<int32_t>> dimensions) {
    fbb_.AddOffset(FlatNode::VT_DIMENSIONS, dimensions);
  }
  void add_device(int32_t device) {
    fbb_.AddElement<int32_t>(FlatNode::VT_DEVICE, device, 0);
  }
  void add_scalar(float scalar) {
    fbb_.AddElement<float>(FlatNode::VT_SCALAR, scalar, 0.0f);
  }
  FlatNodeBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  FlatNodeBuilder &operator=(const FlatNodeBuilder &);
  flatbuffers::Offset<FlatNode> Finish() {
    const auto end = fbb_.EndTable(start_, 13);
    auto o = flatbuffers::Offset<FlatNode>(end);
    return o;
  }
};

inline flatbuffers::Offset<FlatNode> CreateFlatNode(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    OpType opType = OpType_TRANSFORM,
    int64_t opNum = 0,
    flatbuffers::Offset<flatbuffers::Vector<int32_t>> input = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<IntPair>>> inputPaired = 0,
    DataType dataType = DataType_INHERIT,
    flatbuffers::Offset<flatbuffers::Vector<int32_t>> output = 0,
    flatbuffers::Offset<flatbuffers::Vector<float>> extraParams = 0,
    flatbuffers::Offset<flatbuffers::Vector<int32_t>> extraInteger = 0,
    flatbuffers::Offset<flatbuffers::Vector<int32_t>> dimensions = 0,
    int32_t device = 0,
    float scalar = 0.0f) {
  FlatNodeBuilder builder_(_fbb);
  builder_.add_opNum(opNum);
  builder_.add_scalar(scalar);
  builder_.add_device(device);
  builder_.add_dimensions(dimensions);
  builder_.add_extraInteger(extraInteger);
  builder_.add_extraParams(extraParams);
  builder_.add_output(output);
  builder_.add_inputPaired(inputPaired);
  builder_.add_input(input);
  builder_.add_name(name);
  builder_.add_id(id);
  builder_.add_dataType(dataType);
  builder_.add_opType(opType);
  return builder_.Finish();
}

inline flatbuffers::Offset<FlatNode> CreateFlatNodeDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    const char *name = nullptr,
    OpType opType = OpType_TRANSFORM,
    int64_t opNum = 0,
    const std::vector<int32_t> *input = nullptr,
    const std::vector<flatbuffers::Offset<IntPair>> *inputPaired = nullptr,
    DataType dataType = DataType_INHERIT,
    const std::vector<int32_t> *output = nullptr,
    const std::vector<float> *extraParams = nullptr,
    const std::vector<int32_t> *extraInteger = nullptr,
    const std::vector<int32_t> *dimensions = nullptr,
    int32_t device = 0,
    float scalar = 0.0f) {
  return nd4j::graph::CreateFlatNode(
      _fbb,
      id,
      name ? _fbb.CreateString(name) : 0,
      opType,
      opNum,
      input ? _fbb.CreateVector<int32_t>(*input) : 0,
      inputPaired ? _fbb.CreateVector<flatbuffers::Offset<IntPair>>(*inputPaired) : 0,
      dataType,
      output ? _fbb.CreateVector<int32_t>(*output) : 0,
      extraParams ? _fbb.CreateVector<float>(*extraParams) : 0,
      extraInteger ? _fbb.CreateVector<int32_t>(*extraInteger) : 0,
      dimensions ? _fbb.CreateVector<int32_t>(*dimensions) : 0,
      device,
      scalar);
}

inline const nd4j::graph::FlatNode *GetFlatNode(const void *buf) {
  return flatbuffers::GetRoot<nd4j::graph::FlatNode>(buf);
}

inline bool VerifyFlatNodeBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<nd4j::graph::FlatNode>(nullptr);
}

inline void FinishFlatNodeBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<nd4j::graph::FlatNode> root) {
  fbb.Finish(root);
}

}  // namespace graph
}  // namespace nd4j

#endif  // FLATBUFFERS_GENERATED_NODE_ND4J_GRAPH_H_
