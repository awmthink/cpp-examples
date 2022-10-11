#ifndef SRC_AUTO_DIFF_AUTO_DIFF_H_
#define SRC_AUTO_DIFF_AUTO_DIFF_H_

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

#define UNUSED(x) (void)(x)

namespace ad {
class OpBase;
class Variable;
class VariableRef {
 public:
  VariableRef() = default;
  VariableRef operator+(const VariableRef& rhs) const;
  VariableRef operator-(const VariableRef& rhs) const;
  VariableRef operator*(const VariableRef& rhs) const;
  VariableRef operator/(const VariableRef& rhs) const;
  VariableRef operator-() const;
  VariableRef Sin() const;
  VariableRef Cos() const;
  VariableRef Log() const;
  VariableRef Exp() const;

  explicit operator bool() { return variable_ != nullptr; }

  float Value();

  std::string Name() const;

  VariableRef& Inputs(std::size_t i);

  std::size_t NumInputs() const;

  const VariableRef& Inputs(std::size_t i) const;

  std::shared_ptr<OpBase> Op();

  const std::shared_ptr<OpBase> Op() const;

  void Backpropagation();

  VariableRef GetAdjoint();
  const VariableRef GetAdjoint() const;

  bool operator==(const VariableRef& rhs) { return rhs.variable_ == variable_; }

  static VariableRef MakeVariableRef(float value = 0);
  static void PrintAllVariableRefs();
  static void ZeroGradient();

 private:
  VariableRef(Variable* var);

 private:
  struct Pool {
    std::vector<VariableRef> refs_;
    ~Pool();
  };
  Variable* variable_ = nullptr;
  static Pool variable_pool_;
};

class Variable {
  friend VariableRef;

 public:
  explicit Variable(const std::string& name, float value = .0F)
      : name_(name), cached_value_(value) {}

  Variable(const Variable&) = delete;
  Variable& operator=(const Variable&) = delete;

 private:
  std::vector<VariableRef> inputs_;
  VariableRef adjoint_;
  std::vector<VariableRef> adjoint_vec_;
  std::shared_ptr<OpBase> op_;
  std::string name_;
  float cached_value_;
};

class OpBase {
 public:
  OpBase(const char* name) : op_name_(name) {}
  virtual float Compute(std::vector<VariableRef>& inputs) const = 0;
  virtual std::vector<VariableRef> Gradient(
      const std::vector<VariableRef> inputs, VariableRef out_adjoint) const = 0;
  std::string GetName() const { return op_name_; };
  virtual ~OpBase() = default;

 private:
  std::string op_name_;
};

class PlusOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<VariableRef>& inputs) const final;
  std::vector<VariableRef> Gradient(const std::vector<VariableRef> inputs,
                                    VariableRef out_adjoint) const final;
};

class MinusOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<VariableRef>& inputs) const final;
  std::vector<VariableRef> Gradient(const std::vector<VariableRef> inputs,
                                    VariableRef out_adjoint) const final;
};

class MultipleOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<VariableRef>& inputs) const final;
  std::vector<VariableRef> Gradient(const std::vector<VariableRef> inputs,
                                    VariableRef out_adjoint) const final;
};

class DivideOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<VariableRef>& inputs) const final;
  std::vector<VariableRef> Gradient(const std::vector<VariableRef> inputs,
                                    VariableRef out_adjoint) const final;
};

class SinOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<VariableRef>& inputs) const final;
  std::vector<VariableRef> Gradient(const std::vector<VariableRef> inputs,
                                    VariableRef out_adjoint) const final;
};

class CosOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<VariableRef>& inputs) const final;
  std::vector<VariableRef> Gradient(const std::vector<VariableRef> inputs,
                                    VariableRef out_adjoint) const final;
};

class LogOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<VariableRef>& inputs) const final;
  std::vector<VariableRef> Gradient(const std::vector<VariableRef> inputs,
                                    VariableRef out_adjoint) const final;
};

class ExpOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<VariableRef>& inputs) const final;
  std::vector<VariableRef> Gradient(const std::vector<VariableRef> inputs,
                                    VariableRef out_adjoint) const final;
};

class Negitive : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<VariableRef>& inputs) const final;
  std::vector<VariableRef> Gradient(const std::vector<VariableRef> inputs,
                                    VariableRef out_adjoint) const final;
};

VariableRef::VariableRef(Variable* var) : variable_(var) {}

VariableRef::Pool VariableRef::variable_pool_;

VariableRef VariableRef::MakeVariableRef(float value) {
  std::string name = "v" + std::to_string(variable_pool_.refs_.size());
  auto var = new Variable{name, value};
  variable_pool_.refs_.emplace_back(VariableRef{var});
  return variable_pool_.refs_.back();
}

void VariableRef::PrintAllVariableRefs() {
  for (const auto& ref : variable_pool_.refs_) {
    std::cout << ref.Name();
    if (ref.NumInputs() == 0) {
      std::cout << std::endl;
    } else if (ref.NumInputs() == 1) {
      std::cout << ": " << ref.Op()->GetName() << ref.Inputs(0).Name()
                << std::endl;
    } else if (ref.NumInputs() == 2) {
      std::cout << ": " << ref.Inputs(0).Name() << ref.Op()->GetName()
                << ref.Inputs(1).Name() << std::endl;
    }
  }
}

void VariableRef::ZeroGradient() {
  for (auto& ref : variable_pool_.refs_) {
    ref.variable_->adjoint_vec_.clear();
  }
}

void VariableRef::Backpropagation() {
  std::queue<VariableRef> q;
  q.push(*this);
  std::vector<VariableRef> sorted_vec;
  while (!q.empty()) {
    auto ref = q.front();
    for (auto& input : ref.variable_->inputs_) {
      q.push(input);
    }
    auto iter = std::find(sorted_vec.begin(), sorted_vec.end(), ref);
    if (iter != sorted_vec.end()) {
      sorted_vec.erase(iter);
    }
    sorted_vec.push_back(ref);
    q.pop();
  }
  // for (auto& ref : sorted_vec) {
  //   std::cout << ref.Name() << ", ";
  // }
  std::cout << std::endl;
  auto all_refs = sorted_vec;
  for (std::size_t i = 0; i < all_refs.size(); ++i) {
    if (all_refs[i].variable_->adjoint_vec_.empty()) {
      all_refs[i].variable_->adjoint_vec_.push_back(MakeVariableRef(1));
    }
    all_refs[i].variable_->adjoint_ =
        all_refs[i].variable_->adjoint_vec_.front();
    for (std::size_t j = 1; j < all_refs[i].variable_->adjoint_vec_.size();
         ++j) {
      all_refs[i].variable_->adjoint_ = all_refs[i].variable_->adjoint_ +
                                        all_refs[i].variable_->adjoint_vec_[j];
    }
    if (all_refs[i].Op() == nullptr) {
      continue;
    }
    auto gradients = all_refs[i].Op()->Gradient(
        all_refs[i].variable_->inputs_, all_refs[i].variable_->adjoint_);
    for (std::size_t j = 0; j < gradients.size(); ++j) {
      all_refs[i].variable_->inputs_[j].variable_->adjoint_vec_.push_back(
          gradients[j]);
    }
  }
}

VariableRef VariableRef::operator+(const VariableRef& rhs) const {
  auto op = std::make_shared<PlusOp>("@plus@");
  auto var_ref = MakeVariableRef();
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  var_ref.variable_->inputs_.emplace_back(rhs);
  return var_ref;
}

VariableRef VariableRef::operator-(const VariableRef& rhs) const {
  auto op = std::make_shared<MinusOp>("@minus@");
  auto var_ref = MakeVariableRef();
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  var_ref.variable_->inputs_.emplace_back(rhs);
  return var_ref;
}

VariableRef VariableRef::operator*(const VariableRef& rhs) const {
  auto op = std::make_shared<MultipleOp>("@multiple@");
  auto var_ref = MakeVariableRef();
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  var_ref.variable_->inputs_.emplace_back(rhs);
  return var_ref;
}

VariableRef VariableRef::operator/(const VariableRef& rhs) const {
  auto op = std::make_shared<DivideOp>("@divide@");
  auto var_ref = MakeVariableRef();
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  var_ref.variable_->inputs_.emplace_back(rhs);
  return var_ref;
}

VariableRef VariableRef::operator-() const {
  auto op = std::make_shared<Negitive>("negitive@");
  auto var_ref = MakeVariableRef();
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  return var_ref;
}

VariableRef VariableRef::Sin() const {
  auto op = std::make_shared<SinOp>("sin@");
  auto var_ref = MakeVariableRef();
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  return var_ref;
}

VariableRef VariableRef::Cos() const {
  auto op = std::make_shared<CosOp>("cos@");
  auto var_ref = MakeVariableRef();
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  return var_ref;
}

VariableRef VariableRef::Log() const {
  auto op = std::make_shared<LogOp>("log@");
  auto var_ref = MakeVariableRef();
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  return var_ref;
}

VariableRef VariableRef::Exp() const {
  auto op = std::make_shared<ExpOp>("exp@");
  auto var_ref = MakeVariableRef();
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  return var_ref;
}

float VariableRef::Value() {
  if (variable_->op_) {
    variable_->cached_value_ = variable_->op_->Compute(variable_->inputs_);
  }
  return variable_->cached_value_;
}

std::string VariableRef::Name() const { return variable_->name_; }

VariableRef& VariableRef::Inputs(std::size_t i) {
  return variable_->inputs_[i];
}

std::size_t VariableRef::NumInputs() const { return variable_->inputs_.size(); }

const VariableRef& VariableRef::Inputs(std::size_t i) const {
  return variable_->inputs_[i];
}

std::shared_ptr<OpBase> VariableRef::Op() { return variable_->op_; }

const std::shared_ptr<OpBase> VariableRef::Op() const { return variable_->op_; }

VariableRef VariableRef::GetAdjoint() {
  if (variable_->adjoint_) {
    return variable_->adjoint_;
  }
  throw std::invalid_argument("Run backward before get adjoint");
}

const VariableRef VariableRef::GetAdjoint() const {
  if (variable_->adjoint_) {
    return variable_->adjoint_;
  }
  throw std::invalid_argument("Run backward before get adjoint");
}

float PlusOp::Compute(std::vector<VariableRef>& inputs) const {
  return inputs[0].Value() + inputs[1].Value();
}

std::vector<VariableRef> PlusOp::Gradient(const std::vector<VariableRef> inputs,
                                          VariableRef out_adjoint) const {
  UNUSED(inputs);
  return {out_adjoint, out_adjoint};
}

float MinusOp::Compute(std::vector<VariableRef>& inputs) const {
  return inputs[0].Value() - inputs[1].Value();
}

std::vector<VariableRef> MinusOp::Gradient(
    const std::vector<VariableRef> inputs, VariableRef out_adjoint) const {
  UNUSED(inputs);
  return {out_adjoint, -out_adjoint};
}

float MultipleOp::Compute(std::vector<VariableRef>& inputs) const {
  return inputs[0].Value() * inputs[1].Value();
}

std::vector<VariableRef> MultipleOp::Gradient(
    const std::vector<VariableRef> inputs, VariableRef out_adjoint) const {
  return {out_adjoint * inputs[1], out_adjoint * inputs[0]};
}

float DivideOp::Compute(std::vector<VariableRef>& inputs) const {
  return inputs[0].Value() / inputs[1].Value();
}

std::vector<VariableRef> DivideOp::Gradient(
    const std::vector<VariableRef> inputs, VariableRef out_adjoint) const {
  return {out_adjoint / inputs[1],
          -inputs[0] * out_adjoint / (inputs[1] * inputs[1])};
}

float SinOp::Compute(std::vector<VariableRef>& inputs) const {
  return std::sin(inputs[0].Value());
}

std::vector<VariableRef> SinOp::Gradient(const std::vector<VariableRef> inputs,
                                         VariableRef out_adjoint) const {
  return {out_adjoint * inputs[0].Cos()};
}

float CosOp::Compute(std::vector<VariableRef>& inputs) const {
  return std::cos(inputs[0].Value());
}

std::vector<VariableRef> CosOp::Gradient(const std::vector<VariableRef> inputs,
                                         VariableRef out_adjoint) const {
  return {out_adjoint * -inputs[0].Sin()};
}

float LogOp::Compute(std::vector<VariableRef>& inputs) const {
  return std::log(inputs[0].Value());
}

std::vector<VariableRef> LogOp::Gradient(const std::vector<VariableRef> inputs,
                                         VariableRef out_adjoint) const {
  return {out_adjoint / inputs[0]};
}

float ExpOp::Compute(std::vector<VariableRef>& inputs) const {
  return std::exp(inputs[0].Value());
}

std::vector<VariableRef> ExpOp::Gradient(const std::vector<VariableRef> inputs,
                                         VariableRef out_adjoint) const {
  return {out_adjoint * inputs[0].Exp()};
}

float Negitive::Compute(std::vector<VariableRef>& inputs) const {
  return -inputs[0].Value();
}

std::vector<VariableRef> Negitive::Gradient(
    const std::vector<VariableRef> inputs, VariableRef out_adjoint) const {
  UNUSED(inputs);
  return {-out_adjoint};
}

VariableRef::Pool::~Pool() {
  for (auto& ref : refs_) {
    delete ref.variable_;
  }
}

}  // namespace ad
#endif  // SRC_AUTO_DIFF_AUTO_DIFF_H_